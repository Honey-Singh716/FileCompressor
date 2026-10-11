
#include "../include/Decompressor.h"
#include "../include/FileHeader.h"
#include "../include/HuffmanTree.h"
#include "../include/BitReader.h"
#include "../include/Decoder.h"

#include <array>
#include <filesystem>
#include <fstream>
#include <stdexcept>

namespace fs = std::filesystem;

void Decompressor::decompress(
    const std::string& inputFileName,
    const std::string& outputFileName
) {
    fs::path inputPath =
        fs::absolute(inputFileName).lexically_normal();

    fs::path outputPath =
        fs::absolute(outputFileName).lexically_normal();

    if (inputPath == outputPath) {
        throw std::runtime_error(
            "Input and output files must be different."
        );
    }

    std::ifstream input(
        inputFileName,
        std::ios::binary
    );

    if (!input) {
        throw std::runtime_error(
            "Could not open compressed file."
        );
    }

    uint64_t originalSize = 0;
    std::array<uint64_t, 256> frequencies{};

    uint8_t mode = FileHeader::readMode(
        input,
        originalSize,
        frequencies
    );

    // Reject obviously truncated raw streams before creating output.
    if (mode == 0) {
        auto currentPosition = input.tellg();
        input.seekg(0, std::ios::end);
        auto endPosition = input.tellg();

        if (currentPosition < 0 || endPosition < currentPosition ||
            static_cast<uint64_t>(endPosition - currentPosition)
                < originalSize) {
            throw std::runtime_error(
                "Raw data is incomplete."
            );
        }

        input.seekg(currentPosition);
    }

    std::ofstream output(
        outputFileName,
        std::ios::binary
    );

    if (!output) {
        throw std::runtime_error(
            "Could not create restored file."
        );
    }

    
    if (mode == 0) {
        char buffer[65536];
        uint64_t remaining = originalSize;

        while (remaining > 0) {
            std::streamsize wanted =
                static_cast<std::streamsize>(
                    remaining < sizeof(buffer)
                        ? remaining
                        : sizeof(buffer)
                );

            input.read(buffer, wanted);

            std::streamsize count = input.gcount();

            if (count != wanted) {
                throw std::runtime_error(
                    "Raw data ended unexpectedly."
                );
            }

            output.write(buffer, count);

            if (!output) {
                throw std::runtime_error(
                    "Failed to restore raw data."
                );
            }

            remaining -= static_cast<uint64_t>(count);
        }
    }

    else {
        HuffmanTree tree;
        tree.build(frequencies);

        BitReader reader(input);

        Decoder::decodeFile(
            reader,
            tree.getRoot(),
            originalSize,
            output
        );
    }

    output.close();

    if (!output) {
        throw std::runtime_error(
            "Failed to finish restored file."
        );
    }
}
