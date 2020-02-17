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

    static void testDecodeVideo();

    static void testDemuxingDecoding();

    static void testEncodeAudio();

    static void testEncodeVideo();

    static void testMetaData();
};


#endif //FFMPEGCORE_FFMPEGTEST_H
