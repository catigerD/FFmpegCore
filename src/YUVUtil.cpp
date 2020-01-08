//
// Created by dengchong on 2020-01-07.
//

#include "YUVUtil.h"

#include <fstream>
#include <iterator>
#include <cmath>
#include <iostream>
#include <array>

using std::string;
using std::ifstream;
using std::ofstream;
using std::ostream_iterator;
using std::istreambuf_iterator;
using std::vector;
using std::array;
using std::back_inserter;

void YUVUtil::splitYUV420p(const std::string &inUrl, unsigned int width, unsigned int height,
                           const std::array<std::string, 3> &outUrls) {
    std::vector<unsigned char> yuvData;
    yuvData.reserve(width * height * 2);
    std::ifstream inputStream(inUrl);
    std::istreambuf_iterator<char> inputBegin(inputStream), inputEnd;
    std::copy(inputBegin, inputEnd, back_inserter(yuvData));
    std::ofstream outputYStream(outUrls[0]);
    auto yBegin = yuvData.cbegin();
    std::ostream_iterator<unsigned char> outYBegin(outputYStream);
    std::copy(yBegin, yBegin + width * height, outYBegin);
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
    std::vector<unsigned char> yuvData;
    yuvData.reserve(width * height * 2);
    std::ifstream inputStream(inUrl);
    std::istreambuf_iterator<char> inputBegin(inputStream), inputEnd;
    std::copy(inputBegin, inputEnd, back_inserter(yuvData));
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
    vector<unsigned char> yuvData;
    yuvData.reserve(width * height * 2);
    ifstream inputStream(inUrl);
    istreambuf_iterator<char> inputBegin(inputStream), inputEnd;
    std::copy(inputBegin, inputEnd, back_inserter(yuvData));
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
    vector<unsigned char> yuvData;
    yuvData.reserve(width * height * 2);
    ifstream inputStream(inUrl);
    istreambuf_iterator<char> inputBegin(inputStream), inputEnd;
    std::copy(inputBegin, inputEnd, back_inserter(yuvData));
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
    vector<unsigned char> srcYuvData;
    srcYuvData.reserve(width * height * 2);
    ifstream srcInputStream(srcUrl);
    istreambuf_iterator<char> srcInputBegin(srcInputStream), srcInputEnd;
    std::copy(srcInputBegin, srcInputEnd, back_inserter(srcYuvData));
    vector<unsigned char> dstYuvData;
    dstYuvData.reserve(width * height * 2);
    ifstream destInputStream(dstUrl);
    istreambuf_iterator<char> dstInputBegin(destInputStream), dstInputEnd;
    std::copy(dstInputBegin, dstInputEnd, back_inserter(dstYuvData));

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
    ifstream inputStream(inUrl);
    istreambuf_iterator<char> inputBegin(inputStream), inputEnd;
    vector<unsigned char> rgbData(inputBegin, inputEnd);

    vector<unsigned char> rData;
    rData.reserve(width * height);
    vector<unsigned char> gData;
    gData.reserve(width * height);
    vector<unsigned char> bData;
    bData.reserve(width * height);
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

//ifstream 读取 rgb size 变小？原始 256x256x3 = 196608, 文件读取后 195617。yuv 数据无问题
void YUVUtil::rgb24ToBmp(const std::string &inUrl, unsigned int width, unsigned int height,
                         const std::string &outUrl) {
    ifstream inputStream(inUrl, std::ios::binary);
    istreambuf_iterator<char> inputBegin(inputStream), inputEnd;
    vector<unsigned char> rgbData(inputBegin, inputEnd);

    BitmapFileHeader bmpHead;
    ///* Magic number for file. It does not fit in the header structure due to alignment requirements, so put it outside */
    unsigned short bfType = 0x4d42; //This field is _not_ included in `BITMAPFILEHEADER`
    int headerSize = sizeof(bfType) + sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader);
    bmpHead.bfSize = 3 * width * height + headerSize;
    bmpHead.bfbfOffBits = headerSize;
    BitmapInfoHeader infoHead;
    infoHead.biSize = sizeof(BitmapInfoHeader);
    infoHead.biWidth = width;
    ////BMP storage pixel data in opposite direction of Y-axis (from bottom to top).
    infoHead.biHeight = -height;
    infoHead.biPlanes = 1;
    infoHead.biBitcount = 24;
    infoHead.biSizeImage = 3 * width * height;

    ofstream outputStream(outUrl);
    outputStream.write(reinterpret_cast<const char *>(&bfType), sizeof(bfType));
    outputStream.write(reinterpret_cast<const char *>(&bmpHead), sizeof(BitmapFileHeader));
    outputStream.write(reinterpret_cast<const char *>(&infoHead), sizeof(BitmapInfoHeader));
    //BMP save R1|G1|B1,R2|G2|B2 as B1|G1|R1,B2|G2|R2
    //It saves pixel data in Little Endian
    //So we change 'R' and 'B'
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            auto temp = rgbData[(i * width + j) * 3];
            rgbData[(i * width + j) * 3] = rgbData[(i * width + j) * 3 + 2];
            rgbData[(i * width + j) * 3 + 2] = temp;
        }
    }
    ostream_iterator<char> outBegin(outputStream);
    std::copy(rgbData.cbegin(), rgbData.cend(), outBegin);

