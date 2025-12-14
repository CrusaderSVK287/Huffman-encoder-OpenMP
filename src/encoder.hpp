#pragma once

#include <vector>
#include <stdint.h>
#include "huffman.hpp"

class Encoder {
public:
    Encoder() {};
    void encode(HuffmanTree &t);
    void dumpMappings();
    std::unordered_map<char, std::vector<bool>> getMapping() {return m_mapping;}
    void encodeBuffer(const std::vector<char>&, std::vector<uint8_t> &outBuffer);
    void _dumpBuffer(std::vector<uint8_t> &buffer);

private:

    void generateHuffmanCodes(Node *root, std::vector<bool> code);

    std::unordered_map<char, std::vector<bool>> m_mapping;
};

