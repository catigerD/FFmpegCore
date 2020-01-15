//
// Created by dengchong on 2020-01-15.
//

#include "DecodeVideo.h"

#include <iostream>
#include <sstream>

using namespace std;

static const int INBUF_SIZE = 4096;

DecodeVideo::~DecodeVideo() {
    av_frame_free(&frame);
    av_packet_free(&packet);
    av_parser_close(parserCtx);
    avcodec_free_context(&codecCtx);
}

void DecodeVideo::operator()(const std::string &inUrl, const std::string &outUrl) {
    AVCodec *codec{};
    codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec) {
        cerr << "Codec not found ..." << endl;
        exit(1);
    }
    parserCtx = av_parser_init(codec->id);
    if (!parserCtx) {
        cerr << "parser not found ..." << endl;
        exit(1);
    }
    codecCtx = avcodec_alloc_context3(codec);
    if (!codecCtx) {
        cerr << "Could not allocate video codec context" << endl;
        exit(1);
    }

    /* For some codecs ,such as msmpeg4 and mpeg4, width and height must be initialized there,
     * beacause this information is not available in the bitstream*/

    /*open it*/
    if (avcodec_open2(codecCtx, codec, nullptr) < 0) {
        cerr << "Could not open codec" << endl;
        exit(1);
    }

    ifstream inputStream(inUrl);
    uint8_t inBuf[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    /*set end of buffer to 0 (this ensures that no overreading happens for damaged MPEG streams)*/
    memset(inBuf + INBUF_SIZE, 0, AV_INPUT_BUFFER_PADDING_SIZE);
    uint8_t *data{};
    size_t dataSize{};

    packet = av_packet_alloc();
    if (!packet) {
        cerr << "Could not allocate video packet..." << endl;
        exit(1);
    }
    frame = av_frame_alloc();
    if (!frame) {
        cerr << "Could not allocate video frame..." << endl;
        exit(1);
    }
    int ret{};
    while (!inputStream.eof()) {
        /* read raw data from the input file */
        inputStream.read(reinterpret_cast<char *>(inBuf), INBUF_SIZE);
        dataSize = inputStream.gcount();
        if (!dataSize) {
            break;
        }
        data = inBuf;
        while (dataSize > 0) {
            /* use the parser to split the data into frames */
            ret = av_parser_parse2(parserCtx, codecCtx, &packet->data, &packet->size, data, dataSize, AV_NOPTS_VALUE,
                                   AV_NOPTS_VALUE, 0);
            if (ret < 0) {
                cerr << "Error while parsing" << endl;
                exit(1);
            }
            data += ret;
            dataSize -= ret;
            if (packet->size) {
                decode(codecCtx, packet, frame, outUrl);
            }
        }
    }
    /* flush the decoder */
    decode(codecCtx, nullptr, frame, outUrl);
}

void DecodeVideo::pgmSave(uint8_t **buf, int *wrap, int xSize, int ySize, std::ofstream &outputStream) {
    //fprintf是C/C++中的一个格式化库函数，位于头文件<cstdio>中，其作用是格式化输出到一个流文件中；
    // 函数原型为int fprintf( FILE *stream, const char *format, [ argument ]...)，
    // fprintf()函数根据指定的格式(format)，向输出流(stream)写入数据(argument)。
    //PBM 格式: https://zh.wikipedia.org/wiki/PBM%E6%A0%BC%E5%BC%8F
    /* 这句代码可使得图片可直接被预览 */
//    outputStream << "P5" << endl << xSize << " " << ySize << endl << 255;
    //YUV422p
    /* save Y data */
    for (int i = 0; i < ySize; ++i) {
        outputStream.write(reinterpret_cast<const char *>(buf[0] + i * wrap[0]), xSize);
    }
    /* save U(Cb) data */
    for (int i = 0; i < ySize / 2; ++i) {
        outputStream.write(reinterpret_cast<const char *>(buf[1] + i * wrap[1]), xSize);
    }
    /* save V(Cr) data */
    for (int i = 0; i < ySize / 2; ++i) {
        outputStream.write(reinterpret_cast<const char *>(buf[2] + i * wrap[2]), xSize);
    }
}

void DecodeVideo::decode(AVCodecContext *codecCtx, AVPacket *packet, AVFrame *frame, const std::string &outFileName) {
    int ret{};
    ret = avcodec_send_packet(codecCtx, packet);
    if (ret < 0) {
        cerr << "Error sending a packet for decoding" << endl;
        exit(1);
    }
    while (ret >= 0) {
        ret = avcodec_receive_frame(codecCtx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return;
        } else if (ret < 0) {
            cerr << "Error during decoding" << endl;
            exit(1);
        }
        cout << "cur decode frame " << codecCtx->frame_number << endl;
        /*the picture is allocated by the decoder. no need to free it*/
//            snprintf()，函数原型为int snprintf(char *str, size_t size, const char *format, ...)。
//            将可变参数 “…” 按照format的格式格式化为字符串，然后再将其拷贝至str中。
        //RGB格式？
        if (codecCtx->frame_number >= 100 && codecCtx->frame_number <= 110) {
            stringstream fileNameStream;
            fileNameStream << outFileName << "_" << codecCtx->frame_number << ".yuv";
            ofstream outputStream(fileNameStream.str());
            if (!outputStream) {
                cerr << fileNameStream.str() << " open failed ..." << endl;
                exit(1);
            }
            cout << "saving frame " << codecCtx->frame_number << endl;
            pgmSave(frame->data, frame->linesize, frame->width, frame->height, outputStream);
        }
    }
}