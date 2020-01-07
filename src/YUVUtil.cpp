//
// Created by dengchong on 2020-01-07.
//

#include "YUVUtil.h"

#include <fstream>
#include <iterator>
#include <cmath>
#include <iostream>

using std::string;
using std::ifstream;
using std::ofstream;
using std::istream_iterator;
using std::ostream_iterator;
using std::vector;

void YUVUtil::splitYUV420p(const std::string &inUrl, unsigned int width, unsigned int height,
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

void YUVUtil::grayYUV420p(const std::string &inUrl, unsigned int width, unsigned int height,
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
    std::copy(yuvData.cbegin(), yuvData.cend(), outBegin);

}

void YUVUtil::halfyYUV420p(const std::string &inUrl, unsigned int width, unsigned int height,
                           const std::string &outUrl) {
    vector<unsigned char> yuvData(width * height * 2);
    ifstream inputStream(inUrl);
    istream_iterator<unsigned char> inputBegin(inputStream);
    istream_iterator<unsigned char> inputEnd;
    std::copy(inputBegin, inputEnd, yuvData.begin());
    for (int i = 0; i < yuvData.size(); ++i) {
        if (i < width * height) {
            yuvData[i] = yuvData[i] / 2;
        }
    }
    ofstream outputStream(outUrl);
    ostream_iterator<unsigned char> outBegin(outputStream);
    std::copy(yuvData.cbegin(), yuvData.cend(), outBegin);
}

void YUVUtil::borderYUV420p(const std::string &inUrl, unsigned int width, unsigned int height, unsigned int border,
                            const std::string &outUrl) {
    vector<unsigned char> yuvData(width * height * 2);
    ifstream inputStream(inUrl);
    istream_iterator<unsigned char> inputBegin(inputStream);
    istream_iterator<unsigned char> inputEnd;
    std::copy(inputBegin, inputEnd, yuvData.begin());
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (i < border
                || height - border < i
                || j < border
                || width - border < j) {
                yuvData[i * width + j] = 255;
//                yuvData[i * width + j] = 0;
            }
        }
    }
    ofstream outputStream(outUrl);
    ostream_iterator<unsigned char> outBegin(outputStream);
    std::copy(yuvData.cbegin(), yuvData.cend(), outBegin);
}

void YUVUtil::psnrYUV420p(const std::string &srcUrl, const std::string &dstUrl, unsigned int width,
                          unsigned int height) {
    vector<unsigned char> srcYuvData(width * height * 2);
    ifstream srcInputStream(srcUrl);
    istream_iterator<unsigned char> srcInputBegin(srcInputStream);
    istream_iterator<unsigned char> srcInputEnd;
    std::copy(srcInputBegin, srcInputEnd, srcYuvData.begin());
    vector<unsigned char> dstYuvData(width * height * 2);
    ifstream destInputStream(dstUrl);
    istream_iterator<unsigned char> dstInputBegin(destInputStream);
    istream_iterator<unsigned char> dstInputEnd;
    std::copy(dstInputBegin, dstInputEnd, dstYuvData.begin());

    double mseSum = 0.0;
    for (int i = 0; i < width * height; ++i) {
        mseSum += std::pow(srcYuvData[i] - dstYuvData[i], 2);
    }
    double mse = mseSum / (width * height);
    double psnr = 10 * std::log10(std::pow(255, 2) / mse);
    std::cout << "--- psnr : " << psnr;
}

void YUVUtil::splitRGB24(const std::string &inUrl, unsigned int width, unsigned int height,
                         const std::array<std::string, 3> &outUrls) {
    vector<unsigned char> rgbData(width * height * 3);
    ifstream inputStream(inUrl);
    istream_iterator<unsigned char> inputBegin(inputStream);
    istream_iterator<unsigned char> inputEnd;
    std::copy(inputBegin, inputEnd, rgbData.begin());

    vector<unsigned char> rData(width * height);
    vector<unsigned char> gData(width * height);
    vector<unsigned char> bData(width * height);
    for (int i = 0; i < rgbData.size(); i = i + 3) {
        rData.push_back(rgbData[i]);
        gData.push_back(rgbData[i + 1]);
        bData.push_back(rgbData[i + 2]);
    }

    ofstream rOutStream(outUrls[0]);
    ostream_iterator<unsigned char> rOutBegin(rOutStream);
    std::copy(rData.cbegin(), rData.cend(), rOutBegin);

    ofstream gOutStream(outUrls[1]);
    ostream_iterator<unsigned char> gOutBegin(gOutStream);
    std::copy(gData.cbegin(), gData.cend(), gOutBegin);

    ofstream bOutStream(outUrls[2]);
    ostream_iterator<unsigned char> bOutBegin(bOutStream);
    std::copy(bData.cbegin(), bData.cend(), bOutBegin);
}