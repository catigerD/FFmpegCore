//
// Created by dengchong on 2020-01-07.
//

#include "MediaDataTest.h"
#include "YUVUtil.h"

using std::string;
using std::array;

const string MediaDataTest::WORK_SPACE = "/Users/dengchong/Study/FFmpegCore/res/simplest_mediadata_test/";

const auto OUT_PUT = MediaDataTest::WORK_SPACE + "output/";

void MediaDataTest::testSplitYUV420p() {
    std::string inUrl(WORK_SPACE + "simplest_mediadata_test/lena_256x256_yuv420p.yuv");
    array<string, 3> outUrls{OUT_PUT + "split420pY.yuv", OUT_PUT + "split420pU.yuv", OUT_PUT + "split420pV.yuv"};
    YUVUtil::splitYUV420p(inUrl, 256, 256, outUrls);
}

void MediaDataTest::testGrayYUV420p() {
    std::string inUrl(WORK_SPACE + "simplest_mediadata_test/lena_256x256_yuv420p.yuv");
    std::string outUlr(OUT_PUT + "grayYuv.yuv");
    YUVUtil::grayYUV420p(inUrl, 256, 256, outUlr);
}

void MediaDataTest::testHalfyYUV420p() {
    std::string inUrl(WORK_SPACE + "simplest_mediadata_test/lena_256x256_yuv420p.yuv");
    std::string outUlr(OUT_PUT + "halfYuv.yuv");
    YUVUtil::halfyYUV420p(inUrl, 256, 256, outUlr);
}

void MediaDataTest::testBorderYUV420p() {
    std::string inUrl(WORK_SPACE + "simplest_mediadata_test/lena_256x256_yuv420p.yuv");
    std::string outUlr(OUT_PUT + "borderYuv.yuv");
    YUVUtil::borderYUV420p(inUrl, 256, 256, 20, outUlr);
}

void MediaDataTest::testPsnrYUV420p() {
    std::string srcUrl(WORK_SPACE + "simplest_mediadata_test/lena_256x256_yuv420p.yuv");
    std::string dstUrl(WORK_SPACE + "simplest_mediadata_test/lena_distort_256x256_yuv420p.yuv");
    YUVUtil::psnrYUV420p(srcUrl, dstUrl, 256, 256);
}

void MediaDataTest::testSplitRGB24() {
    std::string inUrl(WORK_SPACE + "simplest_mediadata_test/cie1931_500x500.rgb");
    array<string, 3> outUrls{OUT_PUT + "splitRGB24R.yuv", OUT_PUT + "splitRGB24G.yuv", OUT_PUT + "splitRGB24B.yuv"};
    YUVUtil::splitRGB24(inUrl, 500, 500, outUrls);
}

void MediaDataTest::testRgb24ToBmp() {
    std::string inUrl(WORK_SPACE + "simplest_mediadata_test/lena_256x256_rgb24.rgb");
    std::string outUlr(OUT_PUT + "rgb2bmp.bmp");
    YUVUtil::rgb24ToBmp(inUrl, 256, 256, outUlr);
}