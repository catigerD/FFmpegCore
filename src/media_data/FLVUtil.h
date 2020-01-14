//
// Created by dengchong on 2020-01-14.
//

#ifndef FFMPEGCORE_FLVUTIL_H
#define FFMPEGCORE_FLVUTIL_H

#include <cstdint>
#include <vector>
#include <string>

class FLVUtil {
public:
    static const int TAG_TYPE_AUDIO = 8;
    static const int TAG_TYPE_VIDEO = 9;
    static const int TAG_TYPE_SCRIPT = 18;

    static void parserFLV(const std::string &inUrl);

private:
    //reverse bytes - turn a BigEndian byte array into a LittleEndian integer
    static uint32_t reverseBytes(uint8_t *bytes, int size);
};

#pragma pack(push, 1)

struct FLVHeader {
    uint8_t signature[3];
    uint8_t version{};
    uint8_t flags{};
    uint32_t dataOffset{};
};

struct TagHeader {
    uint8_t tagType{};
    uint8_t dataSize[3];
    uint8_t timestamp[3];
    uint32_t reserved;
};

#pragma pack(pop)


#endif //FFMPEGCORE_FLVUTIL_H
