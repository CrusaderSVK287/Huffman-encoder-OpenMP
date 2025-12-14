#include "huffman.hpp"
#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <list>
#include <omp.h>

#include "encoder.hpp"

void writeHuffmanTable(std::ofstream& outFile, 
                       const std::unordered_map<char, std::vector<bool>>& huffmanMap) {
    uint8_t mapSize = huffmanMap.size();  // Store the number of entries
    outFile.write(reinterpret_cast<const char*>(&mapSize), sizeof(mapSize));

    for (const auto& pair : huffmanMap) {
        outFile.put(pair.first); // Write character

        uint8_t bitLength = pair.second.size(); // Length of the encoding
        outFile.put(bitLength); // Store bit length

        uint8_t byte = 0;
        for (size_t i = 0; i < bitLength; i++) {
            if (pair.second[i]) {
                byte |= (1 << (7 - (i % 8))); // Set bit in byte
            }

            if (i % 8 == 7 || i == bitLength - 1) { 
                outFile.put(byte); // Write full byte or last byte
                byte = 0; // Reset for next batch
            }
        }
    }
}

void writeToFile(const std::string& filename, const std::vector<uint8_t>& outBuffer, Encoder &encoder) {
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    writeHuffmanTable(outFile, encoder.getMapping()); 

    std::string endStr = "\n__ENDTABLE__\n";
    outFile.write(endStr.data(), endStr.size());

    outFile.write(reinterpret_cast<const char*>(outBuffer.data()), outBuffer.size());
    outFile.close();
}

void _dumpFrequencyMap(std::unordered_map<char, int> &map)
{
    for (const auto& pair: map) {
        std::cout << pair.first << " " << pair.second << std::endl;
    }
}

void sortFrequencyMap(std::unordered_map<char, int> &map)
{
    std::vector<std::pair<char, int>> sortedVector(map.begin(), map.end());

    std::sort(sortedVector.begin(), sortedVector.end(), [](const auto& a, const auto& b) {
            return (a.second == b.second) ? a.first < b.first : a.second < b.second;
            });

    map.clear();
    for(const auto& pair : sortedVector) {
        map[pair.first] = pair.second;
    }
}

void countFrequencies(std::vector<char> &buffer, std::unordered_map<char,int> &map) 
{
    #pragma omp parallel
    {   
        std::unordered_map<char, int> localMap;

        #pragma omp for
        for (size_t i = 0; i < buffer.size(); i++) {
            localMap[buffer[i]]++;
        }

    #pragma omp critical
        {
            for (const auto& pair : localMap) {
                map[pair.first] += pair.second;
            }
        }
    }
}

void huffmanEncode(Settings &settings) 
{
    // Read the file, create frequency map 
    std::ifstream infile(settings.path, std::ios::binary | std::ios::ate);
    std::streamsize size = infile.tellg();
    infile.seekg(0, std::ios::beg);

    std::vector<char> inBuffer(size);
    if (!infile.read(inBuffer.data(), size)) throw std::runtime_error("Failed to open file " + settings.path);

    std::unordered_map<char, int> frequencyMap;   

    countFrequencies(inBuffer, frequencyMap);
    sortFrequencyMap(frequencyMap);
    if (settings.debug) _dumpFrequencyMap(frequencyMap);

    // Create huffman tree, create a mapping
    HuffmanTree tree;
    tree.create(frequencyMap);
    if (settings.debug) tree.dumpTree();

    Encoder encoder;
    encoder.encode(tree);
    if(settings.debug) encoder.dumpMappings();
    
    std::vector<uint8_t> outBuffer;
    encoder.encodeBuffer(inBuffer, outBuffer);
    if(settings.debug) encoder._dumpBuffer(outBuffer);
    writeToFile(settings.path + ".huff", outBuffer, encoder);
}

void HuffmanTree::InsertNodeToList(std::list<Node> &l, Node n)
{
    std::list<Node>::iterator it = l.end();
    for(auto &elem : l) {
        if (elem.frequency > n.frequency)
            break;
        it--;
    }

    l.insert(it , std::move(n));
}

void HuffmanTree::create(std::unordered_map<char, int> &map)
{
    for (auto &pair : map) {
        Node node;
        node.isLeaf = true; // all leters are leaves
        node.character = pair.first;
        node.frequency = pair.second;
        l.emplace_back(node);
    }

    Node *rootNode = nullptr;

    // create the huffman tree 
    while (!l.empty()) {
       Node node1 = std::move(l.back());
       l.pop_back();

       if (l.empty()) {
           root = new Node(std::move(node1));
           break;
       }

       Node node2 = std::move(l.back());
       l.pop_back();

       Node newNode;
       newNode.isLeaf = false;
       newNode.frequency = node1.frequency + node2.frequency;
       newNode.lnode = new Node(std::move(node1));
       newNode.rnode = new Node(std::move(node2));

       InsertNodeToList(l, std::move(newNode));
    }
}

void HuffmanTree::_dumpTree(Node *n, int depth, std::string prefix)
{
    if (!n) return; // Base case
    
    std::string indent(depth * 8, ' '); // Indentation based on depth
    
    if (!n->isLeaf) {
        std::cout << indent << prefix << "[FREQ: " << n->frequency << "]" << std::endl;
        _dumpTree(n->lnode, depth + 1, "L─ ");
        _dumpTree(n->rnode, depth + 1, "R─ ");
    } else {
        std::cout << indent << prefix << "[CHAR: " << n->character << " | FREQ: " << n->frequency << "]" << std::endl;
    }
}


