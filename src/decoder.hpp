#pragma once

#include <unordered_map>

#include "huffman.hpp"

class Decoder {
    public:
        Decoder(std::unordered_map<char, std::vector<bool>> map):m_mapping(map) {}  

        void decodeAndWrite(std::ifstream &inFile, HuffmanTree &tree, std::string path);
        void dumpMappings();

    private:

        std::unordered_map<char, std::vector<bool>> m_mapping;
};

