#include "encoder.hpp"
#include <omp.h>

void Encoder::encode(HuffmanTree &t)
{
    std::vector<bool> code;
    generateHuffmanCodes(t.getRootNode(), code);
}


void Encoder::generateHuffmanCodes(Node *root, std::vector<bool> code)
{
    if (!root) return;

    // If it's a leaf node, store the code
    if (root->isLeaf) {
        m_mapping[root->character] = code;
        return;
    }

    // Traverse left (append 0)
    code.push_back(0);
    generateHuffmanCodes(root->lnode, code);
    code.pop_back(); // Backtrack

    // Traverse right (append 1)
    code.push_back(1);
    generateHuffmanCodes(root->rnode, code);
}

void Encoder::dumpMappings() 
{
    for (const auto& pair : m_mapping) {
        std::cout << pair.first << " = ";
        for (bool bit : pair.second) {
            std::cout << bit;
        }
        std::cout << std::endl;
    }
}

void Encoder::encodeBuffer(const std::vector<char>& inBuffer, std::vector<uint8_t>& outBuffer)
{
    size_t inSize = inBuffer.size();
    size_t numThreads = omp_get_max_threads();
    // Each thread will work on its local buffer
    std::vector<std::vector<bool>> localBitStreams(numThreads);

    #pragma omp parallel
    {
        int threadId = omp_get_thread_num();
        size_t chunkSize = inSize / numThreads;
        size_t startIdx = threadId * chunkSize;
        size_t endIdx = (threadId == numThreads - 1) ? inSize : (threadId + 1) * chunkSize;

        // Local bitstream for each thread
        std::vector<bool> bitStream;

        // Encode the assigned part of the buffer
        for (size_t i = startIdx; i < endIdx; ++i) {
            char c = inBuffer[i];
            auto it = m_mapping.find(c);
            if (it != m_mapping.end()) {
                bitStream.insert(bitStream.end(), it->second.begin(), it->second.end());
            } else {
                throw std::runtime_error("Error: Character not found in Huffman map.\n");
            }
        }

        localBitStreams[threadId] = std::move(bitStream);
    }

    size_t totalBitCount = 0;
    for (const auto& localStream : localBitStreams) {
        totalBitCount += localStream.size();
    }

    outBuffer.clear();
    outBuffer.resize((totalBitCount + 7) / 8, 0);

    size_t bitPos = 0;
    // Combine the local bitstreams
    for (const auto& localStream : localBitStreams) {
        for (bool bit : localStream) {
            if (bit) {
                outBuffer[bitPos / 8] |= (1 << (7 - (bitPos % 8))); // Pack bits (big-endian)
            }
            bitPos++;
        }
    }
}

void Encoder::_dumpBuffer(std::vector<uint8_t> &buffer)
{
    for (uint8_t byte : buffer) {
        for (int i = 7; i >= 0; --i) {
            std::cout << ((byte >> i) & 1);
        }
    }
    std::cout << std::endl;
}
