#include "../include/HuffmanTree.h"

HuffmanTree::HuffmanTree()
    : root(nullptr),
      nextOrder(0) {
}

void HuffmanTree::build(
    const std::array<uint64_t, 256>& frequencies
) {

    // In case build() is called more than once
    deleteTree(root);

    root = nullptr;

    codes.clear();

    nextOrder = 0;

    std::priority_queue<
        HuffmanNode*,
        std::vector<HuffmanNode*>,
        CompareNodes
    > minHeap;

    // Create leaf nodes
    for (int i = 0; i < 256; i++) {

        if (frequencies[i] > 0) {

            HuffmanNode* node =
                new HuffmanNode(
                    static_cast<uint8_t>(i),
                    frequencies[i],
                    nextOrder++
                );

            minHeap.push(node);
        }
    }

    // Empty file
    if (minHeap.empty()) {

        root = nullptr;

        return;
    }

    // Build Huffman tree
    while (minHeap.size() > 1) {

        HuffmanNode* left =
            minHeap.top();

        minHeap.pop();

        HuffmanNode* right =
            minHeap.top();

        minHeap.pop();

        HuffmanNode* parent =
            new HuffmanNode(
                left->frequency +
                right->frequency,

                left,
                right,

                nextOrder++
            );

        minHeap.push(parent);
    }

    root = minHeap.top();

    // Generate Huffman codes
    generateCodes(root, "");
}

void HuffmanTree::generateCodes(
    HuffmanNode* node,
    const std::string& currentCode
) {

    if (node == nullptr) {
        return;
    }

    // Leaf node
    if (node->isLeaf()) {

        // Special case:
        // file contains only one unique byte
        if (currentCode.empty()) {

            codes[node->byte] = "0";

        } else {

            codes[node->byte] = currentCode;
        }

        return;
    }

    // Left = 0
    generateCodes(
        node->left,
        currentCode + "0"
    );

    // Right = 1
    generateCodes(
        node->right,
        currentCode + "1"
    );
}

const std::unordered_map<uint8_t, std::string>&
HuffmanTree::getCodes() const {

    return codes;
}

void HuffmanTree::deleteTree(
    HuffmanNode* node
) {

    if (node == nullptr) {
        return;
    }

    deleteTree(node->left);

    deleteTree(node->right);

    delete node;
}

HuffmanTree::~HuffmanTree() {

    deleteTree(root);
}