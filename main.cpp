#include <iostream>
#include <string>

#include "include/Compressor.h"

int main(
    int argc,
    char* argv[]
) {

    if (argc != 4 ||
        std::string(argv[1]) != "compress") {

        std::cerr
            << "Usage: compressor compress <input> <output>\n";

        return 1;
    }

    try {

        Compressor::compress(
            argv[2],
            argv[3]
        );

        std::cout
            << "Compression successful.\n";

    }
    catch (const std::exception& e) {

        std::cerr
            << "Error: "
            << e.what()
            << '\n';

        return 1;
    }

    return 0;
}