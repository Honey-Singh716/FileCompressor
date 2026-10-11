#ifndef DECODER_H
#define DECODER_H

#include "BitReader.h"
#include "HuffmanNode.h"

#include <cstdint>
#include <fstream>

class Decoder {
public:
    static void decodeFile(
        BitReader& reader,
        HuffmanNode* root,
        uint64_t originalSize,
        std::ofstream& outputFile
    );
};

#endif