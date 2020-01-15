//
// Created by dengchong on 2020-01-15.
//

#ifndef FFMPEGCORE_AVIOREADING_H
#define FFMPEGCORE_AVIOREADING_H

#include <cstdint>
#include <string>

extern "C" {
#include <libavformat/avformat.h>
};

class AVIOReading {
public:
    ~AVIOReading();

    int operator()(const std::string &url);

private:
    AVFormatContext *fmtCtx{};
    AVIOContext *ioCtx{};
    //文件 mmap 到buffer
    uint8_t *buffer{};
    size_t bufferSize{};
    int ret{};

    static int readPacket(void *opaque, uint8_t *buf, int bufSize);
};

#endif //FFMPEGCORE_AVIOREADING_H
