
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
        throw std::runtime_error("Failed to write header.");
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
        throw std::runtime_error("Incomplete file header.");
    }

    return value;
}

void FileHeader::readFrequencies(
    std::ifstream& inputFile,
    uint64_t originalSize,
    std::array<uint64_t, 256>& frequencies
) {
    uint64_t total = 0;

    for (int i = 0; i < 256; i++) {
        frequencies[i] = readUint64(inputFile);

        if (frequencies[i] > originalSize - total) {
            throw std::runtime_error("Invalid frequency table.");
        }

        total += frequencies[i];
    }

    if (total != originalSize) {
        throw std::runtime_error(
            "Frequency total does not match original size."
        );
    }
}

void FileHeader::write(
    std::ofstream& outputFile,
    uint64_t originalSize,
    const std::array<uint64_t, 256>& frequencies
) {
    outputFile.write(MAGIC_V1, 4);
    writeUint64(outputFile, originalSize);

    for (uint64_t frequency : frequencies) {
        writeUint64(outputFile, frequency);
    }

    if (!outputFile) {
        throw std::runtime_error("Failed to write header.");
    }
}

void FileHeader::writeV2(
    std::ofstream& outputFile,
    uint8_t mode,
    uint64_t originalSize,
    const std::array<uint64_t, 256>& frequencies
) {
    if (mode > 1) {
        throw std::runtime_error("Invalid compression mode.");
    }

    outputFile.write(MAGIC_V2, 4);
    outputFile.put(static_cast<char>(mode));
    writeUint64(outputFile, originalSize);

    for (uint64_t frequency : frequencies) {
        writeUint64(outputFile, frequency);
    }

    if (!outputFile) {
        throw std::runtime_error("Failed to write header.");
    }
}

uint8_t FileHeader::readMode(
    std::ifstream& inputFile,
    uint64_t& originalSize,
    std::array<uint64_t, 256>& frequencies
) {
    char magic[4];
    inputFile.read(magic, 4);

    if (!inputFile) {
        throw std::runtime_error("File is too small.");
    }

    uint8_t mode;

    if (magic[0] == 'H' &&
        magic[1] == 'U' &&
        magic[2] == 'F' &&
        magic[3] == '1') {
        // Legacy format: no mode byte, Huffman mode assumed.
        mode = 1;
        originalSize = readUint64(inputFile);
    }
    else if (magic[0] == 'H' &&
             magic[1] == 'U' &&
             magic[2] == 'F' &&
             magic[3] == '2') {
        char modeByte;

        if (!inputFile.get(modeByte)) {
            throw std::runtime_error("Missing mode flag.");
        }

        mode = static_cast<uint8_t>(
            static_cast<unsigned char>(modeByte)
        );

        if (mode > 1) {
            throw std::runtime_error("Invalid compression mode.");
        }

        originalSize = readUint64(inputFile);
    }
    else {
        throw std::runtime_error("Invalid compressed file format.");
    }

    if (mode == 1) {
        readFrequencies(inputFile, originalSize, frequencies);
    }
    else {
        frequencies.fill(0);
    }

    return mode;
}

void FileHeader::read(
    std::ifstream& inputFile,
    uint64_t& originalSize,
    std::array<uint64_t, 256>& frequencies
) {
    uint8_t mode = readMode(
        inputFile,
        originalSize,
        frequencies
    );

    if (mode != 1) {
        throw std::runtime_error(
            "Legacy read() supports Huffman mode only."
        );
    }
}
