//
// Created by dengchong on 2020-01-07.
//

#ifndef FFMPEGCORE_YUVUTIL_H
#define FFMPEGCORE_YUVUTIL_H

#include <string>
#include <vector>
#include <array>
#include <fstream>

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

    //将 RGB24 格式像素数据封装为 BMP 图像
    static void rgb24ToBmp(const std::string &inUrl, unsigned int width, unsigned int height,
                           const std::string &outUrl);
};

//BMP
struct BitmapFileHeader {
//    unsigned short bfType{0x424d};       //位图文件的类型，必须为BM
    unsigned int bfSize{};       //文件大小，以字节为单位
    unsigned short bfReserverd1{}; //位图文件保留字，必须为0
    unsigned short bfReserverd2{}; //位图文件保留字，必须为0
    unsigned int bfbfOffBits{};  //位图文件头到数据的偏移量，以字节为单位
};

struct BitmapInfoHeader {
    unsigned int biSize{};                        //该结构大小，字节为单位
    int biWidth{};                     //图形宽度以象素为单位
    int biHeight{};                     //图形高度以象素为单位
    unsigned short biPlanes{};               //目标设备的级别，必须为1
    unsigned short biBitcount{};             //颜色深度，每个象素所需要的位数
    unsigned int biCompression{};        //位图的压缩类型
    unsigned int biSizeImage{};              //位图的大小，以字节为单位
    int biXPelsPermeter{};       //位图水平分辨率，每米像素数
    int biYPelsPermeter{};       //位图垂直分辨率，每米像素数
    unsigned int biClrUsed{};            //位图实际使用的颜色表中的颜色数
    unsigned int biClrImportant{};       //位图显示过程中重要的颜色数
};


#endif //FFMPEGCORE_YUVUTIL_H
