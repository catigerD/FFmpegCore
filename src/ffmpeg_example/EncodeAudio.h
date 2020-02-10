//
// Created by dengchong on 2020-02-10.
//

#ifndef FFMPEGCORE_ENCODEAUDIO_H
#define FFMPEGCORE_ENCODEAUDIO_H

#include <string>
#include <fstream>

extern "C" {
#include "libavcodec/avcodec.h"
};

class EncodeAudio {
public:
    EncodeAudio(std::string inputPath, std::string outPath)
            : mInputPath(std::move(inputPath)),
              inputStream(mInputPath),
              mOutPath(std::move(outPath)),
              outputStream(mOutPath) {
        init();
    }

    ~EncodeAudio() {
        release();
    }

    void operator()();

private:
    const std::string mInputPath;
    std::ifstream inputStream;
    const std::string mOutPath;
    std::ofstream outputStream;

    AVCodecContext *codecCtx{};
    AVPacket *packet{};
    AVFrame *frame{};

    void init();

    void release();
};


#endif //FFMPEGCORE_ENCODEAUDIO_H
