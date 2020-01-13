//
// Created by dengchong on 2020-01-10.
//

#include "H264Util.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <iomanip>

using namespace std;

void H264Util::parseH264(const std::string &inUrl) {
    ifstream inputStream(inUrl);
    cout << "-----+--------  NALU Table  ------+---------+\n" << endl;
    cout << " NUM |    POS  |    IDC   |  TYPE |   LEN   |\n" << endl;
    cout << "-----+---------+----------+-------+---------+\n" << endl;
    int nalNum{};
    int dataOffset{};
    while (!inputStream.eof()) {
        NALUnit nalUnit;
        int dataLength = getAnnexbNALU(inputStream, nalUnit);
        string typeStr;
        switch (nalUnit.nal_unit_type) {
            case NaluType::NALU_TYPE_SLICE:
                typeStr = "SLICE";
                break;
            case NALU_TYPE_DPA:
                typeStr = "DPA";
                break;
            case NALU_TYPE_DPB:
                typeStr = "DPB";
                break;
            case NALU_TYPE_DPC:
                typeStr = "DPC";
                break;
            case NALU_TYPE_IDR:
                typeStr = "IDR";
                break;
            case NALU_TYPE_SEI:
                typeStr = "SEI";
                break;
            case NALU_TYPE_SPS:
                typeStr = "SPS";
                break;
            case NALU_TYPE_PPS:
                typeStr = "PPS";
                break;
            case NALU_TYPE_AUD:
                typeStr = "AUD";
                break;
            case NALU_TYPE_EOSEQ:
                typeStr = "EOSEQ";
                break;
            case NALU_TYPE_EOSTREAM:
                typeStr = "EOSTREAM";
                break;
            case NALU_TYPE_FILL:
                typeStr = "FILL";
                break;
        }
        string idcStr;
        switch (nalUnit.nal_reference_idc >> 5) {
            case NALU_PRIORITY_DISPOSABLE:
                idcStr = "DISPOSABLE";
                break;
            case NALU_PRIORITY_LOW:
                idcStr = "LOW";
                break;
            case NALU_PRIORITY_HIGH:
                idcStr = "HIGH";
                break;
            case NALU_PRIORITY_HIGHEST:
                idcStr = "HIGHEST";
                break;
        }
        cout << setfill(' ') << setw(5) << nalNum << " " << setw(9) << dataOffset << " " << setw(10) << idcStr << " "
             << setw(7) << typeStr << " " << setw(9) << nalUnit.len << " " << endl;
        dataOffset += dataLength;
        nalNum++;
    }
}

int H264Util::getAnnexbNALU(std::ifstream &inputStream, NALUnit &nalUnit) {
    vector<char> workBuf(nalUnit.maxSize);

    //1. 找到curIndex流中的第一个 NALU
    if (!inputStream.read(workBuf.data(), 3)) {
        return 0;
    }
    int startCodePos = findStartCode3(workBuf.begin(), workBuf.begin() + 3);
    int bufIndex = 3;
    if (startCodePos == 0) {
        if (!inputStream.read(workBuf.data() + 3, 1)) {
            return 0;
        }
        startCodePos = findStartCode4(workBuf.begin(), workBuf.begin() + 4);
        if (startCodePos == 0) {
            return 0;
        }
        bufIndex = 4;
        nalUnit.startCodeLen = 4;
    } else {
        bufIndex = 3;
        nalUnit.startCodeLen = 3;
    }

    //2. 找流中下一个 NALU
    startCodePos = 0;
    while (startCodePos == 0) {
        if (inputStream.eof()) {
            nalUnit.len = bufIndex - 1 - nalUnit.startCodeLen;
            std::copy(workBuf.cbegin() + nalUnit.startCodeLen, workBuf.cbegin() + nalUnit.startCodeLen + nalUnit.len,
                      back_inserter(nalUnit.buf));
            nalUnit.forbidden_bit = nalUnit.buf[0] & 0x80;
            nalUnit.nal_reference_idc = nalUnit.buf[0] & 0x60;
            nalUnit.nal_unit_type = nalUnit.buf[0] & 0x1f;
            return bufIndex - 1;
        }
        inputStream.read(workBuf.data() + bufIndex++, 1);
        startCodePos = findStartCode4(workBuf.begin() + bufIndex - 4, workBuf.begin() + bufIndex);
        if (startCodePos == 0) {
            startCodePos = findStartCode3(workBuf.begin() + bufIndex - 3, workBuf.begin() + bufIndex);
        }
    }
    int rewind = -startCodePos;
    inputStream.seekg(rewind, ios::cur);

    //3. 填充 nalUnit 内容
    nalUnit.len = (bufIndex + rewind) - nalUnit.startCodeLen;
    std::copy(workBuf.cbegin() + nalUnit.startCodeLen, workBuf.cbegin() + nalUnit.startCodeLen + nalUnit.len,
              back_inserter(nalUnit.buf));
    nalUnit.forbidden_bit = nalUnit.buf[0] & 0x80;
    nalUnit.nal_reference_idc = nalUnit.buf[0] & 0x60;
    nalUnit.nal_unit_type = nalUnit.buf[0] & 0x1f;

    return bufIndex + rewind;
}

int
H264Util::findStartCode3(const std::vector<char>::const_iterator &begin, const std::vector<char>::const_iterator &end) {
    if (end - begin < 3) {
        return 0;
    }
    if ((*begin) == 0 && *(begin + 1) == 0 && *(begin + 2) == 1) {
        return 3;
    }
    return 0;
}

int
H264Util::findStartCode4(const std::vector<char>::const_iterator &begin, const std::vector<char>::const_iterator &end) {
    if (end - begin < 4) {
        return 0;
    }
    if (*(begin) == 0 && *(begin + 1) == 0 && *(begin + 2) == 0 && *(begin + 3) == 1) {
        return 4;
    }
    return 0;
}