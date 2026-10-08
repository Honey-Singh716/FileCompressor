#include "../include/Encoder.h"

#include <fstream>
#include <stdexcept>

void Encoder::encodeFile(
    const std::string& inputFileName,
    BitWriter& writer,
    const std::unordered_map<
        uint8_t,
        std::string
    >& codes
) {

    std::ifstream inputFile(
        inputFileName,
        std::ios::binary
    );

    if (!inputFile) {

        throw std::runtime_error(
            "Could not open input file."
        );
    }

    char byte;

    while (inputFile.get(byte)) {

        uint8_t value =
            static_cast<uint8_t>(
                static_cast<unsigned char>(byte)
            );

        auto it = codes.find(value);

        if (it == codes.end()) {

            throw std::runtime_error(
                "Huffman code not found for byte."
            );
        }

        const std::string& code =
            it->second;

        for (char bit : code) {

            writer.writeBit(
                bit == '1'
            );
        }
    }
}