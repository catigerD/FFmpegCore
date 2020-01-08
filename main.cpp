#include <iostream>

#include "MediaDataTest.h"

int main() {
    std::cout << "Hello, World!" << std::endl;
    MediaDataTest::testSplitYUV420p();
    MediaDataTest::testGrayYUV420p();
    MediaDataTest::testHalfyYUV420p();
    MediaDataTest::testBorderYUV420p();
    MediaDataTest::testPsnrYUV420p();
    MediaDataTest::testSplitRGB24();
    MediaDataTest::testRgb24ToBmp();
    return 0;
}