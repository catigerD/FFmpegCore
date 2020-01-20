//
// Created by dengchong on 2020-01-20.
//

#ifndef FFMPEGCORE_FFMPEGDEXMUXDECODEEXERCISE_H
#define FFMPEGCORE_FFMPEGDEXMUXDECODEEXERCISE_H

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
};

#include <string>
#include <fstream>
#include <functional>

class FFmpegDexmuxDecodeExercise {
public:
    FFmpegDexmuxDecodeExercise(const std::string &url, const std::string &videoUrl, const std::string &audioUrl)
            : inputUrl(url),
              outVideoUrl(videoUrl),
              outAudioUrl(audioUrl),
              videoOutputStream(outVideoUrl),
              audioOutputStream(outAudioUrl) {
        init();
    }

    ~FFmpegDexmuxDecodeExercise();

    void demuxingDecode();

private:
    std::string inputUrl;
    std::string outVideoUrl;
    std::string outAudioUrl;
    std::ofstream videoOutputStream;
    std::ofstream audioOutputStream;

    AVFormatContext *fmtCtx{};
    AVCodecContext *vCodecCtx{};
    int vStreamIdx{};
    AVCodecContext *aCodecCtx{};
    int aStreamIdx{};

    AVPacket *packet{};
    AVFrame *frame{};

    int ret{};

    void init();

    static void writeVideo(std::ofstream &outputStream, const AVCodecContext *codecContext, const AVFrame *avFrame);

    static void writeAudio(std::ofstream &outputStream, const AVCodecContext *codecContext, const AVFrame *avFrame);
};

int openCodec(AVFormatContext *fmtCtx, AVCodecContext *&codecCtx, int &index, const AVMediaType &type);

void decode(std::ofstream &outputStream, AVCodecContext *codecCtx, AVPacket *packet, AVFrame *&frame,
            const std::function<void(std::ofstream &outputStream, const AVCodecContext *, const AVFrame *)> &writeFun);


#endif //FFMPEGCORE_FFMPEGDEXMUXDECODEEXERCISE_H
