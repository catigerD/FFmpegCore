//
// Created by dengchong on 2020-01-09.
//

#ifndef FFMPEGCORE_PCMUTIL_H
#define FFMPEGCORE_PCMUTIL_H

#include <string>
#include <array>

class PcmUtil {
public:

    //分离 PCM16LE 双声道音频采样数据的左右声道
    static void splitPcm16LE(const std::string &inUrl, std::array<std::string, 2> &outUrls);

    //将 PCM16LE 双声道音频采样数据中左声道的音量降低一半
    static void halfVolumeLToPcm16LE(const std::string &inUrl, const std::string &outUrl);

    //将 PCM16LE 双声道音频采样数据的声音速度提高一倍
    static void doubleSpeedPcm16LE(const std::string &inUrl, const std::string &outUrl);

    //将 PCM16LE 双声道音频采样数据转换为 PCM8 音频采样数据
    static void pcm16LE2Pcm8(const std::string &inUrl, const std::string &outUrl);

    //从 PCM16LE 单声道音频采样数据中截取一部分数据
    static void
    cutSingleChannelPcm16LE(const std::string &inUrl, unsigned int start, unsigned int dur, const std::string &outUrl);

    //将 PCM16LE 双声道音频采样数据转换为 WAVE格式音频数据
    static void pcm16LE2Wave(const std::string &inUrl, int channels, int sampleRate, const std::string &outUrl);

};

struct WAVE_HEADER {
    char fccID[4]{'R', 'I', 'F', 'F'};
    unsigned int dwSize;
    char fccType[4]{'W', 'A', 'V', 'E'};
};

struct WAVE_FMT {
    char fccID[4]{'f', 'm', 't', ' '};
    unsigned int dwSize{16};
    unsigned short wFormatTag{1};
    unsigned short wChannels;
    unsigned int dwSamplesPerSec;
    unsigned int dwAvgBytesPerSec;
    unsigned short wBlockAlign{2};
    unsigned short uiBitsPerSample{16};
};

struct WAVE_DATA {
    char fccID[4]{'d', 'a', 't', 'a'};
    unsigned int dwSize;
};


#endif //FFMPEGCORE_PCMUTIL_H
