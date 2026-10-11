#include "../include/Decoder.h"

#include <stdexcept>

void Decoder::decodeFile(
    BitReader& reader,
    HuffmanNode* root,
    uint64_t originalSize,
    std::ofstream& outputFile
) {
    if (originalSize == 0) {
        return;
    }

    if (root == nullptr) {
        throw std::runtime_error(
            "Invalid Huffman tree."
        );
    }

    // Special case: only one unique byte
    if (root->isLeaf()) {
        for (uint64_t i = 0; i < originalSize; i++) {
            bool bit;

            if (!reader.readBit(bit) || bit != false) {
                throw std::runtime_error(
                    "Invalid single-symbol bitstream."
                );
            }

            outputFile.put(
                static_cast<char>(root->byte)
            );
        }

        if (!outputFile) {
            throw std::runtime_error(
                "Failed to write restored file."
            );
        }

        return;
    }

    // Decode normal Huffman tree
    for (uint64_t i = 0; i < originalSize; i++) {
        HuffmanNode* current = root;

        while (!current->isLeaf()) {
            bool bit;

            if (!reader.readBit(bit)) {
                throw std::runtime_error(
                    "Compressed data ended unexpectedly."
                );
            }

            current = bit
                ? current->right
                : current->left;

            if (current == nullptr) {
                throw std::runtime_error(
                    "Invalid Huffman bitstream."
                );
            }
        }

        outputFile.put(
            static_cast<char>(current->byte)
        );

        if (!outputFile) {
            throw std::runtime_error(
                "Failed to write restored file."
            );
        }
    }
}