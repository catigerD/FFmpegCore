//
// Created by dengchong on 2020-01-14.
//

#include "FFmpegTest.h"

#include <string>

#include "AVIOReading.h"
#include "DecodeAudio.h"
#include "DecodeVideo.h"

using namespace std;

static const string WORK_SPACE = "/Users/dengchong/Study/FFmpegCore/res/simplest_mediadata_test/";
static const auto OUT_PUT = WORK_SPACE + "output/";

void FFmpegTest::testAVIOReading() {
    string url(WORK_SPACE + "simplest_mediadata_test/cuc_ieschool.flv");
    AVIOReading avioReading;
    avioReading(url);
}

void FFmpegTest::testDecodeAudio() {
    string inUrl(WORK_SPACE + "simplest_mediadata_test/nocturne.aac");
    string outUrl(OUT_PUT + "decodeAudio.pcm");
    DecodeAudio decodeAudio;
    decodeAudio(inUrl, outUrl);
}

void FFmpegTest::testDecodeVideo() {
    string inUrl(WORK_SPACE + "simplest_mediadata_test/sintel.h264");
    string outUrl(OUT_PUT + "decodeVideo/picture");
    DecodeVideo decodeVideo;
    decodeVideo(inUrl, outUrl);
}