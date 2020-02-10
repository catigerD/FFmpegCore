//
// Created by dengchong on 2020-02-10.
//

#ifndef FFMPEGCORE_VALIDUTIL_H
#define FFMPEGCORE_VALIDUTIL_H

extern "C" {
#include <libavcodec/avcodec.h>
};

struct ValidUtil {
    /*check that a given sample format is supported by the encoder*/
    static bool checkSampleFormat(const AVCodec *codec, AVSampleFormat sampleFormat);

    /* just pick the highest supported sample rate */
    static int selectSampleRate(const AVCodec *codec);

    /* select layout with the highest channel count */
    static int selectChannelLayout(const AVCodec *codec);
};


#endif //FFMPEGCORE_VALIDUTIL_H
