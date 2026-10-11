#include "../include/Decompressor.h"

#include "../include/FileHeader.h"
#include "../include/HuffmanTree.h"
#include "../include/BitReader.h"
#include "../include/Decoder.h"

#include <array>
#include <fstream>
#include <stdexcept>

void Decompressor::decompress(
    const std::string& inputFileName,
    const std::string& outputFileName
) {
    if (inputFileName == outputFileName) {
        throw std::runtime_error(
            "Input and output files must be different."
        );
    }

    // Step 1: Open compressed file
    std::ifstream inputFile(
        inputFileName,
        std::ios::binary
    );

    if (!inputFile) {
        throw std::runtime_error(
            "Could not open compressed file."
        );
    }

    // Step 2: Read metadata
    uint64_t originalSize = 0;
    std::array<uint64_t, 256> frequencies{};

    FileHeader::read(
        inputFile,
        originalSize,
        frequencies
    );

    // Step 3: Rebuild the same Huffman tree
    HuffmanTree tree;
    tree.build(frequencies);

    // Step 4: Create restored output file
    std::ofstream outputFile(
        outputFileName,
        std::ios::binary
    );

    if (!outputFile) {
        throw std::runtime_error(
            "Could not create restored file."
        );
    }

    // Step 5: Read bits after the header
    BitReader reader(inputFile);

    // Step 6: Decode original bytes
    Decoder::decodeFile(
        reader,
        tree.getRoot(),
        originalSize,
        outputFile
    );

    outputFile.close();

    if (!outputFile) {
        throw std::runtime_error(
            "Failed to finish writing restored file."
        );
    }
}