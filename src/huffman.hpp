#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
#include <cstring>
#include <iostream>
#include <unordered_map>
#include <list>
#include <vector>

class Settings;
class HuffmanTree;
class Node;

class Settings {
public:
    Settings(int argc, char **argv) {
        for (int i = 0; i < argc; i++) {
            if (!strcmp(argv[i], "-n")) {
                this->threadCount = std::stoi(argv[i+1]);
            }
            else if (!strcmp(argv[i], "-i")) {
                this->path = argv[i+1];
            } else if (!strcmp(argv[i], "-p")) {sequntial = false;}
            else if (!strcmp(argv[i], "-s")) {parallel = false;}
        }
    }

    void dump() {
        std::cout << "threadCount: " << threadCount << std::endl <<
            "Path: " << path << std::endl;
    }

    void check() {
        if (threadCount <= 0) throw std::runtime_error("Wrong -n config, usage: " + usage);
        if (path == "") throw std::runtime_error("Wrong path, usage:" + usage);
    }

    int threadCount {-1};
    const std::string usage = "./huffman [-n <num_of_threads>] -i <infile>";
    std::string path {""};
    bool parallel{true};
    bool sequntial{true};
};

class HuffmanTree {
public:
    HuffmanTree() {

    }

    void InsertNodeToList(std::list<Node> &l, Node n);
    void create(std::unordered_map<char, int> &map);
    void dumpTree() {_dumpTree(root, 0, "root");};
    Node *getRootNode() {return root;}

private:
    void _dumpTree(Node *n, int depth, std::string s);
    std::list<Node> l;
    
    Node *root;
};

class Node {
public:
    Node() {};

    bool    isLeaf      {false};
    int     frequency   {0};
    char    character   {0};

    Node *lnode {nullptr};
    Node *rnode {nullptr};
};

class Encoder {
public:
    Encoder() {};
    void encode(HuffmanTree &t);
    void dumpMappings();
    std::unordered_map<char, std::vector<bool>> getMapping() {return m_mapping;}
    void encodeBuffer(const std::vector<char>&, std::vector<uint8_t> &outBuffer);

private:

    void generateHuffmanCodes(Node *root, std::vector<bool> code);

    std::unordered_map<char, std::vector<bool>> m_mapping;
};

void huffmanEncode(Settings &, bool parallel);
