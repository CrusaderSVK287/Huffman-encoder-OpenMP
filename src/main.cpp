#include <exception>
#include <omp.h>
#include <iostream>
#include "huffman.hpp"

int main(int argc, char **argv) {
    Settings settings(argc, argv);

    try {
        settings.check();
        if (settings.decode) {
            huffmanDecode(settings);
        } else {
            huffmanEncode(settings);
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

