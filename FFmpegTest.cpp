//
// Created by dengchong on 2020-01-07.
//

#include "FFmpegTest.h"
#include "YUVUtil.h"

void testSplitYUV420p(){
    std::string inUrl("/Users/dengchong/Downloads/ffmpeg/test/output.yuv");
    std::string outUlr("/Users/dengchong/Downloads/ffmpeg/test/splitYUV.yuv");
    YUVUtil::splitYUV420p(inUrl, 499, 416, outUlr);
}

void testGrayYUV420p(){
    std::string inUrl("/Users/dengchong/Downloads/ffmpeg/test/yuv444p.yuv");
    std::string outUlr("/Users/dengchong/Downloads/ffmpeg/test/grayYuv420p.yuv");
    YUVUtil::grayYUV420p(inUrl, 499, 416, outUlr);
}
