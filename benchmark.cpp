#include "include/Compressor.h"
#include "include/Decompressor.h"

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

namespace fs = std::filesystem;
using Clock = std::chrono::steady_clock;

bool filesAreEqual(
    const std::string& firstFile,
    const std::string& secondFile
) {
    std::ifstream first(firstFile, std::ios::binary);
    std::ifstream second(secondFile, std::ios::binary);

    if (!first || !second) {
        return false;
    }

    char a, b;

    while (true) {
        bool hasA = static_cast<bool>(first.get(a));
        bool hasB = static_cast<bool>(second.get(b));

        if (hasA != hasB) {
            return false;
        }

        if (!hasA) {
            return true;
        }

        if (a != b) {
            return false;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr
            << "Usage: benchmark <input> <compressed> <restored>\n";
        return 1;
    }

    const std::string input = argv[1];
    const std::string compressed = argv[2];
    const std::string restored = argv[3];

    try {
        // Avoid accidentally overwriting the original input.
        fs::path inputPath = fs::absolute(input).lexically_normal();
        fs::path compressedPath =
            fs::absolute(compressed).lexically_normal();
        fs::path restoredPath =
            fs::absolute(restored).lexically_normal();

        if (inputPath == compressedPath ||
            inputPath == restoredPath ||
            compressedPath == restoredPath) {
            throw std::runtime_error(
                "Input, compressed and restored paths must be different."
            );
        }

        // Measure compression time.
        auto compressionStart = Clock::now();

        Compressor::compress(input, compressed);

        auto compressionEnd = Clock::now();

        // Measure decompression time.
        auto decompressionStart = Clock::now();

        Decompressor::decompress(compressed, restored);

        auto decompressionEnd = Clock::now();

        // Validate lossless reconstruction.
        if (!filesAreEqual(input, restored)) {
            throw std::runtime_error(
                "Verification failed: restored file differs from input."
            );
        }

        uintmax_t originalSize = fs::file_size(input);
        uintmax_t compressedSize = fs::file_size(compressed);

        double compressionMs =
            std::chrono::duration<double, std::milli>(
                compressionEnd - compressionStart
            ).count();

        double decompressionMs =
            std::chrono::duration<double, std::milli>(
                decompressionEnd - decompressionStart
            ).count();

        std::cout << std::fixed << std::setprecision(2);

        std::cout << "\n===== HUFFMAN BENCHMARK =====\n";

        std::cout << "Original size      : "
                  << originalSize << " bytes\n";

        std::cout << "Compressed size    : "
                  << compressedSize << " bytes\n";

        if (originalSize > 0) {
            double ratio =
                100.0 * compressedSize / originalSize;

            double saving =
                100.0 * (1.0 -
                    static_cast<double>(compressedSize) /
                    static_cast<double>(originalSize));

            std::cout << "Compression ratio  : "
                      << ratio << "%\n";

            std::cout << "Space saving       : "
                      << saving << "%\n";
        } else {
            std::cout << "Compression ratio  : N/A (empty file)\n";
            std::cout << "Space saving       : N/A (empty file)\n";
        }

        std::cout << "Compression time   : "
                  << compressionMs << " ms\n";

        std::cout << "Decompression time : "
                  << decompressionMs << " ms\n";

        std::cout << "Round-trip check   : PASS\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Benchmark failed: "
                  << e.what() << '\n';
        return 1;
    }

    return 0;
}