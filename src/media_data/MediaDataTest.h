//
// Created by dengchong on 2020-01-07.
//

#ifndef FFMPEGCORE_MEDIADATATEST_H
#define FFMPEGCORE_MEDIADATATEST_H

#include <string>

class MediaDataTest {
public:
    const static std::string WORK_SPACE;

    static void testSplitYUV420p();

    static void testGrayYUV420p();

    static void testHalfyYUV420p();

    static void testBorderYUV420p();

    static void testPsnrYUV420p();

    static void testSplitRGB24();

    static void testRgb24ToBmp();

    static void testRgb24ToYuv420p();

    static void testSplitPcm16LE();

    static void testHalfVolumeLToPcm16LE();

    static void testDoubleSpeedPcm16LE();

    static void testPcm16LE2Pcm8();

    static void testCutSingleChannelPcm16LE();

    static void testPcm16LE2Wave();

    static void testParseH264();
};

#endif //FFMPEGCORE_MEDIADATATEST_H
