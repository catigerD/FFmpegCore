//
// Created by dengchong on 2020-01-13.
//

#include "AACUtil.h"

#include <iterator>
#include <fstream>
#include <iostream>

using namespace std;

void AACUtil::parserAAC(const std::string &inUrl) {
    ifstream inputStream(inUrl);
    cout << "-----+- ADTS Frame Table -+------+" << endl;
    cout << " NUM | Profile | Frequency| Size |" << endl;
    cout << "-----+---------+----------+------+" << endl;

    vector<unsigned char> aacBuf(1024 * 1024);

    int offset{};
    int bufSize{};
    int count{};
    int dataSize{};

    while (!inputStream.eof()) {
        inputStream.read(reinterpret_cast<char *>(aacBuf.data()), 1024 * 1024 - offset);
        bufSize = inputStream.gcount();
        auto bufBegin = aacBuf.cbegin();
        auto bufEnd = aacBuf.cbegin() + bufSize;
        while (true) {
            vector<unsigned char> aacFrame;
            aacFrame.reserve(1024 * 5);
            int ret = getADTSFrame(bufBegin, bufEnd, aacFrame);
            if (ret < 0) {
                break;
            } else if (ret == 1) {
                std::copy(bufBegin, bufEnd, aacBuf.begin());
                offset = bufSize;
            }
            dataSize = aacFrame.size();
            string profileStr;
            string frequenceStr;
            unsigned char profile = (aacFrame[2] & 0xC0) >> 6;
            switch (profile) {
                case 0:
                    profileStr = "Main";
                    break;
                case 1:
                    profileStr = "LC";
                    break;
                case 2:
                    profileStr = "SSR";
                    break;
                default:
                    profileStr = "unknown";
                    break;
            }
            unsigned char samplingFrequencyIndex = (aacFrame[2] & 0x3C) >> 2;
            switch (samplingFrequencyIndex) {
                case 0:
                    frequenceStr = "96000Hz";
                    break;
                case 1:
                    frequenceStr = "88200Hz";
                    break;
                case 2:
                    frequenceStr = "64000Hz";
                    break;
                case 3:
                    frequenceStr = "48000Hz";
                    break;
                case 4:
                    frequenceStr = "44100Hz";
                    break;
                case 5:
                    frequenceStr = "32000Hz";
                    break;
                case 6:
                    frequenceStr = "24000Hz";
                    break;
                case 7:
                    frequenceStr = "22050Hz";
                    break;
                case 8:
                    frequenceStr = "16000Hz";
                    break;
                case 9:
                    frequenceStr = "12000Hz";
                    break;
                case 10:
                    frequenceStr = "11025Hz";
                    break;
                case 11:
                    frequenceStr = "8000Hz";
                    break;
                default:
                    frequenceStr = "unknown";
                    break;
            }
            cout << setfill(' ') << setw(5) << count << " "
                 << setw(9) << profileStr << " "
                 << setw(10) << frequenceStr << " "
                 << setw(6) << dataSize << " "
                 << endl;
            count++;
            bufSize -= dataSize;
            bufBegin += dataSize;
        }
    }
}

int AACUtil::getADTSFrame(Itr &inputBegin,
                          ConstItr &inputEnd,
                          std::vector<unsigned char> &data) {
    auto bufSize = inputEnd - inputBegin;
    if (bufSize == 0) {
        return -1;
    }
    int frameSize{};
    while (true) {
        if (bufSize < 7) {
            return -1;
        }
        //sync words
        if ((*inputBegin) == 0xff
            && (*(inputBegin + 1) & 0xf0) == 0xf0) {
            frameSize |= (*(inputBegin + 3) & 0x03) << 11; //high 2 bit
            frameSize |= *(inputBegin + 4) << 3;
            frameSize |= (*(inputBegin + 5) & 0xe0) >> 5;
            break;
        }
        inputBegin++;
        bufSize--;
    }
    if (bufSize < frameSize) {
        return 1;
    }
    std::copy(inputBegin, inputBegin + frameSize, back_inserter(data));
    return 0;
}