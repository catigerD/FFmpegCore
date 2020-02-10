//
// Created by dengchong on 2020-02-10.
//

#ifndef FFMPEGCORE_CODECUTIL_H
#define FFMPEGCORE_CODECUTIL_H

#include <fstream>

extern "C" {
#include "libavcodec/avcodec.h"
};

struct CodecUtil {
    static void encode(AVCodecContext *codecCtx, AVFrame *frame, AVPacket *packet, std::ofstream &outputStream);
};


#endif //FFMPEGCORE_CODECUTIL_H
