#include "decoder.hpp"
#include "huffman.hpp"
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <fstream>

std::string processPath(const std::string& path) {
    std::string result = path;

    const std::string huffSuffix = ".huff";

    // Remove ".huff" suffix if present
    if (result.size() >= huffSuffix.size() &&
        result.compare(result.size() - huffSuffix.size(), huffSuffix.size(), huffSuffix) == 0) 
    {
        result.erase(result.size() - huffSuffix.size());
    }

    // Check if the file exists
    if (std::filesystem::exists(result)) {
        result += ".decompressed";
    }

    return result;
}

void Decoder::decodeAndWrite(std::ifstream &inFile, HuffmanTree &tree, std::string path)
{
    path = processPath(path);
    std::ofstream outFile(path, std::ios::binary);

    uint8_t byte;

    Node *current = tree.getRootNode();

    while(inFile.read(reinterpret_cast<char*>(&byte), sizeof(byte))) {
        for(int i = 0; i < 8; i++) {
            bool bit = byte & (1 << (7 - i)); // MSB first

            // 1 to the right, 0 to the left
            if (bit) {
                if (current->rnode) current = current->rnode;
            } else {
                if (current->lnode) current = current->lnode;
            }

            // we reached a leaf node
            if (current->isLeaf) {
                outFile.write(&current->character, 1);
                current = tree.getRootNode(); // reset the root node
                continue; // we still have the rest of the byte to go
            }
        }
    }
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