//    FILE *fpRgb24 = nullptr, *fpBmp = nullptr;
//    fpRgb24 = fopen(inUrl.c_str(), "rb");
//    fpBmp = fopen(outUrl.c_str(), "wb");
//    unsigned char rgb24Buffer[width * height * 3];
//    fread(rgb24Buffer, 1, width * height * 3, fpRgb24);
//    for (int i = 0; i < height; ++i) {
//        for (int j = 0; j < width; ++j) {
//            auto temp = rgb24Buffer[(i * width + j) * 3];
//            rgb24Buffer[(i * width + j) * 3] = rgb24Buffer[(i * width + j) * 3 + 2];
//            rgb24Buffer[(i * width + j) * 3 + 2] = temp;
//        }
//    }
//
//    fwrite(&bfType, 1, sizeof(bfType), fpBmp);
//    fwrite(&bmpHead, 1, sizeof(BitmapFileHeader), fpBmp);
//    fwrite(&infoHead, 1, sizeof(BitmapInfoHeader), fpBmp);
//    fwrite(rgb24Buffer, width * height * 3, 1, fpBmp);
//
//    fclose(fpRgb24);
//    fclose(fpBmp);
}

void YUVUtil::rgb24ToYuv420p(const std::string &inUrl, unsigned int width, unsigned int height,
                             const std::string &outUrl) {
    vector<unsigned char> rgbData;
    rgbData.reserve(width * height * 3);
    vector<unsigned char> yuvData(width * height * 3 / 2);

    ifstream inputStream(inUrl);
    istreambuf_iterator<char> inputBegin(inputStream), inputEnd;
    rgbData.insert(rgbData.begin(), inputBegin, inputEnd);

    auto yItr = yuvData.begin();
    auto uItr = yItr + width * height;
    auto vItr = uItr + width * height / 4;

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            auto r = rgbData[(i * width + j) * 3];
            auto g = rgbData[(i * width + j) * 3 + 1];
            auto b = rgbData[(i * width + j) * 3 + 2];
            unsigned char y = ((66 * r + 129 * g + 25 * b + 128) >> 8) + 16;
            unsigned char u = ((-38 * r - 74 * g + 112 * b + 128) >> 8) + 128;
            unsigned char v = ((112 * r - 94 * g - 18 * b + 128) >> 8) + 128;
            *(yItr++) = clipValue(y, 0, 255);
            if (i % 2 == 0 && j % 2 == 0) {
                *(uItr++) = clipValue(u, 16, 235);
            } else {
                *(vItr++) = clipValue(v, 16, 239);
            }
        }
    }

    ofstream outputStream(outUrl);
    ostream_iterator<unsigned char> outBegin(outputStream);
    std::copy(yuvData.cbegin(), yuvData.cend(), outBegin);
}

unsigned char YUVUtil::clipValue(unsigned char target, unsigned char min, unsigned char max) {
    if (target > max) {
        return max;
    } else if (target < min) {
        return min;
    } else {
        return target;
    }
}