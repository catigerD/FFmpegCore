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
};

#endif //FFMPEGCORE_MEDIADATATEST_H
