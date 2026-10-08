#ifndef HUFFMAN_NODE_H
#define HUFFMAN_NODE_H

#include <cstdint>

struct HuffmanNode {

    uint8_t byte;

    uint64_t frequency;

    HuffmanNode* left;
    HuffmanNode* right;

    uint64_t order;

    HuffmanNode(
        uint8_t byte,
        uint64_t frequency,
        uint64_t order
    );

    HuffmanNode(
        uint64_t frequency,
        HuffmanNode* left,
        HuffmanNode* right,
        uint64_t order
    );

    bool isLeaf() const;
};

#endif