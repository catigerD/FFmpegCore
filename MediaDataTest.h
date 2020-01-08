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
};

#endif //FFMPEGCORE_MEDIADATATEST_H
