#pragma once

#include <optional>
#include <queue>
#include <vector>

#include "huffman_constants.h"

namespace Huffman {
    class HuffmanTree {
    public:
        struct EncodedChar {
            Letter char_value;
            size_t codelen;
            Huffman::Code code;
        };

        explicit HuffmanTree();
        ~HuffmanTree();

        std::vector<EncodedChar> GetEncodedChars(const std::vector<size_t>& char_count);
        void BuildTreeWithLeaves(std::vector<std::pair<size_t, Letter>> char_codelen);
        std::optional<Letter> NextNode(bool to_right);

    private:
        class Node {
        public:
            explicit Node();
            explicit Node(Letter char_value, size_t priority);
            explicit Node(Node* left, Node* right);
            ~Node();

            Node* GetLeft() const;
            Node* GetRight() const;
            Letter GetChar() const;
            size_t GetPriority() const;
            Node* GoLeft();
            Node* GoRight();
            void SetChar(Letter char_value);

            bool operator<(const Node& other) const;

            friend class NodeHeap;

        private:
            Letter char_;
            size_t priority_;
            Node* left_;
            Node* right_;
        };

        Node* Merge(Node* left, Node* right);

        class NodeHeap {
        public:
            void Push(Node* node);
            void Pop();
            Node* GetMin() const;
            size_t Size() const;

        private:
            std::vector<Node*> heap_;

            size_t ParentIndex(size_t index) const;
            size_t LeftChildIndex(size_t index) const;
            size_t RightChildIndex(size_t index) const;
        };

        Node* root_;
        Node* active_node_;

        void BuildTreeWithHeap(NodeHeap& node_heap);
        void FindCharCodeLength(std::vector<std::pair<size_t, Letter>>& char_codelen);
        std::vector<EncodedChar> GetCanonicalHuffmanCodes(std::vector<std::pair<size_t, Letter>>& char_codelen);
        void AddLeaf(const EncodedChar& leaf);
    };
}
