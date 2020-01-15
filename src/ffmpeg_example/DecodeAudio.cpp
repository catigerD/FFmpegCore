//
// Created by dengchong on 2020-01-15.
//

#include "DecodeAudio.h"

extern "C" {
#include <libavutil/mem.h>
}

#include <iostream>

using namespace std;

static const int AUDIO_INBUF_SIZE = 20480;
static const int AUDIO_REFILL_THRESH = 4096;

DecodeAudio::~DecodeAudio() {
    av_frame_free(&frame);
    av_packet_free(&packet);
    av_parser_close(parserCtx);
    avcodec_free_context(&codecCtx);
}

void DecodeAudio::operator()(const std::string &inUrl, const std::string &outUrl) {
    int ret{};

    codec = avcodec_find_decoder(AV_CODEC_ID_AAC);
    if (!codec) {
        cerr << "Codec not found ..." << endl;
        exit(1);
    }
    parserCtx = av_parser_init(codec->id);
    if (!parserCtx) {
        cerr << "Parser not found ..." << endl;
        exit(1);
    }
    codecCtx = avcodec_alloc_context3(codec);
    if (!codecCtx) {
        cerr << "Could not allocate audio codec context" << endl;
        exit(1);
    }

    /*open it*/
    if (avcodec_open2(codecCtx, codec, nullptr) < 0) {
        cerr << "Could not open codec" << endl;
        exit(1);
    }

    ifstream inputStream(inUrl);
    if (!inputStream) {
        cerr << "Could not open " << inUrl << endl;
    }
    ofstream outputStream(outUrl);
    if (!outputStream) {
        exit(1);
    }

    packet = av_packet_alloc();
    if (!packet) {
        cerr << "Could not allocate audio packet" << endl;
        exit(1);
    }
    /*decode until eof*/
    uint8_t inBuf[AUDIO_INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    uint8_t *data = inBuf;
    inputStream.read(reinterpret_cast<char *>(data), AUDIO_INBUF_SIZE);
    size_t dataSize = inputStream.gcount();
    while (dataSize > 0) {
        if (!frame) {
            if (!(frame = av_frame_alloc())) {
                cerr << "Could not allocate audio frame" << endl;
                exit(1);
            }
        }
        ret = av_parser_parse2(parserCtx, codecCtx, &packet->data, &packet->size, data, dataSize, AV_NOPTS_VALUE,
                               AV_NOPTS_VALUE, 0);
        if (ret < 0) {
            cerr << "Error while parsing" << endl;
            exit(1);
        }
        data += ret;
        dataSize -= ret;
        if (packet->size) {
            decode(codecCtx, packet, frame, outputStream);
        }
        if (dataSize < AUDIO_REFILL_THRESH) {
            memmove(inBuf, data, dataSize);
            data = inBuf;
            inputStream.read(reinterpret_cast<char *>(data + dataSize), AUDIO_INBUF_SIZE - dataSize);
            int len = inputStream.gcount();
            if (len > 0) {
                dataSize += len;
            }
        }
    }

    /* flush the decoder*/
    packet->data = nullptr;
    packet->size = 0;
    decode(codecCtx, packet, frame, outputStream);
}

void DecodeAudio::decode(AVCodecContext *codecCtx, AVPacket *packet, AVFrame *frame, std::ofstream &outputStream) {
    int ret{}, dataSize{};
    /* send the packet with the compressed data to the decoder*/
    ret = avcodec_send_packet(codecCtx, packet);
    if (ret < 0) {
        cerr << "Error submitting the packet to the decoder..." << endl;
        exit(1);
    }
    while (ret >= 0) {
        ret = avcodec_receive_frame(codecCtx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return;
        } else if (ret < 0) {
            cerr << "Error during decoding..." << endl;
            exit(1);
        }
        dataSize = av_get_bytes_per_sample(codecCtx->sample_fmt);
        if (dataSize < 0) {
            /* This should not occur, checking just for paranoia*/
            cerr << "Failed to calculate data size..." << endl;
            exit(1);
        }
        for (int i = 0; i < frame->nb_samples; ++i) {
            for (int ch = 0; ch < codecCtx->channels; ++ch) {
                /* frame->data为指针数组类型，数组小标表示 channel?*/
                outputStream.write(reinterpret_cast<const char *>(frame->data[ch] + dataSize * i), dataSize);
            }
        }
    }
}