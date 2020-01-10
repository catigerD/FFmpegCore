//
// Created by dengchong on 2020-01-09.
//

#include "PcmUtil.h"

#include <memory>
#include <fstream>
#include <iterator>

#include "FileUtil.h"

using namespace std;

void PcmUtil::splitPcm16LE(const std::string &inUrl, std::array<std::string, 2> &outUrls) {
    auto pcmData = FileUtil::getByteVector(inUrl);

    auto lPcmData = make_shared<vector<char >>();
    lPcmData->reserve(pcmData->size() / 2);
    auto rPcmData = make_shared<vector<char >>();
    rPcmData->reserve(pcmData->size() / 2);
    for (int i = 0; i < pcmData->size(); i += 4) {
        lPcmData->push_back((*pcmData)[i]);
        lPcmData->push_back((*pcmData)[i + 1]);
        rPcmData->push_back((*pcmData)[i + 2]);
        rPcmData->push_back((*pcmData)[i + 3]);
    }

    FileUtil::writeByteVector(outUrls[0], lPcmData);
    FileUtil::writeByteVector(outUrls[1], rPcmData);
}

void PcmUtil::halfVolumeLToPcm16LE(const std::string &inUrl, const std::string &outUrl) {
    auto srcData = FileUtil::getByteVector(inUrl);

    for (int i = 0; i < srcData->size(); i += 4) {
        short *lSample = nullptr;
        lSample = reinterpret_cast<short *>(srcData->data() + i);
        (*lSample) = (*lSample) / 2;
    }

    FileUtil::writeByteVector(outUrl, srcData);
}

void PcmUtil::doubleSpeedPcm16LE(const std::string &inUrl, const std::string &outUrl) {
    auto pcmData = FileUtil::getByteVector(inUrl);

    auto dstData = make_shared<vector<char >>();
    dstData->reserve(pcmData->size());

    for (int i = 0; i < pcmData->size(); ++i) {
        if (i % 2 != 0) {
            dstData->push_back((*pcmData)[i]);
        }
    }

    FileUtil::writeByteVector(outUrl, dstData);
}

void PcmUtil::pcm16LE2Pcm8(const std::string &inUrl, const std::string &outUrl) {
    auto srcData = FileUtil::getByteVector(inUrl);
    auto dstData = make_shared<vector<char >>();
    dstData->reserve(srcData->size() / 2);

    for (int i = 0; i < srcData->size(); i += 2) {
        short *sample = nullptr;
        sample = reinterpret_cast<short *>(srcData->data() + i);
        unsigned char dstSample = ((*sample) >> 8) + 128;
        dstData->push_back(dstSample);
    }

    FileUtil::writeByteVector(outUrl, dstData);
}

void PcmUtil::cutSingleChannelPcm16LE(const std::string &inUrl, unsigned int start, unsigned int dur,
                                      const std::string &outUrl) {
    auto srcData = FileUtil::getByteVector(inUrl);
    auto dstData = make_shared<vector<char>>();
    dstData->reserve(dur);

    std::copy(srcData->cbegin() + start * 2, srcData->cbegin() + (start + dur) * 2, back_inserter(*dstData));

    FileUtil::writeByteVector(outUrl, dstData);
}

void PcmUtil::pcm16LE2Wave(const std::string &inUrl, int channels, int sampleRate, const std::string &outUrl) {
    auto pcmData = FileUtil::getByteVector(inUrl);

    ofstream outputStream(outUrl);
    WAVE_HEADER waveHeader;
    outputStream.seekp(sizeof(WAVE_HEADER), ios::beg);
    WAVE_FMT waveFmt;
    if (channels == 0 || sampleRate == 0) {
        channels = 2;
        sampleRate = 44100;
    }
    waveFmt.dwSamplesPerSec = sampleRate;
    waveFmt.dwAvgBytesPerSec = waveFmt.dwSamplesPerSec * 2;
    waveFmt.wChannels = channels;
    outputStream.write(reinterpret_cast<const char *>(&waveFmt), sizeof(WAVE_FMT));
    WAVE_DATA waveData;
    waveData.dwSize = pcmData->size();
    outputStream.write(reinterpret_cast<const char *>(&waveData), sizeof(WAVE_DATA));
    ostream_iterator<char> outputBegin(outputStream);
    std::copy(pcmData->cbegin(), pcmData->cend(), outputBegin);
    waveHeader.dwSize = sizeof(waveHeader) + sizeof(waveFmt) + sizeof(waveData) + waveData.dwSize;
    outputStream.seekp(0, ios::beg);
    outputStream.write(reinterpret_cast<const char *>(&waveHeader), sizeof(WAVE_HEADER));
}
