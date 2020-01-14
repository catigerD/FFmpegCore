//
// Created by dengchong on 2020-01-14.
//

#include "FLVUtil.h"

#include <fstream>
#include <iostream>

using namespace std;

void FLVUtil::parserFLV(const std::string &inUrl) {
    ifstream inputStream(inUrl);

    FLVHeader flvHeader;
    TagHeader tagHeader;

    inputStream.read(reinterpret_cast<char *>(&flvHeader), sizeof(FLVHeader));
    cout << "============== FLV Header ==============" << endl;
    cout << "Signature:  0x" << " " << flvHeader.signature[0] << " " << flvHeader.signature[1] << " "
         << flvHeader.signature[2] << endl;
    cout << "Version:    0x" << " " << flvHeader.version + 0 << endl;
    cout << "Flags  :    0x" << " " << flvHeader.flags + 0 << endl;
    cout << "HeaderSize: 0x" << " "
         << reverseBytes(reinterpret_cast<uint8_t *>(&flvHeader.dataOffset), sizeof(flvHeader.dataOffset)) << endl;
    cout << "========================================" << endl;

    int previousTagSize{};
    //从流中读取整数，c 中 getw(File *f)
    //C++中
//    inputStream.read(reinterpret_cast<char *>(&previousTagSize), sizeof(previousTagSize));
//    inputStream >> previousTagSize;
//    cout << previousTagSize << endl;
    //process each tag
    do {
        inputStream.read(reinterpret_cast<char *>(&previousTagSize), sizeof(previousTagSize));
        inputStream.read(reinterpret_cast<char *>(&tagHeader), sizeof(tagHeader));

        int tagHeaderDataSize = tagHeader.dataSize[0] * 65536 + tagHeader.dataSize[1] * 256 + tagHeader.dataSize[2];
        int tagHeaderTimestamp = tagHeader.timestamp[0] * 65536 + tagHeader.timestamp[1] * 256 + tagHeader.timestamp[2];
        string tagTypeStr;
        switch (tagHeader.tagType) {
            case TAG_TYPE_AUDIO:
                tagTypeStr = "AUDIO";
                break;
            case TAG_TYPE_VIDEO:
                tagTypeStr = "VIDEO";
                break;
            case TAG_TYPE_SCRIPT:
                tagTypeStr = "SCRIPT";
                break;
            default:
                tagTypeStr = "UNKNOWN";
                break;
        }
        cout << setfill(' ') << setw(6) << tagTypeStr
             << setw(6) << tagHeaderDataSize
             << setw(6) << tagHeaderTimestamp;
        if (inputStream.eof()) {
            break;
        }
        switch (tagHeader.tagType) {
            case TAG_TYPE_AUDIO: {
                string audioTagStr("| ");
                char tagAudioDataFirstByte{};
                inputStream.read(&tagAudioDataFirstByte, 1);
                int audioEncodeType = (tagAudioDataFirstByte & 0xF0) >> 4;
                switch (audioEncodeType) {
                    case 0:
                        audioTagStr.append("Linear PCM, platform endian");
                        break;
                    case 1:
                        audioTagStr.append("ADPCM");
                        break;
                    case 2:
                        audioTagStr.append("MP3");
                        break;
                    case 3:
                        audioTagStr.append("Linear PCM, little endian");
                        break;
                    case 4:
                        audioTagStr.append("Nellymoser 16-kHz mono");
                        break;
                    case 5:
                        audioTagStr.append("Nellymoser 8-kHz mono");
                        break;
                    case 6:
                        audioTagStr.append("Nellymoser");
                        break;
                    case 7:
                        audioTagStr.append("G.711 A-law logarithmic PCM");
                        break;
                    case 8:
                        audioTagStr.append("G.711 mu-law logarithmic PCM");
                        break;
                    case 9:
                        audioTagStr.append("reserved");
                        break;
                    case 10:
                        audioTagStr.append("AAC");
                        break;
                    case 11:
                        audioTagStr.append("Speex");
                        break;
                    case 14:
                        audioTagStr.append("MP3 8-Khz");
                        break;
                    case 15:
                        audioTagStr.append("Device-specific sound");
                        break;
                    default:
                        audioTagStr.append("UNKNOWN");
                        break;
                }
                audioTagStr.append("| ");
                int sampleRate = (tagAudioDataFirstByte & 0x0C) >> 2;
                switch (sampleRate) {
                    case 0:
                        audioTagStr.append("5.5-kHz");
                        break;
                    case 1:
                        audioTagStr.append("1-kHz");
                        break;
                    case 2:
                        audioTagStr.append("22-kHz");
                        break;
                    case 3:
                        audioTagStr.append("44-kHz");
                        break;
                    default:
                        audioTagStr.append("UNKNOWN");
                        break;
                }
                audioTagStr.append("| ");
                int format = (tagAudioDataFirstByte & 0x02) >> 1;
                switch (format) {
                    case 0:
                        audioTagStr.append("8Bit");
                        break;
                    case 1:
                        audioTagStr.append("16Bit");
                        break;
                    default:
                        audioTagStr.append("UNKNOWN");
                        break;
                }
                audioTagStr.append("| ");
                int channel = (tagAudioDataFirstByte & 0x01);
                switch (channel) {
                    case 0:
                        audioTagStr.append("Mono");
                        break;
                    case 1:
                        audioTagStr.append("Stereo");
                        break;
                    default:
                        audioTagStr.append("UNKNOWN");
                        break;
                }
                cout << audioTagStr << endl;
                inputStream.seekg(tagHeaderDataSize - 1, ios::cur);
                break;
            }
            case TAG_TYPE_VIDEO: {
                string tagVideoStr("| ");
                char tagVideoDataFirstByte{};
                inputStream.read(&tagVideoDataFirstByte, 1);
                int frameType = (tagVideoDataFirstByte & 0xF0) >> 4;
                switch (frameType) {
                    case 1:
                        tagVideoStr.append("key frame  ");
                        break;
                    case 2:
                        tagVideoStr.append("inter frame");
                        break;
                    case 3:
                        tagVideoStr.append("disposable inter frame");
                        break;
                    case 4:
                        tagVideoStr.append("generated keyframe");
                        break;
                    case 5:
                        tagVideoStr.append("video info/command frame");
                        break;
                    default:
                        tagVideoStr.append("UNKNOWN");
                        break;
                }
                tagVideoStr.append("| ");
                int videoEncodeType = (tagVideoDataFirstByte & 0x0F);
                switch (videoEncodeType) {
                    case 1:
                        tagVideoStr.append("JPEG (currently unused)");
                        break;
                    case 2:
                        tagVideoStr.append("Sorenson H.263");
                        break;
                    case 3:
                        tagVideoStr.append("Screen video");
                        break;
                    case 4:
                        tagVideoStr.append("On2 VP6");
                        break;
                    case 5:
                        tagVideoStr.append("On2 VP6 with alpha channel");
                        break;
                    case 6:
                        tagVideoStr.append("Screen video version 2");
                        break;
                    case 7:
                        tagVideoStr.append("AVC");
                        break;
                }
                cout << tagVideoStr << endl;
                inputStream.seekg(tagHeaderDataSize - 1, ios::cur);
                break;
            }
            default: {
                cout << endl;
                inputStream.seekg(tagHeaderDataSize, ios::cur);
                break;
            }
        }

    } while (!inputStream.eof());
}

uint32_t FLVUtil::reverseBytes(uint8_t *bytes, int size) {
    uint32_t r{};
    for (int i = 0; i < size; ++i) {
        r |= (*(bytes + i) << (size - 1 - i) * 8);
    }
    return r;
}