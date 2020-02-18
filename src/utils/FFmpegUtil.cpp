//
// Created by dengchong on 2020-02-10.
//

#include "FFmpegUtil.h"

#include <iostream>

extern "C" {
#include "libavutil/timestamp.h"
}

using namespace std;

void FFmpegUtil::encode(AVCodecContext *codecCtx, AVFrame *frame, AVPacket *packet, std::ofstream &outputStream) {
    int ret{};
    /* send the frame for encoding */
    ret = avcodec_send_frame(codecCtx, frame);
    if (ret < 0) {
        cerr << "Error sending the frame to the encoder" << endl;
        exit(1);
    }
    /* read all the available output packets (in general there may be any
     * number of them */
    while (ret >= 0) {
        ret = avcodec_receive_packet(codecCtx, packet);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return;
        } else if (ret < 0) {
            cerr << "Error encoding audio frame" << endl;
            exit(1);
        }
        outputStream.write(reinterpret_cast<const char *>(packet->data), packet->size);
        av_packet_unref(packet);
    }
}

/*==================================codec context==========================================*/

static const int BEST_SAMPLE_RATE = 44100;

bool FFmpegUtil::checkSampleFormat(const AVCodec *codec, AVSampleFormat sampleFormat) {
    const AVSampleFormat *fmt = codec->sample_fmts;
    while (*fmt != AV_SAMPLE_FMT_NONE) {
        if (*fmt == sampleFormat) {
            return true;
        }
        fmt++;
    }
    return false;
}

int FFmpegUtil::selectSampleRate(const AVCodec *codec) {
    int bestSampleRate = 0;
    const int *supportedSampleRate = codec->supported_samplerates;
    while (supportedSampleRate) {
        if (!bestSampleRate || abs(BEST_SAMPLE_RATE - *supportedSampleRate) < abs(BEST_SAMPLE_RATE - bestSampleRate)) {
            bestSampleRate = *supportedSampleRate;
        }
        supportedSampleRate++;
    }
    return bestSampleRate;
}

int FFmpegUtil::selectChannelLayout(const AVCodec *codec) {
    int bestNbChannel{};
    uint64_t bestChannelLayout{};
    const uint64_t *channelLayout = codec->channel_layouts;
    while (channelLayout) {
        int nbChannel = av_get_channel_layout_nb_channels(*channelLayout);
        if (nbChannel > bestNbChannel) {
            bestNbChannel = nbChannel;
            bestChannelLayout = *channelLayout;
        }
        channelLayout++;
    }
    return bestChannelLayout;
}

/*==================================alloc==========================================*/

AVFrame *FFmpegUtil::allocPicture(AVPixelFormat pixFmt, int width, int height) {
    AVFrame *frame = av_frame_alloc();
    if (!frame) {
        cerr << "Error allocating an video frame" << endl;
        return nullptr;
    }
    frame->format = pixFmt;
    frame->width = width;
    frame->height = height;
    /* allocate the buffers for the frame data */
    int ret = av_frame_get_buffer(frame, 0);
    if (ret < 0) {
        cerr << "Could not allocate frame data error :" << av_err2str(ret) << endl;
        return nullptr;
    }
    return frame;
}

AVFrame *FFmpegUtil::allocAudioFrame(AVSampleFormat sampleFmt, uint64_t channelLayout, int sampleRate, int nbSamples) {
    AVFrame *frame = av_frame_alloc();
    if (!frame) {
        cerr << "Error allocating an audio frame ..." << endl;
        return nullptr;
    }
    frame->format = sampleFmt;
    frame->channel_layout = channelLayout;
    frame->sample_rate = sampleRate;
    frame->nb_samples = nbSamples;
    int ret = av_frame_get_buffer(frame, 0);
    if (ret < 0) {
        cerr << "Error allocating an audio buffer, error :" << av_err2str(ret) << endl;
        return nullptr;
    }
    return frame;
}

/*==================================read==========================================*/

