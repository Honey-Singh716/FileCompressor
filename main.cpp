#include <iostream>
#include <string>
#include <exception>

#include "include/Compressor.h"
#include "include/Decompressor.h"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr
            << "Usage:\n"
            << "  compressor compress <input> <output>\n"
            << "  compressor decompress <input> <output>\n";

        return 1;
    }

    try {
        std::string operation = argv[1];

        if (operation == "compress") {
            Compressor::compress(
                argv[2],
                argv[3]
            );

            std::cout
                << "Compression successful.\n";
        }
        else if (operation == "decompress") {
            Decompressor::decompress(
                argv[2],
                argv[3]
            );

            std::cout
                << "Decompression successful.\n";
        }
        else {
            std::cerr
                << "Unknown operation: "
                << operation << '\n';

            return 1;
        }
    }
    catch (const std::exception& e) {
        std::cerr
            << "Error: "
            << e.what() << '\n';

        return 1;
    }

    return 0;
}