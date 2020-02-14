//
// Created by dengchong on 2020-02-14.
//

#ifndef FFMPEGCORE_ENCODEVIDEO_H
#define FFMPEGCORE_ENCODEVIDEO_H

#include <string>
#include <fstream>

extern "C" {
#include "libavcodec/avcodec.h"
};

class EncodeVideo {
public:
    EncodeVideo(std::string inPath, std::string outPath)
            : inputPath(std::move(inPath)),
              inputStream(inputPath),
              outputPath(std::move(outPath)),
              outputStream(outputPath) {
        init();
    }

    ~EncodeVideo() {
        release();
    }

    void operator()();

private:
    const std::string inputPath;
    std::ifstream inputStream;
    const std::string outputPath;
    std::ofstream outputStream;

    AVCodecContext *codecCtx{};
    AVFrame *frame{};
    AVPacket *packet{};

    void init();

    void release();
};


#endif //FFMPEGCORE_ENCODEVIDEO_H
