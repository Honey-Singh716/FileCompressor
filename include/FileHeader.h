
#ifndef FILE_HEADER_H
#define FILE_HEADER_H

#include <array>
#include <cstdint>
#include <fstream>

class FileHeader {
public:
    inline static constexpr char MAGIC_V1[4] =
        {'H', 'U', 'F', '1'};

    inline static constexpr char MAGIC_V2[4] =
        {'H', 'U', 'F', '2'};

    static void write(
        std::ofstream& outputFile,
        uint64_t originalSize,
        const std::array<uint64_t, 256>& frequencies
    );

    static void writeV2(
        std::ofstream& outputFile,
        uint8_t mode,
        uint64_t originalSize,
        const std::array<uint64_t, 256>& frequencies
    );

    static void read(
        std::ifstream& inputFile,
        uint64_t& originalSize,
        std::array<uint64_t, 256>& frequencies
    );

    static uint8_t readMode(
        std::ifstream& inputFile,
        uint64_t& originalSize,
        std::array<uint64_t, 256>& frequencies
    );

private:
    static void writeUint64(
        std::ofstream& outputFile,
        uint64_t value
    );

    static uint64_t readUint64(
        std::ifstream& inputFile
    );

    static void readFrequencies(
        std::ifstream& inputFile,
        uint64_t originalSize,
        std::array<uint64_t, 256>& frequencies
    );
};

#endif
