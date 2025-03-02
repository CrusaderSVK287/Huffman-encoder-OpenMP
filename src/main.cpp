#include <exception>
#include <omp.h>
#include <iostream>
#include "huffman.hpp"

int main(int argc, char **argv) {

#ifdef _SERIAL
    std::cout << "Warning, running in serial mode" << std::endl;
#endif 

    Settings settings(argc, argv);

    try {
        settings.check();
        
        omp_set_num_threads(settings.threadCount);
        
        huffmanEncode(settings);
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

