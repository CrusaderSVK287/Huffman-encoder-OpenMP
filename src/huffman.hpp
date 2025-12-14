#pragma once

#include <string>
#include <unordered_map>
#include <list>

#include "settings.hpp"
#include "decoder.hpp"

class HuffmanTree;
class Node;

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


void huffmanEncode(Settings &settings);
void huffmanDecode(Settings &settings);
