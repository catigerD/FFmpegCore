#include <iostream>

#include "MediaDataTest.h"
#include "FFmpegTest.h"
#include "FFmpegExerciseTest.h"

int main() {
    std::cout << "Hello, World!" << std::endl;
//    MediaDataTest::testSplitYUV420p();
//    MediaDataTest::testGrayYUV420p();
//    MediaDataTest::testHalfyYUV420p();
//    MediaDataTest::testBorderYUV420p();
//    MediaDataTest::testPsnrYUV420p();
//    MediaDataTest::testSplitRGB24();
//    MediaDataTest::testRgb24ToBmp();
//    MediaDataTest::testRgb24ToYuv420p();

//    MediaDataTest::testSplitPcm16LE();
//    MediaDataTest::testHalfVolumeLToPcm16LE();
//    MediaDataTest::testDoubleSpeedPcm16LE();
//    MediaDataTest::testPcm16LE2Pcm8();
//    MediaDataTest::testCutSingleChannelPcm16LE();
//    MediaDataTest::testPcm16LE2Wave();

//    MediaDataTest::testParseH264();

//    MediaDataTest::testParserAAC();

//    MediaDataTest::testParserFLV();

//    FFmpegTest::testAVIOReading();

//    FFmpegTest::testDecodeAudio();

//    FFmpegTest::testDecodeVideo();

//    FFmpegTest::testDemuxingDecoding();

//    FFmpegExerciseTest::testDemuxingDecode();

//    FFmpegTest::testEncodeAudio();

//    FFmpegTest::testEncodeVideo();

    FFmpegTest::testMetaData();
    return 0;
}