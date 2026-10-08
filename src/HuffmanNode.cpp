#include "../include/HuffmanNode.h"

HuffmanNode::HuffmanNode(
    uint8_t byte,
    uint64_t frequency,
    uint64_t order
)
    : byte(byte),
      frequency(frequency),
      left(nullptr),
      right(nullptr),
      order(order) {
}

HuffmanNode::HuffmanNode(
    uint64_t frequency,
    HuffmanNode* left,
    HuffmanNode* right,
    uint64_t order
)
    : byte(0),
      frequency(frequency),
      left(left),
      right(right),
      order(order) {
}

bool HuffmanNode::isLeaf() const {

    return left == nullptr &&
           right == nullptr;
}