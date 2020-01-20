//
// Created by dengchong on 2020-01-19.
//

#include "FFmpegExerciseTest.h"

#include <string>

#include "FFmpegDexmuxDecodeExercise.h"

using namespace std;

static const string WORK_SPACE = "/Users/dengchong/Study/FFmpegCore/res/simplest_mediadata_test/";
static const auto OUT_PUT = WORK_SPACE + "output/";

void FFmpegExerciseTest::testDemuxingDecode() {
    string inUrl(WORK_SPACE + "simplest_mediadata_test/cuc_ieschool.flv");
    string outVideoUrl(OUT_PUT + "exercise-demuxingDecodingVideo.yuv");
    string outAudioUrl(OUT_PUT + "exercise-demuxingDecodingAudio.pcm");
    FFmpegDexmuxDecodeExercise exercise(inUrl, outVideoUrl, outAudioUrl);
    exercise.demuxingDecode();
}