bool FFmpegUtil::readVideoFrame(AVCodecContext *codecCtx, AVFrame *&frame, std::ifstream &inputStream) {
    if (!inputStream.eof()) {
        /* when we pass a frame to the encoder, it may keep a reference to it
        * internally; make sure we do not overwrite it here */
        int ret = av_frame_make_writable(frame);
        if (ret < 0) {
            exit(1);
        }
        if (!inputStream.good()) {
            cerr << "inputStream fail ..." << endl;
            exit(1);
        }
        for (int i = 0; i < codecCtx->height; ++i) {
            inputStream.read(reinterpret_cast<char *>(frame->data[0] + i * frame->linesize[0]),
                             codecCtx->width);
        }
        for (int j = 0; j < codecCtx->height / 2; ++j) {
            inputStream.read(reinterpret_cast<char *>(frame->data[1] + j * frame->linesize[1]),
                             codecCtx->width / 2);
        }
        for (int j = 0; j < codecCtx->height / 2; ++j) {
            inputStream.read(reinterpret_cast<char *>(frame->data[2] + j * frame->linesize[2]),
                             codecCtx->width / 2);
        }
        return true;
    }
    return false;
}

bool FFmpegUtil::readVideoFrame(AVCodecContext *codecCtx, AVFrame *&frame, std::ifstream &inputStream, int &pts,
                                SwsContext *&swsCtx, AVFrame *&tempFrame) {
    bool ret{};
    if (codecCtx->pix_fmt != AV_PIX_FMT_YUV420P) {
        /* as we only generate a YUV420P picture, we must convert it
        * to the codec pixel format if needed */
        if (!swsCtx) {
            swsCtx = sws_getContext(codecCtx->width,
                                    codecCtx->height,
                                    AV_PIX_FMT_YUV420P,
                                    codecCtx->width,
                                    codecCtx->height,
                                    codecCtx->pix_fmt,
                                    SWS_BICUBIC,
                                    nullptr, nullptr, nullptr);
            if (!swsCtx) {
                cerr << "Could not initialize the conversion context" << endl;
                exit(1);
            }
        }
        ret = readVideoFrame(codecCtx, tempFrame, inputStream);
        sws_scale(swsCtx, tempFrame->data, tempFrame->linesize, 0, codecCtx->height, frame->data, frame->linesize);
    } else {
        ret = readVideoFrame(codecCtx, frame, inputStream);
    }
    frame->pts = pts++;
    return ret;
}

bool FFmpegUtil::readAudioFrame(AVFrame *frame, std::ifstream &inputStream) {
    int size = av_get_bytes_per_sample(AV_SAMPLE_FMT_S16) * av_get_channel_layout_nb_channels(frame->channel_layout) *
               frame->nb_samples;
    inputStream.read(reinterpret_cast<char *>(frame->data[0]), size);
}

int FFmpegUtil::writeFrame(AVFormatContext *fmtCtx, const AVRational timeBase, AVStream *avStream, AVPacket *packet) {
    /* rescale output packet timestamp values from codec to stream timebase */
    av_packet_rescale_ts(packet, timeBase, avStream->time_base);
    packet->stream_index = avStream->index;
    /* Write the compressed frame to the media file. */
    logPacket(fmtCtx, packet);
    return av_interleaved_write_frame(fmtCtx, packet);
}

void FFmpegUtil::logPacket(const AVFormatContext *fmtCtx, const AVPacket *packet) {
    AVRational *time_base = &fmtCtx->streams[packet->stream_index]->time_base;
    cerr << "pts : " << av_ts2str(packet->pts) << endl;
    cerr << "pts_time : " << av_ts2timestr(packet->pts, time_base) << endl;
    cerr << "dts : " << av_ts2str(packet->dts) << endl;
    cerr << "dts_time : " << av_ts2timestr(packet->dts, time_base) << endl;
    cerr << "duration : " << av_ts2str(packet->duration) << endl;
    cerr << "duration_time : " << av_ts2timestr(packet->duration, time_base) << endl;
    cerr << "stream_index : " << packet->stream_index << endl;
}