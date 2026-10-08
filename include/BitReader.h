#ifndef BIT_READER_H
#define BIT_READER_H

#include <fstream>
#include <cstdint>
#include <string>

class BitReader {
private:
    std::ifstream inputFile;

    uint8_t buffer; 
    int bitsRemaining;

public:
    BitReader(const std::string& fileName);
    ~BitReader();

    bool readBit(bool& bit);
};

#endif 