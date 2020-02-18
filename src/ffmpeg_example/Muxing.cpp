//
// Created by dengchong on 2020-02-17.
//

#include "Muxing.h"

#include <iostream>

extern "C" {
#include "libavutil/opt.h"
}

#include "FFmpegUtil.h"

using namespace std;

static const AVPixelFormat DEFAULT_PIX_FMT = AV_PIX_FMT_YUV420P;

void Muxing::init() {
    /* allocate the output media context */
    avformat_alloc_output_context2(&fmtCxt, nullptr, nullptr, outputPath.c_str());
    if (!fmtCxt) {
        cerr << "Could not deduce output format from file extension ... " << endl;
        exit(1);
    }
    /* Add the audio and video streams using the default format codecs
     * and initialize the codecs. */
    if (fmtCxt->video_codec_id != AV_CODEC_ID_NONE) {
        addStream(videoStream, videoCodecCtx, fmtCxt->video_codec_id);
        hasVideo = true;
        encodeVideo = true;
    }
    if (fmtCxt->audio_codec_id != AV_CODEC_ID_NONE) {
        addStream(audioStream, audioCodecCtx, fmtCxt->audio_codec_id);
        hasAudio = true;
        encodeAudio = true;
    }
    /* Now that all the parameters are set, we can open the audio and
    * video codecs and allocate the necessary encode buffers. */
    if (hasVideo) {
        openVideo();
    }
    if (hasAudio) {
        openAudio();
    }
    av_dump_format(fmtCxt, 0, outputPath.c_str(), 1);
}

void Muxing::release() {
    avcodec_free_context(&videoCodecCtx);
    av_frame_free(&videoFrame);
    av_frame_free(&tempVideoFrame);
    avcodec_free_context(&audioCodecCtx);
    av_frame_free(&audioFrame);
    av_frame_free(&tempAudioFrame);
    sws_freeContext(swsCxt);
    swr_free(&swrCxt);
    if (!(fmtCxt->flags & AVFMT_NOFILE))
        /* Close the output file. */
        avio_closep(&fmtCxt->pb);
    avformat_free_context(fmtCxt);
}

void Muxing::operator()() {
    int ret{};
    /* open the output file, if needed */
    if (!(fmtCxt->flags & AVFMT_NOFILE)) {
        ret = avio_open(&fmtCxt->pb, outputPath.c_str(), AVIO_FLAG_WRITE);
        if (ret < 0) {
            cerr << "Could not open " << outputPath << " , error : " << av_err2str(ret) << endl;
            exit(1);
        }
    }
    /* Write the stream header, if any. */
    ret = avformat_write_header(fmtCxt, nullptr);
    if (ret < 0) {
        cerr << "Error occurred when opening output file, error : " << av_err2str(ret) << endl;
        exit(1);
    }
    while (encodeVideo || encodeAudio) {
        /* select the stream to encode */
        if (encodeVideo
            && (!encodeAudio ||
                av_compare_ts(videoNextPts, videoCodecCtx->time_base, audioNextPts, audioCodecCtx->time_base) < 0)) {
            encodeVideo = writeVideoFrame();
        } else {
            encodeAudio = writeAudioFrame();
        }
    }
    /* Write the trailer, if any. The trailer must be written before you
     * close the CodecContexts open when you wrote the header; otherwise
     * av_write_trailer() may try to use memory that was freed on
     * av_codec_close(). */
    av_write_trailer(fmtCxt);
}

void Muxing::addStream(AVStream *&avStream, AVCodecContext *&codecCtx, AVCodec *&codec, AVCodecID codecId) {
    codec = avcodec_find_encoder(codecId);
    if (!codec) {
        cerr << "Could not find encoder for " << avcodec_get_name(codecId) << endl;
        exit(1);
    }
    avStream = avformat_new_stream(fmtCxt, codec);
    if (!avStream) {
        cerr << "Could not allocate stream ..." << endl;
        exit(1);
    }
    avStream->id = fmtCxt->nb_streams - 1;
    codecCtx = avcodec_alloc_context3(codec);
    if (!codecCtx) {
        cerr << "Could not alloc an encoding context ..." << endl;
        exit(1);
    }
    switch (codecCtx->codec_type) {
        case AVMEDIA_TYPE_AUDIO:
            codecCtx->sample_fmt = codec->sample_fmts ? codec->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;
            codecCtx->bit_rate = 64000;
            codecCtx->sample_rate = FFmpegUtil::selectSampleRate(codec);
            codecCtx->channel_layout = FFmpegUtil::selectChannelLayout(codec);
            codecCtx->channels = av_get_channel_layout_nb_channels(codecCtx->channel_layout);
            avStream->time_base = AVRational{1, codecCtx->sample_rate};
            break;
        case AVMEDIA_TYPE_VIDEO:
            codecCtx->bit_rate = 400000;
            codecCtx->width = 480;
            codecCtx->height = 272;
            codecCtx->framerate = AVRational{25, 1};
            codecCtx->time_base = AVRational{1, 25};
            /* timebase: This is the fundamental unit of time (in seconds) in terms
            * of which frame timestamps are represented. For fixed-fps content,
            * timebase should be 1/framerate and timestamp increments should be
            * identical to 1. */
            avStream->time_base = codecCtx->time_base;
            codecCtx->gop_size = 10;
            codecCtx->max_b_frames = 1;
            codecCtx->pix_fmt = DEFAULT_PIX_FMT;
            if (codec->id == AV_CODEC_ID_H264)
                av_opt_set(codecCtx->priv_data, "preset", "slow", 0);
            break;
        default:
            break;
    }
    /* Some formats want stream headers to be separate. */
    if (fmtCxt->oformat->flags & AVFMT_GLOBALHEADER) {
        codecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }
}

