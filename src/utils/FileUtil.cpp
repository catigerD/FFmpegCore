//
// Created by dengchong on 2020-01-09.
//

#include "FileUtil.h"

#include <iterator>

using namespace std;

std::shared_ptr<std::vector<char >> FileUtil::getByteVector(const std::string &url) {
    ifstream inputStream(url);
    inputStream.seekg(0, ios::end);
    auto size = inputStream.tellg();
    inputStream.seekg(0, ios::beg);
    istreambuf_iterator<char> inputBegin(inputStream), inputEnd;

    shared_ptr<vector<char >> data = make_shared<vector<char>>();
    data->reserve(size);

    std::copy(inputBegin, inputEnd, back_inserter(*data));

    return data;
}

void FileUtil::writeByteVector(const std::string &url, const std::shared_ptr<std::vector<char >> &data) {
    ofstream outStream(url);
    ostream_iterator<char> outputBegin(outStream);
    std::copy(data->cbegin(), data->cend(), outputBegin);
}