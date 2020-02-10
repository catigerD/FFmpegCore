//
// Created by dengchong on 2020-02-10.
//

#include "ValidUtil.h"

static const int BEST_SAMPLE_RATE = 44100;

bool ValidUtil::checkSampleFormat(const AVCodec *codec, AVSampleFormat sampleFormat) {
    const AVSampleFormat *fmt = codec->sample_fmts;
    while (*fmt != AV_SAMPLE_FMT_NONE) {
        if (*fmt == sampleFormat) {
            return true;
        }
        fmt++;
    }
    return false;
}

int ValidUtil::selectSampleRate(const AVCodec *codec) {
    int bestSampleRate = 0;
    const int *supportedSampleRate = codec->supported_samplerates;
    while (supportedSampleRate) {
        if (!bestSampleRate || abs(BEST_SAMPLE_RATE - *supportedSampleRate) < abs(BEST_SAMPLE_RATE - bestSampleRate)) {
            bestSampleRate = *supportedSampleRate;
        }
        supportedSampleRate++;
    }
    return bestSampleRate;
}

int ValidUtil::selectChannelLayout(const AVCodec *codec) {
    int bestNbChannel{};
    uint64_t bestChannelLayout{};
    const uint64_t *channelLayout = codec->channel_layouts;
    while (channelLayout) {
        int nbChannel = av_get_channel_layout_nb_channels(*channelLayout);
        if (nbChannel > bestNbChannel) {
            bestNbChannel = nbChannel;
            bestChannelLayout = *channelLayout;
        }
        channelLayout++;
    }
    return bestChannelLayout;
}