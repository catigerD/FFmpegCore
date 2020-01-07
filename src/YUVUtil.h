//
// Created by dengchong on 2020-01-07.
//

#ifndef FFMPEGCORE_YUVUTIL_H
#define FFMPEGCORE_YUVUTIL_H

#include <string>
#include <vector>
#include <array>

class YUVUtil {
public:

    //分离 YUV420p 像素数据中的 Y,U,V 分量
    static void splitYUV420p(const std::string &inUrl, unsigned int width, unsigned int height,
                             const std::array<std::string, 3> &outUrls);

    //将 YUV420p 像素数据去掉颜色(变成灰度图)
    static void grayYUV420p(const std::string &inUrl, unsigned int width, unsigned int height,
                            const std::string &outUrl);

    //将 YUV420p 像素数据的亮度减半
    static void halfyYUV420p(const std::string &inUrl, unsigned int width, unsigned int height,
                             const std::string &outUrl);

    //将 YUV420p像素数据的周围加上边框
    static void borderYUV420p(const std::string &inUrl, unsigned int width, unsigned int height,
                              unsigned int border, const std::string &outUrl);

    //计算两个 YUV420p 像素数据 Y 的 PSNR
    static void psnrYUV420p(const std::string &srcUrl, const std::string &dstUrl,
                            unsigned int width, unsigned int height);

    //分离 RGB24 像素数据中的 R,G,B 分量
    static void splitRGB24(const std::string &inUrl, unsigned int width, unsigned int height,
                             const std::array<std::string, 3> &outUrls);

};


#endif //FFMPEGCORE_YUVUTIL_H
