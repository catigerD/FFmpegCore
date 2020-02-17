//
// Created by dengchong on 2020-02-14.
//

#ifndef FFMPEGCORE_METADATA_H
#define FFMPEGCORE_METADATA_H

#include <string>

extern "C" {
#include "libavformat/avformat.h"
};

class MetaData {
public:
    explicit MetaData(std::string filePath)
            : fileName(std::move(filePath)) {
        init();
    }

    ~MetaData() {
        release();
    }

    void operator()();

private:
    const std::string fileName;
    AVFormatContext *formatContext{};
    AVDictionaryEntry *entry{};

    void init();

    void release();
};


#endif //FFMPEGCORE_METADATA_H
