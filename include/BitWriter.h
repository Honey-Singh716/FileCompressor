#ifndef BIT_WRITER_H
#define BIT_WRITER_H

#include <fstream>
#include <cstdint>

class BitWriter {
private:
    std::ofstream& outputFile;

    uint8_t buffer;
    int bitCount;

public:
    BitWriter(std::ofstream& outputFile);

    void writeBit(bool bit);

    void flush();
};

#endif