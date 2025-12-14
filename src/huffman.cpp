#include "huffman.hpp"
#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <list>
#include <omp.h>

#include "encoder.hpp"
#include "decoder.hpp"
#include "settings.hpp"

void readHuffmanTable(std::ifstream& inFile,
                      std::unordered_map<char, std::vector<bool>>& huffmanMap)
{
    uint8_t count;
    inFile.read(reinterpret_cast<char*>(&count), sizeof(count));

    for (uint8_t i = 0; i < count; ++i) {
        uint8_t symbol;
        uint8_t bitLength;

        inFile.read(reinterpret_cast<char*>(&symbol), 1);
        inFile.read(reinterpret_cast<char*>(&bitLength), 1);


        std::vector<bool> bits;
        bits.reserve(bitLength);

        size_t byteCount = (bitLength + 7) / 8;
        for (size_t b = 0; b < byteCount; ++b) {
            uint8_t byte;
            inFile.read(reinterpret_cast<char*>(&byte), 1);

            for (int k = 0; k < 8 && bits.size() < bitLength; ++k) {
                bits.push_back((byte & (1 << (7 - k))) != 0);
            }
        }

        huffmanMap[symbol] = std::move(bits);
    }
}

void writeHuffmanTable(std::ofstream& outFile,
                       const std::unordered_map<char, std::vector<bool>>& huffmanMap)
{
    uint8_t count = static_cast<uint8_t>(huffmanMap.size());
    outFile.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& [symbol, bits] : huffmanMap) {
        uint8_t bitLength = static_cast<uint8_t>(bits.size());

        outFile.put(symbol);
        outFile.put(bitLength);

        uint8_t byte = 0;
        for (size_t i = 0; i < bitLength; ++i) {
            if (bits[i]) {
                byte |= (1 << (7 - (i % 8)));
            }
            if (i % 8 == 7 || i + 1 == bitLength) {
                outFile.put(byte);
                byte = 0;
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

void HuffmanTree::load(std::unordered_map<char, std::vector<bool>> mappings)
{
    root = new Node();

    for (auto &[value, bits] : mappings) {
        Node* current = root;
        for (bool bit : bits) {
            // 1 to the right, 0 to the left
            if (bit) {
                if (!current->rnode) {current->rnode = new Node();}
                current = current->rnode;
                current->isLeaf = false;
            } else {
                if (!current->lnode) {current->lnode = new Node();}
                current = current->lnode;
                current->isLeaf = false;
            }
        }
        current->isLeaf = true;
        current->character = value;
    }
}

void HuffmanTree::_dumpTree(Node *n, int depth, std::string prefix)
{
    if (!n) return; // Base case
    
    std::string indent(depth * 8, ' '); // Indentation based on depth
    
    if (!n->isLeaf) {
        //std::cout << indent << prefix << "[FREQ: " << n->frequency << "]" << std::endl;
        _dumpTree(n->lnode, depth + 1, "L─ ");
        _dumpTree(n->rnode, depth + 1, "R─ ");
    } else {
        //std::cout << indent << prefix << "[CHAR: " << n->character << " | FREQ: " << n->frequency << "]" << std::endl;
        std::cout << indent << prefix << "[CHAR: " << n->character << "]" << std::endl;
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
    //tree.dumpTree();

    Encoder encoder;
    encoder.encode(tree);
    if(settings.debug) encoder.dumpMappings();
    
    std::vector<uint8_t> outBuffer;
    encoder.encodeBuffer(inBuffer, outBuffer);
    if(settings.debug) encoder._dumpBuffer(outBuffer);
    writeToFile(settings.path + ".huff", outBuffer, encoder);
}

void huffmanDecode(Settings &settings)
{
    // Read the file, create frequency map 
    std::ifstream infile(settings.path, std::ios::binary);
    //std::streamsize size = infile.tellg();
    infile.seekg(0, std::ios::beg);

    std::unordered_map<char, std::vector<bool>> huffmanMap;   
    readHuffmanTable(infile, huffmanMap);

    Decoder decoder(huffmanMap);

    HuffmanTree tree;
    tree.load(huffmanMap);
    //tree.dumpTree();
    
    decoder.decodeAndWrite(infile, tree, settings.path);
}

