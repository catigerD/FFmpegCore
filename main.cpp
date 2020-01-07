#include <iostream>

#include "MediaDataTest.h"

int main() {
    std::cout << "Hello, World!" << std::endl;
    MediaDataTest::testSplitYUV420p();
    MediaDataTest::testGrayYUV420p();
    return 0;
}