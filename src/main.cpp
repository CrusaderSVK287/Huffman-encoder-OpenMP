#include <chrono>
#include <ctime>
#include <exception>
#include <omp.h>
#include <iostream>
#include "huffman.hpp"

int main(int argc, char **argv) {
    Settings settings(argc, argv);

    try {
        settings.check();
        
        
        if (settings.sequntial) {
            omp_set_num_threads(1);
            
            auto start = std::chrono::system_clock::now();
            huffmanEncode(settings, false);
            auto end   = std::chrono::system_clock::now();
            
            std::chrono::duration<double> elapsed_seconds = end-start;
            std::cout << "Sequential time to finish: " << elapsed_seconds.count() << "s" << std::endl;
        }
        if (settings.parallel) {
            omp_set_num_threads(settings.threadCount);
            
            auto start = std::chrono::system_clock::now();
            huffmanEncode(settings, true);
            auto end   = std::chrono::system_clock::now();

            std::chrono::duration<double> elapsed_seconds = end-start;
            std::cout << "Parallel time to finish:   " << elapsed_seconds.count() << "s" << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

