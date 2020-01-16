#include <utility>

//
// Created by dengchong on 2020-01-16.
//

#include "DemuxingDecoding.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
}

#include <iostream>
#include <array>

using namespace std;

/* Enable or disable frame reference counting. You are not supposed to support
 * both paths in your application but pick the one most appropriate to your
 * needs. Look for the use of refcount in this example to see what are the
 * differences of API usage between them. */
static int refcount = 0;

static int
openCodecContext(int &streamIndex, AVCodecContext *&codecCtx, AVFormatContext *formatContext, AVMediaType type) {
    int ret{};
    ret = av_find_best_stream(formatContext, type, -1, -1, nullptr, 0);
    if (ret < 0) {
        cerr << "Could not find " << av_get_media_type_string(type) << " stream in input file "
             << formatContext->url << endl;
        return ret;
    }
    int streamIdx = ret;
    AVStream *stream = formatContext->streams[streamIdx];
    /* find decoder for the stream*/
    AVCodec *codec = avcodec_find_decoder(stream->codecpar->codec_id);
    if (!codec) {
        cerr << "Failed to find " << av_get_media_type_string(type) << " codec" << endl;
        return AVERROR(EINVAL);
    }
    /* Allocate a codec Context for the decoder*/
    codecCtx = avcodec_alloc_context3(codec);
    if (!codecCtx) {
        cerr << "Failed to allocate the " << av_get_media_type_string(type) << " codec context" << endl;
        return AVERROR(ENOMEM);
    }
    /* Copy codec parameters from input stream to output codec context*/
    if ((ret = avcodec_parameters_to_context(codecCtx, stream->codecpar)) < 0) {
        cerr << "Failed to copy " << av_get_media_type_string(type) << " codec parameters to decoder context" << endl;
        return ret;
    }
    /* Init this decoders, with or without reference counting*/
    AVDictionary *opts{};
    av_dict_set(&opts, "refcounted_frames", refcount ? "1" : "0", 0);
    if ((ret = avcodec_open2(codecCtx, codec, &opts)) < 0) {
        cerr << "Failed to open " << av_get_media_type_string(type) << " codec" << endl;
        return ret;
    }
    streamIndex = streamIdx;
    return 0;
}

struct SampleFmtEntry {

    explicit SampleFmtEntry(AVSampleFormat fmt = AV_SAMPLE_FMT_NONE, string fmtB = string{}, string fmtL = string{})
            : sampleFmt(fmt), fmtBe(std::move(fmtB)), fmtLe(std::move(fmtL)) {}

    AVSampleFormat sampleFmt{AV_SAMPLE_FMT_NONE};
    const string fmtBe{};
    const string fmtLe{};
};

static const array<SampleFmtEntry, 5> sampleFmtEntries = {
        SampleFmtEntry{AV_SAMPLE_FMT_U8, "u8", "u8"},
        SampleFmtEntry{AV_SAMPLE_FMT_S16, "s16be", "s16le"},
        SampleFmtEntry{AV_SAMPLE_FMT_S32, "s32be", "s32le"},
        SampleFmtEntry{AV_SAMPLE_FMT_FLT, "f32be", "f32le"},
        SampleFmtEntry{AV_SAMPLE_FMT_DBL, "f64be", "f64le"},
};

static int getFormatFromSampleFmt(string &fmt, const AVSampleFormat &sampleFmt) {
    for (const auto &sampleFmtEntry : sampleFmtEntries) {
        if (sampleFmt == sampleFmtEntry.sampleFmt) {
            fmt = AV_NE(sampleFmtEntry[i].fmtBe, sampleFmtEntry.fmtLe);
            return 0;
        }
    }
    cerr << "sample fmt " << av_get_sample_fmt_name(sampleFmt) << " is not supported as output format" << endl;
    return -1;
}

DemuxingDecoding::~DemuxingDecoding() {
    avcodec_free_context(&audioCodecCtx);
    avcodec_free_context(&videoCodecCtx);
    avformat_close_input(&fmtCtx);
    av_frame_free(&frame);
    av_free(videoDstData[0]);
}

