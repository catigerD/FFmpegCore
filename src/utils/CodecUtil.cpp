//
// Created by dengchong on 2020-02-10.
//

#include "CodecUtil.h"

#include <iostream>

using namespace std;

void CodecUtil::encode(AVCodecContext *codecCtx, AVFrame *frame, AVPacket *packet, std::ofstream &outputStream) {
    int ret{};
    /* send the frame for encoding */
    ret = avcodec_send_frame(codecCtx, frame);
    if (ret < 0) {
        cerr << "Error sending the frame to the encoder" << endl;
        exit(1);
    }
    /* read all the available output packets (in general there may be any
     * number of them */
    while (ret >= 0) {
        ret = avcodec_receive_packet(codecCtx, packet);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return;
        } else if (ret < 0) {
            cerr << "Error encoding audio frame" << endl;
            exit(1);
        }
        outputStream.write(reinterpret_cast<const char *>(packet->data), packet->size);
        av_packet_unref(packet);
    }
}