#pragma once

#include <omp.h>
#include <stdexcept>
#include <string>
#include <cstring>
#include <iostream>

class Settings {
public:
    Settings(int argc, char **argv) {
        for (int i = 0; i < argc; i++) {
            if (!strcmp(argv[i], "-n")) {
                this->threadCount = std::stoi(argv[i+1]);
                i++;
            }
            else if (!strcmp(argv[i], "-i")) {
                this->path = argv[i+1];
            } 
            else if (!strcmp(argv[i], "-d")) {decode = true;}
            else if (!strcmp(argv[i], "--debug")) {debug = true;}
        }
    }

    void dump() {
        std::cout << "threadCount: " << threadCount << std::endl <<
            "Path: " << path << std::endl;
    }

    void check() {
        if (threadCount <= 0) throw std::runtime_error("Wrong -n config, usage: " + usage);
        if (path == "") throw std::runtime_error("Wrong path, usage:" + usage);
    }

    int threadCount {omp_get_num_procs()};
    const std::string usage = "./huffman [-n <num_of_threads>] -i <infile>";
    std::string path {""};
    bool decode{false};
    bool debug{false};
};

