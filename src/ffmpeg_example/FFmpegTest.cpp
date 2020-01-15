//
// Created by dengchong on 2020-01-14.
//

#include "FFmpegTest.h"

#include <string>

#include "AVIOReading.h"

using namespace std;

static const string WORK_SPACE = "/Users/dengchong/Study/FFmpegCore/res/simplest_mediadata_test/";

void FFmpegTest::testAVIOReading() {
    string url(WORK_SPACE + "simplest_mediadata_test/cuc_ieschool.flv");
    AVIOReading avioReading;
    avioReading(url);
}