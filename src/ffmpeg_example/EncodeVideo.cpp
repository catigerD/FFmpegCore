//
// Created by dengchong on 2020-02-14.
//

#include "EncodeVideo.h"

#include <iostream>

extern "C" {
#include "libavutil/opt.h"
}

#include "FFmpegUtil.h"

using namespace std;

void EncodeVideo::init() {
    AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec) {
        cerr << "could not find encoder : h264" << endl;
        exit(1);
    }
    codecCtx = avcodec_alloc_context3(codec);
    if (!codecCtx) {
        cerr << "could not alloc codec context ... " << endl;
        exit(1);
    }
    /* put sample parameters */
    codecCtx->bit_rate = 400000;
    /* resolution must be a multiple of two */
    codecCtx->width = 480;
    codecCtx->height = 272;
    /* frames per second */
    codecCtx->time_base = AVRational{1, 25};
    codecCtx->framerate = AVRational{25, 1};
    /* emit one intra frame every ten frames
     * check frame pict_type before passing frame
     * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
     * then gop_size is ignored and the output of encoder
     * will always be I frame irrespective to gop_size
     */
    codecCtx->gop_size = 10;
    codecCtx->max_b_frames = 1;
    codecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
    if (codec->id == AV_CODEC_ID_H264)
        av_opt_set(codecCtx->priv_data, "preset", "slow", 0);

    int ret = avcodec_open2(codecCtx, codec, nullptr);
    if (ret < 0) {
        cerr << "could not open h264 codec error : " << av_err2str(ret) << endl;
        exit(1);
    }

    packet = av_packet_alloc();
    if (!packet) {
        cerr << "could not alloc packet ..," << endl;
        exit(1);
    }

    frame = av_frame_alloc();
    if (!frame) {
        cerr << "could not alloc frame ..," << endl;
        exit(1);
    }
    frame->format = codecCtx->pix_fmt;
    frame->width = codecCtx->width;
    frame->height = codecCtx->height;
    ret = av_frame_get_buffer(frame, 0);
    if (ret < 0) {
        cerr << "could not alloc frame data error : " << av_err2str(ret) << endl;
        exit(1);
    }
}

void EncodeVideo::operator()() {
    int pts = 0;
    while (!inputStream.eof()) {
        int ret = av_frame_make_writable(frame);
        if (ret < 0) {
            exit(1);
        }
        if (!inputStream.good()) {
            cerr << "inputStream fail ..." << endl;
            exit(1);
        }
        for (int i = 0; i < codecCtx->height; ++i) {
            inputStream.read(reinterpret_cast<char *>(frame->data[0] + i * frame->linesize[0]), codecCtx->width);
        }
        for (int j = 0; j < codecCtx->height / 2; ++j) {
            inputStream.read(reinterpret_cast<char *>(frame->data[1] + j * frame->linesize[1]), codecCtx->width / 2);
        }
        for (int j = 0; j < codecCtx->height / 2; ++j) {
            inputStream.read(reinterpret_cast<char *>(frame->data[2] + j * frame->linesize[2]), codecCtx->width / 2);
        }
        frame->pts = pts++;
        FFmpegUtil::encode(codecCtx, frame, packet, outputStream);
    }
    FFmpegUtil::encode(codecCtx, nullptr, packet, outputStream);
}

void EncodeVideo::release() {
    av_frame_free(&frame);
    av_packet_free(&packet);
    avcodec_free_context(&codecCtx);
}