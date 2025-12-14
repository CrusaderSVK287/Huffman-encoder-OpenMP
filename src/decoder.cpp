#include "decoder.hpp"
#include <iostream>

void BuildHuffmanTree(std::unordered_map<char, std::vector<bool>>)
{

}

void Decoder::dumpMappings() 
{
    for (const auto& pair : m_mapping) {
        std::cout << pair.first << " = ";
        for (bool bit : pair.second) {
            std::cout << bit;
        }
        std::cout << std::endl;
    }
}
