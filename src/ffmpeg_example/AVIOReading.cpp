//
// Created by dengchong on 2020-01-15.
//

#include "AVIOReading.h"

extern "C" {
#include <libavutil/common.h>
#include <libavcodec/avcodec.h>
#include <libavutil/file.h>
}

#include <iostream>

using namespace std;

struct BufferData {
    uint8_t *prt{};
    size_t size{};
};

AVIOReading::~AVIOReading() {
    avformat_close_input(&fmtCtx);
    avformat_free_context(fmtCtx);
    //note : the internal buffer could have changed, and be != ioCtxBuffer
    if (ioCtx) {
        //ioCtx->buffer == ioCtxBuffer
//    if (ioCtxBuffer) {
//        av_free(ioCtxBuffer);
//    }
        av_freep(&ioCtx->buffer);
        av_freep(&ioCtx);
    }
    avio_context_free(&ioCtx);
    av_file_unmap(buffer, bufferSize);
    if (ret < 0) {
        cerr << "Error occurred : " << av_err2str(ret) << endl;
    }
}

int AVIOReading::operator()(const std::string &url) {
    /*slurp file content into buffer*/
    ret = av_file_map(url.c_str(), &buffer, &bufferSize, 0, nullptr);
    if (ret < 0) {
        return 1;
    }
    /*fill opaque structure used by the AVIOContext read callback*/
    BufferData bd;
    bd.prt = buffer;
    bd.size = bufferSize;
    size_t ioCtxBufferSize = 4096;
    uint8_t *ioCtxBuffer = static_cast<uint8_t *>(av_malloc(ioCtxBufferSize));
    if (!ioCtxBuffer) {
        ret = AVERROR(ENOMEM);
        return 1;
    }
    ioCtx = avio_alloc_context(ioCtxBuffer, ioCtxBufferSize, 0, &bd, &readPacket, nullptr, nullptr);
    if (!ioCtx) {
        ret = AVERROR(ENOMEM);
        return 1;
    }

    if (!(fmtCtx = avformat_alloc_context())) {
        ret = AVERROR(ENOMEM);
        return 1;
    }
    fmtCtx->pb = ioCtx;
    ret = avformat_open_input(&fmtCtx, nullptr, nullptr, nullptr);
    if (ret < 0) {
        cout << "Could not open input ..." << endl;
        return 1;
    }
    ret = avformat_find_stream_info(fmtCtx, nullptr);
    if (ret < 0) {
        cout << "Could not find stream information" << endl;
        return 1;
    }
    av_dump_format(fmtCtx, 0, url.c_str(), 0);
    return 0;
}

int AVIOReading::readPacket(void *opaque, uint8_t *buf, int bufSize) {
    auto *bufferData = static_cast<BufferData *>(opaque);
    bufSize = FFMIN(bufferData->size, bufSize);
    if (!bufSize) {
        return AVERROR_EOF;
    }
    memcpy(buf, bufferData->prt, bufSize);
    bufferData->prt += bufSize;
    bufferData->size -= bufSize;
    return bufSize;
}