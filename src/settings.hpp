#pragma once

#include <omp.h>
#include <stdexcept>
#include <string>
#include <cstring>
#include <iostream>

class Settings {
public:
    Settings(int argc, char **argv);

    void dump() {
        std::cout << "threadCount: " << threadCount << std::endl <<
            "Path: " << path << std::endl;
    }

    void check();

    int threadCount {omp_get_num_procs()};
    void usage() {
        std::cout << "./huffman [-n <num_of_threads>] [-d] <path>\n"
        "\t-n <num_of_threads> - specify the number of threads to be used\n"
        "\t-d or --decode - decode (decompress) a .huff file\n";
    }
    std::string path {""};
    bool decode{false};
    bool debug{false};
};

