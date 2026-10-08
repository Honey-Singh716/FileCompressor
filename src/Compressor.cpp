#include "../include/Compressor.h"

#include "../include/FrequencyTable.h"
#include "../include/HuffmanTree.h"
#include "../include/FileHeader.h"
#include "../include/BitWriter.h"
#include "../include/Encoder.h"

#include <fstream>
#include <stdexcept>

void Compressor::compress(
    const std::string& inputFileName,
    const std::string& outputFileName
) {

    // Basic protection
    if (inputFileName == outputFileName) {

        throw std::runtime_error(
            "Input and output files must be different."
        );
    }

    // ---------------------------------
    // STEP 1: Build frequency table
    // ---------------------------------

    FrequencyTable frequencyTable;

    frequencyTable.buildFromFile(
        inputFileName
    );

    // ---------------------------------
    // STEP 2: Build Huffman tree
    // ---------------------------------

    HuffmanTree huffmanTree;

    huffmanTree.build(
        frequencyTable.getFrequencies()
    );

    // ---------------------------------
    // STEP 3: Open output file
    // ---------------------------------

    std::ofstream outputFile(
        outputFileName,
        std::ios::binary
    );

    if (!outputFile) {

        throw std::runtime_error(
            "Could not create output file."
        );
    }

    // ---------------------------------
    // STEP 4: Write header
    // ---------------------------------

    FileHeader::write(
        outputFile,
        frequencyTable.getTotalBytes(),
        frequencyTable.getFrequencies()
    );

    // ---------------------------------
    // STEP 5: Create BitWriter
    // ---------------------------------

    BitWriter writer(outputFile);

    // ---------------------------------
    // STEP 6: Encode original file
    // ---------------------------------

    Encoder::encodeFile(
        inputFileName,
        writer,
        huffmanTree.getCodes()
    );

    // ---------------------------------
    // STEP 7: Flush remaining bits
    // ---------------------------------

    writer.flush();

    outputFile.close();
}