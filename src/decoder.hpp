#pragma once

#include <unordered_map>
#include <stdbool.h>
#include <vector>
class Decoder {
    public:
        Decoder(std::unordered_map<char, std::vector<bool>> map):m_mapping(map) {}  

        void BuildHuffmanTree();
        void dumpMappings();

    private:

        std::unordered_map<char, std::vector<bool>> m_mapping;
};

