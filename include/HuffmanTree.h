#ifndef HUFFMAN_TREE_H
#define HUFFMAN_TREE_H

#include "HuffmanNode.h"

#include <array>
#include <cstdint>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

struct CompareNodes {

    bool operator()(
        HuffmanNode* a,
        HuffmanNode* b
    ) const {

        if (a->frequency != b->frequency) {

            return a->frequency > b->frequency;
        }

        return a->order > b->order;
    }
};

class HuffmanTree {

private:

    HuffmanNode* root;

    std::unordered_map<
        uint8_t,
        std::string
    > codes;

    uint64_t nextOrder;

    void generateCodes(
        HuffmanNode* node,
        const std::string& currentCode
    );

    void deleteTree(
        HuffmanNode* node
    );

public:

    HuffmanTree();

    ~HuffmanTree();

    void build(
        const std::array<uint64_t, 256>& frequencies
    );

    const std::unordered_map<
        uint8_t,
        std::string
    >& getCodes() const;
};

#endif