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
    array<string, 3> outUrls{OUT_PUT + "split420pY.yuv", OUT_PUT + "split420pU.yuv", OUT_PUT + "split420pV.yuv"};
    std::string inUrl(WORK_SPACE + "simplest_mediadata_test/lena_256x256_yuv420p.yuv");
    YUVUtil::splitYUV420p(inUrl, 256, 256, outUrls);
}

void MediaDataTest::testGrayYUV420p() {
    std::string inUrl(WORK_SPACE + "simplest_mediadata_test/lena_256x256_yuv420p.yuv");
    std::string outUlr(OUT_PUT + "grayYuv.yuv");
    YUVUtil::grayYUV420p(inUrl, 256, 256, outUlr);
}
