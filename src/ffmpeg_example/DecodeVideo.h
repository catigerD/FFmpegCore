//
// Created by dengchong on 2020-01-15.
//

#ifndef FFMPEGCORE_DECODEVIDEO_H
#define FFMPEGCORE_DECODEVIDEO_H

#include <cstdint>
#include <fstream>
#include <string>

extern "C" {
#include <libavcodec/avcodec.h>
};

class DecodeVideo {
public:
    ~DecodeVideo();

    void operator()(const std::string &inUrl, const std::string &outUrl);

private:
    AVCodecContext *codecCtx{};
    AVCodecParserContext *parserCtx{};
    AVPacket *packet{};
    AVFrame *frame{};

    static void pgmSave(uint8_t **buf, int *wrap, int xSize, int ySize, std::ofstream &outputStream);

    static void decode(AVCodecContext *codecCtx, AVPacket *packet, AVFrame *frame, const std::string &outFileName);
};


#endif //FFMPEGCORE_DECODEVIDEO_H
