//
// Created by dengchong on 2020-01-10.
//

#ifndef FFMPEGCORE_H264UTIL_H
#define FFMPEGCORE_H264UTIL_H

#include <string>
#include <vector>
#include <fstream>

struct NALUnit;

class H264Util {
public:
    static void parseH264(const std::string &inUrl);

private:
    static int getAnnexbNALU(std::ifstream &inputStream, NALUnit &nalUnit);

    static int findStartCode3(const std::vector<char >::const_iterator& begin,const std::vector<char >::const_iterator& end);

    static int findStartCode4(const std::vector<char >::const_iterator& begin,const std::vector<char >::const_iterator& end);
};

enum NaluType {
    NALU_TYPE_SLICE = 1,
    NALU_TYPE_DPA = 2,
    NALU_TYPE_DPB = 3,
    NALU_TYPE_DPC = 4,
    NALU_TYPE_IDR = 5,    //-IDR(Instantaneous Decoding Refresh)：即时解码刷新。
    NALU_TYPE_SEI = 6,    //-SEI(Supplemental enhancement information)：附加增强信息，包含了视频画面定时等信息，一般放在主编码图像数据之前，在某些应用中，它可以被省略掉。
    NALU_TYPE_SPS = 7,    //-SPS(Sequence Parameter Sets)：序列参数集，作用于一系列连续的编码图像。
    NALU_TYPE_PPS = 8,    //-PPS(Picture Parameter Set)：图像参数集，作用于编码视频序列中一个或多个独立的图像。
    NALU_TYPE_AUD = 9,
    NALU_TYPE_EOSEQ = 10,
    NALU_TYPE_EOSTREAM = 11,
    NALU_TYPE_FILL = 12,
};//-NALU类型

enum NaluPriority {
    NALU_PRIORITY_DISPOSABLE = 0,
    NALU_PRIORITY_LOW = 1,
    NALU_PRIORITY_HIGH = 2,
    NALU_PRIORITY_HIGHEST = 3
};//-NALU优先级

/*
NAL Header由三部分组成：
forbidden_bit(1bit)
nal_reference_bit(2bits)（优先级）
nal_unit_type(5bits)（类型）
*/
struct NALUnit {
    int startCodeLen{};      //! 4 for parameter sets and first slice in picture, 3 for everything else (suggested)
    unsigned len{};                 //! Length of the NAL unit (Excluding the start code, which does not belong to the NALU)
    unsigned maxSize{100000};
    std::vector<char> buf;        //! contains the first byte followed by the EBSP
    //-下面三个变量是NAL Header
    int forbidden_bit{};            //! should be always FALSE
    int nal_reference_idc{};        //! NALU_PRIORITY_xxxx
    int nal_unit_type{};            //! NALU_TYPE_xxxx
};//-h.264的单元



#endif //FFMPEGCORE_H264UTIL_H
