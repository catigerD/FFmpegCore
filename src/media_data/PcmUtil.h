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
    static void cutSingleChannelPcm16LE(const std::string &inUrl, unsigned int start, unsigned int dur,const std::string &outUrl);

};


#endif //FFMPEGCORE_PCMUTIL_H
