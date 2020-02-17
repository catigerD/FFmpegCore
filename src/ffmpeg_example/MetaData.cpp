//
// Created by dengchong on 2020-02-14.
//

#include "MetaData.h"

#include <iostream>

extern "C" {
#include "libavutil/dict.h"
}

using namespace std;

void MetaData::init() {
    int ret = avformat_open_input(&formatContext, fileName.c_str(), nullptr, nullptr);
//    ret = avformat_find_stream_info(formatContext, nullptr);
    av_dump_format(formatContext, 0, fileName.c_str(), 0);
    if (ret) {
        cerr << "could not open input url : " << fileName << endl;
        exit(1);
    }
}

void MetaData::operator()() {
    while ((entry = av_dict_get(formatContext->metadata, "", entry, AV_DICT_IGNORE_SUFFIX))) {
        cout << entry->key << " : " << entry->value << endl;
    }
}

void MetaData::release() {
    avformat_close_input(&formatContext);
}


