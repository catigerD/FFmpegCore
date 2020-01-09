//
// Created by dengchong on 2020-01-09.
//

#ifndef FFMPEGCORE_FILEUTIL_H
#define FFMPEGCORE_FILEUTIL_H

#include <fstream>
#include <vector>
#include <memory>

class FileUtil {
public:
    static std::shared_ptr<std::vector<char >> getByteVector(const std::string &url);

    static void writeByteVector(const std::string &url, const std::shared_ptr<std::vector<char >> &data);
};


#endif //FFMPEGCORE_FILEUTIL_H
