#include "../include/FrequencyTable.h"

#include <fstream>
#include <stdexcept>

void FrequencyTable::buildFromFile(
    const std::string& fileName
) {
    frequencies.fill(0);
    totalBytes = 0;

    std::ifstream inputFile(
        fileName,
        std::ios::binary
    );

    if (!inputFile) {
        throw std::runtime_error(
            "Could not open input file."
        );
    }

    char byte;

    while (inputFile.get(byte)) {

        uint8_t value =
            static_cast<uint8_t>(
                static_cast<unsigned char>(byte)
            );

        frequencies[value]++;
        totalBytes++;
    }
}

const std::array<uint64_t, 256>&
FrequencyTable::getFrequencies() const {
    return frequencies;
}

uint64_t FrequencyTable::getTotalBytes() const {
    return totalBytes;
}