#include "huffman.hpp"
#include <fstream>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <list>

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

    // Create huffman tree, create a mapping
    HuffmanTree tree;
    tree.create(frequencyMap);
    tree.dumpTree();

    Encoder encoder;
    encoder.encode(tree);
    encoder._dumpMappings();
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
    // create a list of nodes and transform all pairs from map to nodes
    ;

    for (auto &pair : map) {
        Node node;
        node.isLeaf = true; // all leters are leafs
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
           root = new Node(std::move(node1)); // Store root properly
           break;
       }

       Node node2 = std::move(l.back());
       l.pop_back();

       Node newNode;
       newNode.isLeaf = false;
       newNode.frequency = node1.frequency + node2.frequency;
       newNode.lnode = new Node(std::move(node1)); // Allocate new nodes
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

void Encoder::_dumpMappings() 
{
    for (const auto& pair : m_mapping) {
        std::cout << pair.first << " = ";
        for (bool bit : pair.second) {
            std::cout << bit;
        }
        std::cout << std::endl;
    }
}
