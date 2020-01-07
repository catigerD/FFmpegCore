//
// Created by dengchong on 2020-01-07.
//

#ifndef FFMPEGCORE_YUVUTIL_H
#define FFMPEGCORE_YUVUTIL_H

#include <string>
#include <fstream>
#include <iterator>
#include <vector>

class YUVUtil {
public:
    static void splitYUV420p(const std::string &inUrl, unsigned int width, unsigned int height,
                             const std::string &outUrl) {
        std::vector<unsigned char> yuvData(width * height * 2);
        std::ifstream inputStream(inUrl);
        std::istream_iterator<unsigned char> inputBegin(inputStream);
        std::istream_iterator<unsigned char> inputEnd;
        std::copy(inputBegin, inputEnd, yuvData.begin());
        std::ofstream outputStream(outUrl);
        std::ostream_iterator<unsigned char> outBegin(outputStream);
        std::copy(yuvData.cbegin(), yuvData.cbegin() + width * height, outBegin);
    }

    static void grayYUV420p(const std::string &inUrl, unsigned int width, unsigned int height,
                            const std::string &outUrl) {
        std::vector<unsigned char> yuvData(width * height * 3);
        std::ifstream inputStream(inUrl);
        std::istream_iterator<unsigned char> inputBegin(inputStream);
        std::istream_iterator<unsigned char> inputEnd;
        std::copy(inputBegin, inputEnd, yuvData.begin());
        for (int i = 0; i < yuvData.size(); ++i) {
            if (i > width * height) {
                yuvData[i] = 128;
            }
        }
        std::ofstream outputStream(outUrl);
        std::ostream_iterator<unsigned char> outBegin(outputStream);
        std::copy(yuvData.cbegin(), yuvData.cbegin() + yuvData.size(), outBegin);

    };
};


#endif //FFMPEGCORE_YUVUTIL_H
