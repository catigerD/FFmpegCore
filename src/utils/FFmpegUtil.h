//
// Created by dengchong on 2020-02-10.
//

#ifndef FFMPEGCORE_FFMPEGUTIL_H
#define FFMPEGCORE_FFMPEGUTIL_H

#include <fstream>

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
};

struct FFmpegUtil {


    static void encode(AVCodecContext *codecCtx, AVFrame *frame, AVPacket *packet, std::ofstream &outputStream);

    /*==================================codec context==========================================*/

    /*check that a given sample format is supported by the encoder*/
    static bool checkSampleFormat(const AVCodec *codec, AVSampleFormat sampleFormat);

    /* just pick the highest supported sample rate */
    static int selectSampleRate(const AVCodec *codec);

    /* select layout with the highest channel count */
    static int selectChannelLayout(const AVCodec *codec);

    /*==================================alloc==========================================*/

    static AVFrame *allocPicture(AVPixelFormat pixFmt, int width, int height);

    static AVFrame *allocAudioFrame(AVSampleFormat sampleFmt, uint64_t channelLayout, int sampleRate, int nbSamples);

    /*==================================read==========================================*/
    static bool readVideoFrame(AVCodecContext *codecCtx, AVFrame *&frame, std::ifstream &inputStream);

    static bool
    readVideoFrame(AVCodecContext *codecCtx, AVFrame *&frame, std::ifstream &inputStream, int &pts, SwsContext *&swsCtx,
                   AVFrame *&tempFrame);

    static bool readAudioFrame(AVFrame *frame, std::ifstream &inputStream);

    static int writeFrame(AVFormatContext *fmtCtx, const AVRational timeBase, AVStream *avStream, AVPacket *packet);

private:

    static void logPacket(const AVFormatContext *fmtCtx, const AVPacket *packet);
};


#endif //FFMPEGCORE_FFMPEGUTIL_H
