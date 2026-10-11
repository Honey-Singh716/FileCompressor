#include "../include/BitReader.h"

BitReader::BitReader(std::ifstream& inputFile)
    : inputFile(inputFile),
      buffer(0),
      bitsRemaining(0) {
}

bool BitReader::readBit(bool& bit) {

    if (bitsRemaining == 0) {
        char byte;

        if (!inputFile.get(byte)) {
            return false;
        }

        buffer = static_cast<uint8_t>(
            static_cast<unsigned char>(byte)
        );

        bitsRemaining = 8;
    }

    bit = (buffer & 0x80) != 0;

    buffer <<= 1;
    bitsRemaining--;

    return true;
}