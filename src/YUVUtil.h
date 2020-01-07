//
// Created by dengchong on 2020-01-07.
//

#ifndef FFMPEGCORE_YUVUTIL_H
#define FFMPEGCORE_YUVUTIL_H

#include <string>
#include <fstream>
#include <iterator>
#include <vector>
#include <array>

class YUVUtil {
public:
    static void splitYUV420p(const std::string &inUrl, unsigned int width, unsigned int height,
                             const std::array<std::string, 3> &outUrls) {
        std::vector<unsigned char> yuvData(width * height * 2);
        std::ifstream inputStream(inUrl);
        std::istream_iterator<unsigned char> inputBegin(inputStream);
        std::istream_iterator<unsigned char> inputEnd;
        std::copy(inputBegin, inputEnd, yuvData.begin());
        std::ofstream outputYStream(outUrls[0]);
        auto yBegin = yuvData.cbegin();
        std::ostream_iterator<unsigned char> outYBegin(outputYStream);
        std::copy(yuvData.cbegin(), yuvData.cbegin() + width * height, outYBegin);
        std::ofstream outputUStream(outUrls[1]);
        std::ostream_iterator<unsigned char> outUBegin(outputUStream);
        auto uBegin = yuvData.cbegin() + width * height;
        std::copy(uBegin, uBegin + width * height / 4, outUBegin);
        std::ofstream outputVStream(outUrls[2]);
        std::ostream_iterator<unsigned char> outVBegin(outputVStream);
        auto vBegin = yuvData.cbegin() + width * height * 5 / 4;
        std::copy(vBegin, vBegin + width * height / 4, outVBegin);
    }

    static void grayYUV420p(const std::string &inUrl, unsigned int width, unsigned int height,
                            const std::string &outUrl) {
        std::vector<unsigned char> yuvData(width * height * 2);
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
