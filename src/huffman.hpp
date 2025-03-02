#pragma once

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
            }
            else if (i == argc -1 ) {
                this->path = argv[i];
            }
        }
    }

    void dump() {
        std::cout << "threadCount: " << threadCount << std::endl;
    }

    void check() {
        if (threadCount <= 0) throw std::runtime_error("Wrong -n config, usage: " + usage);
        if (path == "") throw std::runtime_error("Wrong path, usage:" + usage);
    }

    int threadCount {-1};
    const std::string usage = "./huffman -n <num_of_threads> <infile>";
    std::string path {""};
};

void huffmanEncode(Settings &);
