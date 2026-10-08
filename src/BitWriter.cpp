#include "../include/BitWriter.h"

BitWriter::BitWriter(std::ofstream& outputFile)
    : outputFile(outputFile),
      buffer(0),
      bitCount(0) {
}

void BitWriter::writeBit(bool bit) {

    buffer = (buffer << 1) | bit;

    bitCount++;

    if (bitCount == 8) {

        outputFile.put(
            static_cast<char>(buffer)
        );

        buffer = 0;
        bitCount = 0;
    }
}

void BitWriter::flush() {

    if (bitCount > 0) {

        buffer <<= (8 - bitCount);

        outputFile.put(
            static_cast<char>(buffer)
        );

        buffer = 0;
        bitCount = 0;
    }

    outputFile.flush();
}