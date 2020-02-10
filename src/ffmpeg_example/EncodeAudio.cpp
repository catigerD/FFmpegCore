//
// Created by dengchong on 2020-02-10.
//

#include "EncodeAudio.h"

#include <iostream>

#include "ValidUtil.h"
#include "CodecUtil.h"

using namespace std;

void EncodeAudio::operator()() {
    int unpadLineSize = av_get_bytes_per_sample(codecCtx->sample_fmt) * codecCtx->frame_size * codecCtx->channels;
    while (!inputStream.eof()) {
        /* make sure the frame is writable -- makes a copy if the encoder kept a reference internally */
        int ret = av_frame_make_writable(frame);
        if (ret < 0) {
            exit(1);
        }
        inputStream.read(reinterpret_cast<char *>(frame->data[0]), frame->linesize[0]);
        if (inputStream.eof()) {
            cerr << "inputStream eof ... " << endl;
            /* flush the encoder */
            CodecUtil::encode(codecCtx, nullptr, packet, outputStream);
            return;
        }
        CodecUtil::encode(codecCtx, frame, packet, outputStream);
    }
}

void EncodeAudio::init() {
    //find the mp2 encoder
    AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_MP2);
    if (!codec) {
        cerr << "Codec not find ..." << endl;
        exit(1);
    }
    codecCtx = avcodec_alloc_context3(codec);
    if (!codecCtx) {
        cerr << "Could not alloc audio codec context ..." << endl;
        exit(1);
    }
    //put sample parameters
    codecCtx->bit_rate = 64000;

    codecCtx->sample_fmt = AV_SAMPLE_FMT_S16;
    //check that the encoder supports s16 pcm input
    if (!ValidUtil::checkSampleFormat(codec, codecCtx->sample_fmt)) {
        cerr << "Encoder does not support sample format " << av_get_sample_fmt_name(codecCtx->sample_fmt) << endl;
        exit(1);
    }

    /* select other audio parameters supported by the encoder */
    codecCtx->sample_rate = /*ValidUtil::selectSampleRate(codec);*/44100;
    codecCtx->channel_layout = /*ValidUtil::selectChannelLayout(codec);*/AV_CH_LAYOUT_STEREO;
    codecCtx->channels = av_get_channel_layout_nb_channels(codecCtx->channel_layout);

    /*open codec*/
    if (avcodec_open2(codecCtx, codec, nullptr) < 0) {
        cerr << "Could not open codec ..." << endl;
        exit(1);
    }

    /* packet for holding encoded output */
    packet = av_packet_alloc();
    if (!packet) {
        cerr << "could not alloc the packet ..." << endl;
        exit(1);
    }
    /* frame containing input raw audio */
    frame = av_frame_alloc();
    if (!frame) {
        cerr << "could not alloc the audio frame" << endl;
        exit(1);
    }
    frame->nb_samples = codecCtx->frame_size;
    frame->format = codecCtx->sample_fmt;
    frame->channel_layout = codecCtx->channel_layout;
    /* allocate the data buffers */
    int ret = av_frame_get_buffer(frame, 1);
    if (ret < 0) {
        cerr << "could not alloc audio data buffer ... error : " << av_err2str(ret) << endl;
        exit(1);
    }
}

void EncodeAudio::release() {
    av_frame_free(&frame);
    av_packet_free(&packet);
    avcodec_free_context(&codecCtx);
}