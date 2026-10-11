#ifndef BIT_READER_H
#define BIT_READER_H

#include <cstdint>
#include <fstream>

class BitReader {
private:
    std::ifstream& inputFile;
    uint8_t buffer;
    int bitsRemaining;

public:
    BitReader(std::ifstream& inputFile);

    bool readBit(bool& bit);
};

#endif