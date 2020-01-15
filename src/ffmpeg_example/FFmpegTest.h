//
// Created by dengchong on 2020-01-14.
//

#ifndef FFMPEGCORE_FFMPEGTEST_H
#define FFMPEGCORE_FFMPEGTEST_H

extern "C" {
#include "libavformat/avformat.h"
};

class FFmpegTest {
public:
    static void testAVIOReading();

    static void testDecodeAudio();
};


#endif //FFMPEGCORE_FFMPEGTEST_H
