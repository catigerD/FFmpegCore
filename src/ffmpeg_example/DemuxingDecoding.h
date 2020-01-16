//
// Created by dengchong on 2020-01-16.
//

#ifndef FFMPEGCORE_DEMUXINGDECODING_H
#define FFMPEGCORE_DEMUXINGDECODING_H

extern "C" {
#include <libavformat/avformat.h>
};

#include <string>
#include <fstream>

class DemuxingDecoding {
public:
    DemuxingDecoding(std::string inUrl, std::string rawVideoUrl, std::string rawAudioUrl)
            : inputUrl(std::move(inUrl)), outVideoUrl(std::move(rawVideoUrl)), outAudioUrl(std::move(rawAudioUrl)),
              videoOutputStream(outVideoUrl), audioOutputStream(outAudioUrl) {
    };

    ~DemuxingDecoding();

    int operator()();

private:
    std::string inputUrl;
    std::string outVideoUrl;
    std::string outAudioUrl;

    AVFormatContext *fmtCtx{};
    AVCodecContext *videoCodecCtx{};
    AVCodecContext *audioCodecCtx{};
    AVPacket packet;
    AVFrame *frame{};
    AVStream *videoStream{};
    AVStream *audioStream{};
    int videoStreamIdx{};
    int audioStreamIdx{};
    int width{};
    int height{};
    AVPixelFormat pixFmt{AV_PIX_FMT_NONE};

    int videoFrameCount{};
    int audioFrameCount{};

    uint8_t *videoDstData[4]{};
    int videoDstLinesize[4];
    int videoDstBufsize{};

    std::ofstream videoOutputStream;
    std::ofstream audioOutputStream;

    int decodePacket(int &gotFrame, int cached);
};


#endif //FFMPEGCORE_DEMUXINGDECODING_H
