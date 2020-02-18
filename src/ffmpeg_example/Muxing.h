//
// Created by dengchong on 2020-02-17.
//

#ifndef FFMPEGCORE_MUXING_H
#define FFMPEGCORE_MUXING_H

#include <string>
#include <fstream>

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h"
#include "libswscale/swscale.h"
};

class Muxing {
public:

    Muxing(std::string videoPath, std::string audioPath, std::string outPath)
            : inputVideoPath(std::move(videoPath)),
              videoInputStream(inputVideoPath),
              inputAudioPath(std::move(audioPath)),
              audioInputStream(inputAudioPath),
              outputPath(std::move(outPath)) {
        init();
    }

    ~Muxing() {
        release();
    }

    void operator()();

private:

    const std::string inputVideoPath;
    std::ifstream videoInputStream;
    const std::string inputAudioPath;
    std::ifstream audioInputStream;
    const std::string outputPath;

    AVFormatContext *fmtCxt{};
    AVStream *videoStream{};
    /* pts of the next frame that will be generated */
    int videoNextPts{};
    AVStream *audioStream{};
    int audioNextPts{};
    AVCodec *videoCodec{};
    AVCodec *audioCodec{};
    AVCodecContext *videoCodecCtx{};
    AVCodecContext *audioCodecCtx{};
    bool hasVideo{};
    bool hasAudio{};
    bool encodeVideo{};
    bool encodeAudio{};

    AVFrame *videoFrame{};
    AVFrame *tempVideoFrame{};
    AVFrame *audioFrame{};
    AVFrame *tempAudioFrame{};

    SwrContext *swrCxt{};
    SwsContext *swsCxt{};

    void init();

    void release();

    void addStream(AVStream *&avStream, AVCodecContext *&codecCtx, AVCodec *&codec, AVCodecID codecId);

    void openVideo();

    void openAudio();

    /*
    * encode one video frame and send it to the muxer
    * return true when encoding is finished, false otherwise
    */
    bool writeVideoFrame();

    bool writeAudioFrame();
};


#endif //FFMPEGCORE_MUXING_H
