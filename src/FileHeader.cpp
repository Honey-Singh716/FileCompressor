#include "../include/FileHeader.h"

void FileHeader::writeUint64(
    std::ofstream& outputFile,
    uint64_t value
) {

    outputFile.write(
        reinterpret_cast<const char*>(&value),
        sizeof(value)
    );
}

void FileHeader::write(
    std::ofstream& outputFile,
    uint64_t originalSize,
    const std::array<
        uint64_t,
        256
    >& frequencies
) {

    // Magic number
    outputFile.write(
        MAGIC,
        4
    );

    // Original file size
    writeUint64(
        outputFile,
        originalSize
    );

    // Frequency table
    for (uint64_t frequency : frequencies) {

        writeUint64(
            outputFile,
            frequency
        );
    }
}