int DemuxingDecoding::decodePacket(int &gotFrame, int cached) {
    int ret{};
    int decoded = packet.size;
    gotFrame = 0;
    if (packet.stream_index == videoStreamIdx) {
        /* decode video frame */
        ret = avcodec_decode_video2(videoCodecCtx, frame, &gotFrame, &packet);
        if (ret < 0) {
            cerr << "Error decoding video frame " << av_err2str(ret) << endl;
            return ret;
        }
        if (gotFrame) {
            if (frame->width != width || frame->height != height
                || frame->format != pixFmt) {
                /* to handle this change, one could call av_image_alloc again and decode the following frames into
                 * another rawvideo file */
                cerr << "Error : width, height, and pixel format have to be constant in a rawvideo file, "
                     << "but the width, height, or pixel format of the input video change " << endl
                     << "old width = " << width << ", height = " << height << ", format = "
                     << av_get_pix_fmt_name(pixFmt) << endl
                     << "new width = " << frame->width << ", height = " << frame->height << ", format = "
                     << av_get_pix_fmt_name(static_cast<AVPixelFormat>(frame->format)) << endl;
                return -1;
            }
            string cacheStr(cached ? "(cached)" : "");
            cout << "video_frame " << cacheStr << " n : " << videoFrameCount++ << " codec_n : "
                 << frame->coded_picture_number << endl;
            /* copy decoded frame to destination buffer :
             * this is required since rawvideo expects non aligned data*/
            av_image_copy(videoDstData, videoDstLinesize, (const uint8_t **) frame->data, frame->linesize,
                          static_cast<AVPixelFormat>(frame->format), width, height);
            /* write to rawvideo file*/
            videoOutputStream.write(reinterpret_cast<const char *>(videoDstData[0]), videoDstBufsize);
        }
    } else if (packet.stream_index == audioStreamIdx) {
        ret = avcodec_decode_audio4(audioCodecCtx, frame, &gotFrame, &packet);
        if (ret < 0) {
            cerr << "Error decoding audio frame " << av_err2str(ret) << endl;
            return ret;
        }
        /* Some audio decoders decode only part of packet, and have to be called again with the remainder of the packer
         * data.
         * Sample : fate-suite/lossless-audio/luckynight-partial.shn
         * Also, some decoders might over-read the packet.*/
        decoded = FFMIN(ret, packet.size);
        if (gotFrame) {
            size_t unpaddedLinesize = frame->nb_samples * av_get_bytes_per_sample(
                    static_cast<AVSampleFormat>(frame->format));
            string cacheStr(cached ? "(cached)" : "");
            cout << "audio_frame " << cacheStr << " n : " << audioFrameCount++ << " nb_samples: " << frame->nb_samples
                 << " pts : " << av_ts2timestr(frame->pts, &audioCodecCtx->time_base) << endl;
            /* Write the raw audio data samples of the first plane. This works
             * fine for packed formats (e.g. AV_SAMPLE_FMT_S16). However,
             * most audio decoders output planar audio, which uses a separate
             * plane of audio samples for each channel (e.g. AV_SAMPLE_FMT_S16P).
             * In other words, this code will write only the first audio channel
             * in these cases.
             * You should use libswresample or libavfilter to convert the frame
             * to packed data. */
            audioOutputStream.write(reinterpret_cast<const char *>(frame->extended_data[0]), unpaddedLinesize);
        }
    }
    /* If we use frame reference counting , we own the data and need to do-reference it when we don't use it anymore */
    if (gotFrame && refcount) {
        av_frame_unref(frame);
    }
    return decoded;
}

int DemuxingDecoding::operator()() {
    int ret{}, gotFrame{};
    /* open inoup file, and allocate format context*/
    ret = avformat_open_input(&fmtCtx, inputUrl.c_str(), nullptr, nullptr);
    if (ret < 0) {
        cerr << "Could not open source file " << inputUrl << endl;
        exit(1);
    }
    /* retrieve stream information */
    ret = avformat_find_stream_info(fmtCtx, nullptr);
    if (ret < 0) {
        cerr << "Could not find stream information" << endl;
        exit(1);
    }
    if (openCodecContext(videoStreamIdx, videoCodecCtx, fmtCtx, AVMEDIA_TYPE_VIDEO) >= 0) {
        videoStream = fmtCtx->streams[videoStreamIdx];
        /* allocate image where the decoded image will be put */
        width = videoCodecCtx->width;
        height = videoCodecCtx->height;
        pixFmt = videoCodecCtx->pix_fmt;
        ret = av_image_alloc(videoDstData, videoDstLinesize, width, height, pixFmt, 1);
        if (ret < 0) {
            cerr << "Could not allocate raw video buffer" << endl;
            return ret;
        }
        videoDstBufsize = ret;
    }
    if (openCodecContext(audioStreamIdx, audioCodecCtx, fmtCtx, AVMEDIA_TYPE_AUDIO) >= 0) {
        audioStream = fmtCtx->streams[audioStreamIdx];
    }

    /* dump input information to stderr*/
    av_dump_format(fmtCtx, 0, inputUrl.c_str(), 0);

    if (!audioStream || !videoStream) {
        cerr << "Could not find audio or video stream in the input , aborting" << endl;
        ret = 1;
        return ret;
    }

    frame = av_frame_alloc();
    if (!frame) {
        cerr << "Could not allocate frame" << endl;
        return AVERROR(ENOMEM);
    }
    /* initialize packet. set data to null , let the demuxer fill it*/
    av_init_packet(&packet);
    packet.data = nullptr;
    packet.size = 0;

    if (videoStream) {
        cout << "Demuxing video from file " << inputUrl << " into " << outVideoUrl << endl;
    }
    if (audioStream) {
        cout << "Demuxing audio from file " << inputUrl << " into " << outAudioUrl << endl;
    }

    /* read frames from the file*/
    while (av_read_frame(fmtCtx, &packet) >= 0) {
        AVPacket origPkt = packet;
        do {
            ret = decodePacket(gotFrame, 0);
            if (ret < 0) {
                break;
            }
            packet.data += ret;
            packet.size -= ret;
        } while (packet.size > 0);
        av_packet_unref(&origPkt);
    }

    /* flush cached frames */
    packet.data = nullptr;
    packet.size = 0;
    do {
        decodePacket(gotFrame, 1);
    } while (gotFrame);

    if (videoStream) {
        cout << "Play the output video file with the command : " << endl
             << "ffplay -f rawvideo -pix_fmt " << av_get_pix_fmt_name(pixFmt) << " -video_size " << width << "x"
             << height << " " << outVideoUrl << endl;
    }
    if (audioStream) {
        AVSampleFormat sFmt = audioCodecCtx->sample_fmt;
        int nChannels = audioCodecCtx->channels;
        if (av_sample_fmt_is_planar(sFmt)) {
            string name = av_get_sample_fmt_name(sFmt);
            cout << "Warning : the sample format the decoder produced is planar (" << name
                 << ") . This example will output the first channel only" << endl;
            sFmt = av_get_packed_sample_fmt(sFmt);
            nChannels = 1;
        }
        string fmt;
        if ((ret = getFormatFromSampleFmt(fmt, sFmt)) < 0) {
            return ret;
        }
        cout << "Play the output audio file with the command : " << endl
             << "ffplay -f " << fmt << " -ac " << nChannels << " " << audioCodecCtx->sample_rate << " " << outAudioUrl
             << endl;
    }
}