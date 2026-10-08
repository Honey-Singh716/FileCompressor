#ifndef ENCODER_H
#define ENCODER_H

#include "BitWriter.h"

#include <cstdint>
#include <string>
#include <unordered_map>

class Encoder {

public:

    static void encodeFile(
        const std::string& inputFileName,
        BitWriter& writer,
        const std::unordered_map<
            uint8_t,
            std::string
        >& codes
    );
};

#endif