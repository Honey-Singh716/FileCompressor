#include "../include/BitReader.h"
#include <stdexcept>

BitReader::BitReader(const std::string& fileName) {
    inputFile.open(fileName, std::ios::binary);

    if (!inputFile) {
        throw std::runtime_error("Could not open input file.");
    }

    buffer = 0;
    bitsRemaining = 0;
}


bool BitReader::readBit(bool& bit) {
    if (bitsRemaining == 0) {
        char byte;

        if (!inputFile.get(byte)) {
            return false;
        }

        buffer = static_cast<uint8_t>(byte);
        bitsRemaining = 8;
    }

    bit = (buffer & 0x80) != 0;

    buffer <<= 1;
    bitsRemaining--;

    return true;
}

BitReader::~BitReader() {
    if (inputFile.is_open()) {
        inputFile.close();
    }
}
