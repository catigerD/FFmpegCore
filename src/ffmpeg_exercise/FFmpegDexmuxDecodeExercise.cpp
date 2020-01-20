//
// Created by dengchong on 2020-01-20.
//

#include "FFmpegDexmuxDecodeExercise.h"

extern "C" {
#include "libavutil/imgutils.h"
}

#include <iostream>

using namespace std;

FFmpegDexmuxDecodeExercise::~FFmpegDexmuxDecodeExercise() {
    av_frame_free(&frame);
    av_packet_free(&packet);
    avcodec_free_context(&aCodecCtx);
    avcodec_free_context(&vCodecCtx);
    avformat_close_input(&fmtCtx);
}


void FFmpegDexmuxDecodeExercise::demuxingDecode() {
    while (av_read_frame(fmtCtx, packet) >= 0) {
        if (packet->stream_index == vStreamIdx) {
            ::decode(videoOutputStream, vCodecCtx, packet, frame, &FFmpegDexmuxDecodeExercise::writeVideo);
        } else if (packet->stream_index == aStreamIdx) {
            ::decode(audioOutputStream, aCodecCtx, packet, frame, &FFmpegDexmuxDecodeExercise::writeAudio);
        }
        av_packet_unref(packet);
    }
    //flush the decoder
    if (packet->stream_index == vStreamIdx) {
        ::decode(videoOutputStream, vCodecCtx, nullptr, frame, &FFmpegDexmuxDecodeExercise::writeVideo);
    } else if (packet->stream_index == aStreamIdx) {
        ::decode(audioOutputStream, aCodecCtx, nullptr, frame, &FFmpegDexmuxDecodeExercise::writeAudio);
    }
}

void FFmpegDexmuxDecodeExercise::init() {
    ret = avformat_open_input(&fmtCtx, inputUrl.c_str(), nullptr, nullptr);
    if (ret < 0) {
        cerr << "avformat_open_input fail ... error : " << av_err2str(ret) << endl;
        exit(1);
    }
    ret = avformat_find_stream_info(fmtCtx, nullptr);
    if (ret < 0) {
        cerr << "avformat_find_stream_info fail ... error : " << av_err2str(ret) << endl;
        exit(1);
    }
    openCodec(fmtCtx, vCodecCtx, vStreamIdx, AVMEDIA_TYPE_VIDEO);
    openCodec(fmtCtx, aCodecCtx, aStreamIdx, AVMEDIA_TYPE_AUDIO);
    cout << "acodecCtx fmt is planar : " << av_sample_fmt_is_planar(aCodecCtx->sample_fmt) << endl;

    packet = av_packet_alloc();
    if (!packet) {
        cerr << "av_packet_alloc fail ... error : " << AVERROR(ENOMEM) << endl;
        exit(1);
    }
    frame = av_frame_alloc();
    if (!frame) {
        cerr << "av_frame_alloc fail ... error : " << AVERROR(ENOMEM) << endl;
        exit(1);
    }

    av_dump_format(fmtCtx, 0, inputUrl.c_str(), 0);
}

void
FFmpegDexmuxDecodeExercise::writeVideo(std::ofstream &outputStream, const AVCodecContext *codecContext,
                                       const AVFrame *avFrame) {
    //1. 遍历读取
//    for (int i = 0; i < codecContext->height; ++i) {
//        outputStream.write(reinterpret_cast<const char *>(avFrame->data[0] + i * avFrame->linesize[0]),
//                           codecContext->width);
//    }
//    /* save U(Cb) data */
//    for (int i = 0; i < codecContext->height / 2; ++i) {
//        outputStream.write(reinterpret_cast<const char *>(avFrame->data[1] + i * avFrame->linesize[1]),
//                           codecContext->width / 2);
//    }
//    /* save V(Cr) data */
//    for (int i = 0; i < codecContext->height / 2; ++i) {
//        outputStream.write(reinterpret_cast<const char *>(avFrame->data[2] + i * avFrame->linesize[2]),
//                           codecContext->width / 2);
//    }
    //2. 使用 av_image_xxx
    uint8_t *dstData[4]{};
    int dstLinesize[4];
    //aligin == 1, linesize == dataSize
    int dstDataSize = av_image_alloc(dstData, dstLinesize, codecContext->width, codecContext->height,
                                     codecContext->pix_fmt, 1);
    av_image_copy(dstData, dstLinesize, (const uint8_t **) (avFrame->data), avFrame->linesize,
                  static_cast<AVPixelFormat>(avFrame->format), avFrame->width,
                  avFrame->height);
    outputStream.write(reinterpret_cast<const char *>(dstData[0]), dstDataSize);
    av_freep(&dstData[0]);
}

void
FFmpegDexmuxDecodeExercise::writeAudio(std::ofstream &outputStream, const AVCodecContext *codecContext,
                                       const AVFrame *avFrame) {
    int planar = av_sample_fmt_is_planar(codecContext->sample_fmt);
    int bytes = av_get_bytes_per_sample(codecContext->sample_fmt);
    if (planar) {
        for (int i = 0; i < avFrame->nb_samples; ++i) {
            for (int j = 0; j < avFrame->channels; ++j) {
                outputStream.write(reinterpret_cast<const char *>(avFrame->extended_data[j] + i * bytes), bytes);
            }
        }
    } else {
        int unpaddingLineSize = bytes * avFrame->nb_samples;
        outputStream.write(reinterpret_cast<const char *>(avFrame->extended_data[0]), unpaddingLineSize);
    }
}


int openCodec(AVFormatContext *fmtCtx, AVCodecContext *&codecCtx, int &index, const AVMediaType &type) {
    AVCodec *codec{};
    int ret = av_find_best_stream(fmtCtx, type, -1, 0, &codec, 0);
    if (ret < 0) {
        cerr << "av_find_best_stream fail ... error : " << av_err2str(ret) << endl;
        return ret;
    }
    index = ret;
    AVStream *avStream = fmtCtx->streams[index];
    codecCtx = avcodec_alloc_context3(codec);
    if (!codecCtx) {
        cerr << "avcodec_alloc_context3 fail ... error : " << av_err2str(AVERROR(ENOMEM)) << endl;
        return AVERROR(ENOMEM);
    }
    if ((ret = avcodec_parameters_to_context(codecCtx, avStream->codecpar)) < 0) {
        cerr << "avcodec_parameters_to_context fail ... error : " << av_err2str(ret) << endl;
        return ret;
    }
    if ((ret = avcodec_open2(codecCtx, codec, nullptr)) < 0) {
        cerr << "avcodec_open2 fail ... error : " << av_err2str(ret) << endl;
        return ret;
    }
    return 0;
}

void decode(ofstream &outputStream, AVCodecContext *codecCtx, AVPacket *packet, AVFrame *&frame,
            const std::function<void(std::ofstream &outputStream, const AVCodecContext *, const AVFrame *)> &writeFun) {
    int ret = avcodec_send_packet(codecCtx, packet);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
        cout << "avcodec_send_packet again or has eof" << endl;
        return;
    } else if (ret < 0) {
        cerr << "avcodec_send_packet fail ... error : " << av_err2str(ret) << endl;
        exit(1);
    }
    while ((ret = avcodec_receive_frame(codecCtx, frame)) >= 0) {
        writeFun(outputStream, codecCtx, frame);
    }
}