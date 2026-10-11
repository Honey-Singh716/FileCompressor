#include "../include/FileHeader.h"

#include <stdexcept>

void FileHeader::writeUint64(
    std::ofstream& outputFile,
    uint64_t value
) {
    outputFile.write(
        reinterpret_cast<const char*>(&value),
        sizeof(value)
    );

    if (!outputFile) {
        throw std::runtime_error(
            "Failed to write file header."
        );
    }
}

uint64_t FileHeader::readUint64(
    std::ifstream& inputFile
) {
    uint64_t value = 0;

    inputFile.read(
        reinterpret_cast<char*>(&value),
        sizeof(value)
    );

    if (!inputFile) {
        throw std::runtime_error(
            "Incomplete or corrupted file header."
        );
    }

    return value;
}

void FileHeader::write(
    std::ofstream& outputFile,
    uint64_t originalSize,
    const std::array<uint64_t, 256>& frequencies
) {
    outputFile.write(MAGIC, 4);

    writeUint64(outputFile, originalSize);

    for (uint64_t frequency : frequencies) {
        writeUint64(outputFile, frequency);
    }

    if (!outputFile) {
        throw std::runtime_error(
            "Failed to write file header."
        );
    }
}

void FileHeader::read(
    std::ifstream& inputFile,
    uint64_t& originalSize,
    std::array<uint64_t, 256>& frequencies
) {
    char magic[4];

    inputFile.read(magic, 4);

    if (!inputFile) {
        throw std::runtime_error(
            "File is too small to contain a header."
        );
    }

    for (int i = 0; i < 4; i++) {
        if (magic[i] != MAGIC[i]) {
            throw std::runtime_error(
                "Invalid compressed file format."
            );
        }
    }

    originalSize = readUint64(inputFile);

    uint64_t totalFrequency = 0;

    for (int i = 0; i < 256; i++) {
        frequencies[i] = readUint64(inputFile);

        if (frequencies[i] > originalSize - totalFrequency) {
            throw std::runtime_error(
                "Invalid frequency table."
            );
        }

        totalFrequency += frequencies[i];
    }

    if (totalFrequency != originalSize) {
        throw std::runtime_error(
            "Header size and frequencies do not match."
        );
    }
}