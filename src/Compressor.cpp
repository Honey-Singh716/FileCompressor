
#include "../include/Compressor.h"
#include "../include/FrequencyTable.h"
#include "../include/HuffmanTree.h"
#include "../include/FileHeader.h"
#include "../include/BitWriter.h"
#include "../include/Encoder.h"

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <array>

namespace fs = std::filesystem;

static void copyFileData(
    const std::string& source,
    std::ofstream& destination
) {
    std::ifstream input(source, std::ios::binary);

    if (!input) {
        throw std::runtime_error("Could not open source file.");
    }

    char buffer[65536];

    while (input) {
        input.read(buffer, sizeof(buffer));
        std::streamsize count = input.gcount();

        if (count > 0) {
            destination.write(buffer, count);
        }
    }

    if (!input.eof() || !destination) {
        throw std::runtime_error("File copy failed.");
    }
}

void Compressor::compress(
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

    FrequencyTable table;
    table.buildFromFile(inputFileName);

    HuffmanTree tree;
    tree.build(table.getFrequencies());

    // Build a temporary Huffman-format file.
    std::string temporaryName = outputFileName + ".tmp";

    if (fs::absolute(temporaryName).lexically_normal() == inputPath) {
        throw std::runtime_error("Unsafe temporary file path.");
    }

    fs::remove(temporaryName);

    try {
        {
            std::ofstream temporary(
                temporaryName,
                std::ios::binary
            );

            if (!temporary) {
                throw std::runtime_error(
                    "Could not create temporary file."
                );
            }

            FileHeader::writeV2(
                temporary,
                1,
                table.getTotalBytes(),
                table.getFrequencies()
            );

            BitWriter writer(temporary);

            Encoder::encodeFile(
                inputFileName,
                writer,
                tree.getCodes()
            );

            writer.flush();
        }

        uintmax_t huffmanSize = fs::file_size(temporaryName);

        // HUF2 header: 4-byte magic + 1-byte mode
        // + 8-byte original size + 2048-byte frequencies.
        constexpr uintmax_t HEADER_SIZE = 2061;

        uintmax_t rawSize =
            HEADER_SIZE + table.getTotalBytes();

        // Use Huffman only if the complete file is smaller.
        if (huffmanSize < rawSize) {
            fs::remove(outputFileName);
            fs::rename(temporaryName, outputFileName);
        }
        else {
            std::ofstream output(
                outputFileName,
                std::ios::binary
            );

            if (!output) {
                throw std::runtime_error(
                    "Could not create output file."
                );
            }

            std::array<uint64_t, 256> emptyFrequencies{};

            FileHeader::writeV2(
                output,
                0,
                table.getTotalBytes(),
                emptyFrequencies
            );

            copyFileData(inputFileName, output);

            output.close();

            if (!output) {
                throw std::runtime_error(
                    "Failed to finish raw output."
                );
            }

            fs::remove(temporaryName);
        }
    }
    catch (...) {
        std::error_code error;
        fs::remove(temporaryName, error);
        throw;
    }
}