void Muxing::openVideo() {
    int ret = avcodec_open2(videoCodecCtx, videoCodec, nullptr);
    if (ret < 0) {
        cerr << "Could not open video codec " << av_err2str(ret) << endl;
        exit(1);
    }
    /* allocate and init a re-usable frame */
    videoFrame = FFmpegUtil::allocPicture(videoCodecCtx->pix_fmt, videoCodecCtx->width, videoCodecCtx->height);
    if (!videoFrame) {
        cerr << "Could not allocate video frame ..." << endl;
        exit(1);
    }
    /* If the output format is not YUV420P, then a temporary YUV420P
     * picture is needed too. It is then converted to the required
     * output format. */
    if (videoCodecCtx->pix_fmt != AV_PIX_FMT_YUV420P) {
        tempVideoFrame = FFmpegUtil::allocPicture(videoCodecCtx->pix_fmt, videoCodecCtx->width,
                                                  videoCodecCtx->height);
        if (!tempVideoFrame) {
            cerr << "Could not allocate video frame ..." << endl;
            exit(1);
        }
    }
    /* copy the stream parameters to the muxer */
    ret = avcodec_parameters_from_context(videoStream->codecpar, videoCodecCtx);
    if (ret < 0) {
        cerr << "Could not copy the stream parameters , error : " << av_err2str(ret) << endl;
        exit(1);
    }
}

void Muxing::openAudio() {
    int ret = avcodec_open2(audioCodecCtx, audioCodec, nullptr);
    if (ret < 0) {
        cerr << "Could not open audio codec " << av_err2str(ret) << endl;
        exit(1);
    }
    int nbSamples{};
    if (audioCodec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE) {
        nbSamples = 1000;
    } else {
        nbSamples = audioCodecCtx->frame_size;
    }
    audioFrame = FFmpegUtil::allocAudioFrame(audioCodecCtx->sample_fmt, audioCodecCtx->channel_layout,
                                             audioCodecCtx->sample_rate, nbSamples);
    tempAudioFrame = FFmpegUtil::allocAudioFrame(AV_SAMPLE_FMT_S16, audioCodecCtx->channel_layout,
                                                 audioCodecCtx->sample_rate, nbSamples);
    /* copy the stream parameters to the muxer */
    ret = avcodec_parameters_from_context(audioStream->codecpar, audioCodecCtx);
    if (ret < 0) {
        cerr << "Could not copy the stream parameters , error : " << av_err2str(ret) << endl;
        exit(1);
    }
    /* create resampler context */
    swrCxt = swr_alloc();
    if (!swrCxt) {
        cerr << "Could not allocate resampler context" << endl;
        exit(1);
    }
    /* set options */
    av_opt_set_int(swrCxt, "in_channel_count", audioCodecCtx->channels, 0);
    av_opt_set_int(swrCxt, "in_sample_rate", audioCodecCtx->sample_rate, 0);
    av_opt_set_sample_fmt(swrCxt, "in_sample_fmt", AV_SAMPLE_FMT_S16, 0);
    av_opt_set_int(swrCxt, "out_channel_count", audioCodecCtx->channels, 0);
    av_opt_set_int(swrCxt, "out_sample_rate", audioCodecCtx->sample_rate, 0);
    av_opt_set_sample_fmt(swrCxt, "out_sample_fmt", audioCodecCtx->sample_fmt, 0);
    /* initialize the resampling context */
    ret = swr_init(swrCxt);
    if (ret < 0) {
        cerr << "Failed to initialize the resampling context" << endl;
        exit(1);
    }
}

bool Muxing::writeVideoFrame() {
    FFmpegUtil::readVideoFrame(videoCodecCtx, videoFrame, videoInputStream, videoNextPts, swsCxt, tempVideoFrame);
    AVPacket *packet = av_packet_alloc();
    /* encode the image */
    int gotPacket{};
    int ret = avcodec_encode_video2(videoCodecCtx, packet, videoFrame, &gotPacket);
    if (ret < 0) {
        cerr << "Error encoding video frame, error : " << av_err2str(ret) << endl;
        exit(1);
    }
    if (gotPacket) {
        ret = FFmpegUtil::writeFrame(fmtCxt, videoCodecCtx->time_base, videoStream, packet);
    } else {
        ret = 0;
    }
    if (ret < 0) {
        cerr << "Error while writing video frame , error : " << av_err2str(ret) << endl;
        exit(1);
    }
    return gotPacket;
}

bool Muxing::writeAudioFrame() {

}