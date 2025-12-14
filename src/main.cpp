#include <exception>
#include <omp.h>
#include <iostream>
#include "huffman.hpp"

int main(int argc, char **argv) {
    Settings settings(argc, argv);

    try {
        settings.check();
        if (!settings.parallel) {
            omp_set_num_threads(1);
        }
        huffmanEncode(settings);
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

