#ifndef DECOMPRESSOR_H
#define DECOMPRESSOR_H

#include <string>

class Decompressor {
public:
    static void decompress(
        const std::string& inputFileName,
        const std::string& outputFileName
    );
};

#endif