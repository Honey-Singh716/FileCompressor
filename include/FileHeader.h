#ifndef FILE_HEADER_H
#define FILE_HEADER_H

#include <array>
#include <cstdint>
#include <fstream>

class FileHeader {

public:

    inline static constexpr char MAGIC[4] =
        {'H', 'U', 'F', '1'};

    static void write(
        std::ofstream& outputFile,
        uint64_t originalSize,
        const std::array<
            uint64_t,
            256
        >& frequencies
    );

private:

    static void writeUint64(
        std::ofstream& outputFile,
        uint64_t value
    );
};

#endif