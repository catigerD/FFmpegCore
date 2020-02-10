//
// Created by dengchong on 2020-01-14.
//

#include "FFmpegTest.h"

#include <string>

#include "AVIOReading.h"
#include "DecodeAudio.h"
#include "DecodeVideo.h"
#include "DemuxingDecoding.h"
#include "EncodeAudio.h"

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

void FFmpegTest::testDemuxingDecoding() {
    string inUrl(WORK_SPACE + "simplest_mediadata_test/cuc_ieschool.flv");
    string outVideoUrl(OUT_PUT + "demuxingDecodingVideo.yuv");
    string outAudioUrl(OUT_PUT + "demuxingDecodingAudio.pcm");
    DemuxingDecoding demuxingDecoding(inUrl, outVideoUrl, outAudioUrl);
    demuxingDecoding();
}

void FFmpegTest::testEncodeAudio() {
    string inUrl(WORK_SPACE + "simplest_mediadata_test/NocturneNo2inEflat_44.1k_s16le.pcm");
    string outUrl(OUT_PUT + "encodeAudio.mp2");
    EncodeAudio encodeAudio(inUrl, outUrl);
    encodeAudio();
}