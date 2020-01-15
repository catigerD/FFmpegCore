//
// Created by dengchong on 2020-01-15.
//

#ifndef FFMPEGCORE_DECODEAUDIO_H
#define FFMPEGCORE_DECODEAUDIO_H

#include <string>
#include <fstream>

extern "C" {
#include <libavutil/frame.h>
#include <libavcodec/avcodec.h>
};

class DecodeAudio {
public:
    ~DecodeAudio();

    void operator()(const std::string &inUrl, const std::string &outUrl);

private:
    AVCodec *codec{};
    AVCodecContext *codecCtx{};
    AVCodecParserContext *parserCtx{};
    AVPacket *packet{};
    AVFrame *frame{};

    static void decode(AVCodecContext *codecCtx, AVPacket *packet, AVFrame *frame, std::ofstream &outputStream);
};


#endif //FFMPEGCORE_DECODEAUDIO_H
