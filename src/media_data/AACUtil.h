//
// Created by dengchong on 2020-01-13.
//

#ifndef FFMPEGCORE_AACUTIL_H
#define FFMPEGCORE_AACUTIL_H

#include <string>
#include <vector>
#include <iterator>

class AACUtil {
    typedef std::vector<unsigned char>::const_iterator Itr;
    typedef const std::vector<unsigned char>::const_iterator ConstItr;

public:

    static void parserAAC(const std::string &inUrl);

private:
    static int getADTSFrame(Itr &inputBegin,
                            ConstItr &inputEnd,
                            std::vector<unsigned char> &data);
};


#endif //FFMPEGCORE_AACUTIL_H
