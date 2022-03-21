#include "huffmantree.h"

#include <algorithm>
#include <stdexcept>

namespace Huffman {
    HuffmanTree::HuffmanTree() {
        root_ = nullptr;
        active_node_ = nullptr;
    }

    HuffmanTree::~HuffmanTree() {
        delete root_;
    }

    std::vector<HuffmanTree::EncodedChar> HuffmanTree::GetEncodedChars(const std::vector<size_t>& char_count) {
        HuffmanTree::NodeHeap node_heap;
        for (Letter i = 0; i < char_count.size(); ++i) {
            if (char_count[i] > 0) {
                node_heap.Push(new Node(i, char_count[i]));
            }
        }
        BuildTreeWithHeap(node_heap);
        std::vector<std::pair<size_t, Letter>> char_codelen;
        FindCharCodeLength(char_codelen);
        std::sort(char_codelen.begin(), char_codelen.end());
        return GetCanonicalHuffmanCodes(char_codelen);
    }

    void HuffmanTree::BuildTreeWithLeaves(std::vector<std::pair<size_t, Letter>> char_codelen) {
        std::vector<EncodedChar> leaves = GetCanonicalHuffmanCodes(char_codelen);
        delete root_;
        root_ = new Node();
        for (const auto& leaf : leaves) {
            AddLeaf(leaf);
        }
        active_node_ = root_;
    }

    std::optional<Letter> HuffmanTree::NextNode(bool to_right) {
        if (active_node_->GetLeft() == nullptr && active_node_->GetRight() == nullptr) {
            active_node_ = root_;
        }
        if (to_right) {
            if (active_node_->GetRight() == nullptr) {
                throw std::runtime_error("NextNode: GetRight nullptr exception");
            }
            active_node_ = active_node_->GetRight();
        } else {
            if (active_node_->GetLeft() == nullptr) {
                throw std::runtime_error("NextNode: GetLeft nullptr exception");
            }
            active_node_ = active_node_->GetLeft();
        }
        if (active_node_->GetLeft() == nullptr && active_node_->GetRight() == nullptr) {
            return active_node_->GetChar();
        } else {
            return std::nullopt;
        }
    }

    HuffmanTree::Node::Node() {
        this->char_ = 0;
        this->left_ = nullptr;
        this->right_ = nullptr;
        this->priority_ = 0;
    }

    HuffmanTree::Node::Node(Letter char_value, size_t priority) {
        this->char_ = char_value;
        this->left_ = nullptr;
        this->right_ = nullptr;
        this->priority_ = priority;
    }

    HuffmanTree::Node::Node(Node* left, Node* right) {
        this->char_ = 0;
        this->left_ = left;
        this->right_ = right;
        this->priority_ = left->GetPriority() + right->GetPriority();
    }

    HuffmanTree::Node::~Node() {
        delete left_;
        delete right_;
    }

    HuffmanTree::Node* HuffmanTree::Node::GetLeft() const {
        return left_;
    }

    HuffmanTree::Node* HuffmanTree::Node::GetRight() const {
        return right_;
    }

    size_t HuffmanTree::Node::GetChar() const {
        return char_;
    }

    size_t HuffmanTree::Node::GetPriority() const {
        return priority_;
    }

    HuffmanTree::Node* HuffmanTree::Node::GoLeft() {
        if (left_ == nullptr) {
            left_ = new Node();
        }
        return left_;
    }

    HuffmanTree::Node* HuffmanTree::Node::GoRight() {
        if (right_ == nullptr) {
            right_ = new Node();
        }
        return right_;
    }

    void HuffmanTree::Node::SetChar(Letter char_value) {
        char_ = char_value;
    }

    bool HuffmanTree::Node::operator<(const HuffmanTree::Node& other) const {
        return this->GetPriority() < other.GetPriority();
    }

    HuffmanTree::Node* HuffmanTree::Merge(Node* left, Node* right) {
        return new Node(left, right);
    }

    void HuffmanTree::NodeHeap::Push(HuffmanTree::Node* node) {
        heap_.push_back(node);
        size_t index = heap_.size() - 1;
        while (index != 0 && *heap_[index] < *heap_[ParentIndex(index)]) {
            std::swap(heap_[index], heap_[ParentIndex(index)]);
            index = ParentIndex(index);
        }
    }

    void HuffmanTree::NodeHeap::Pop() {
        std::swap(heap_.front(), heap_.back());
        heap_.pop_back();
        size_t index = 0;
        while (LeftChildIndex(index) < heap_.size()) {
            if (RightChildIndex(index) < heap_.size() && *heap_[RightChildIndex(index)] < *heap_[index] &&
                *heap_[RightChildIndex(index)] < *heap_[LeftChildIndex(index)]) {
                std::swap(heap_[index], heap_[RightChildIndex(index)]);
                index = RightChildIndex(index);
            } else if (*heap_[LeftChildIndex(index)] < *heap_[index]) {
                std::swap(heap_[index], heap_[LeftChildIndex(index)]);
                index = LeftChildIndex(index);
            } else {
                break;
            }
        }
    }

    HuffmanTree::Node* HuffmanTree::NodeHeap::GetMin() const {
        return heap_.front();
    }

    size_t HuffmanTree::NodeHeap::Size() const {
        return heap_.size();
    }

    size_t HuffmanTree::NodeHeap::ParentIndex(size_t index) const {
        return (index - 1) / 2;
    }

    size_t HuffmanTree::NodeHeap::LeftChildIndex(size_t index) const {
        return index * 2 + 1;
    }

    size_t HuffmanTree::NodeHeap::RightChildIndex(size_t index) const {
        return index * 2 + 2;
    }

    void HuffmanTree::BuildTreeWithHeap(NodeHeap& node_heap) {
        while (node_heap.Size() > 1) {
            Node* A = node_heap.GetMin();
            node_heap.Pop();
            Node* B = node_heap.GetMin();
            node_heap.Pop();
            Node* C = Merge(A, B);
            node_heap.Push(C);
        }
        delete root_;
        root_ = node_heap.GetMin();
    }

    void HuffmanTree::FindCharCodeLength(std::vector<std::pair<size_t, Letter>>& char_codelen) {
        std::queue<std::pair<Node*, size_t>> bfs_queue;
        bfs_queue.push({ root_, 0 });
        while (!bfs_queue.empty()) {
            auto node = bfs_queue.front();
            bfs_queue.pop();
            if (node.first->GetLeft() == nullptr && node.first->GetRight() == nullptr) {
                char_codelen.push_back({ node.second, node.first->GetChar() });
            } else {
                bfs_queue.push({ node.first->GetLeft(), node.second + 1 });
                bfs_queue.push({ node.first->GetRight(), node.second + 1 });
            }
        }
    }

    std::vector<HuffmanTree::EncodedChar> HuffmanTree::GetCanonicalHuffmanCodes(std::vector<std::pair<size_t, Letter>>& char_codelen) {
        std::vector<EncodedChar> encoded_chars;
        Huffman::Code code(char_codelen[0].first, 0);
        for (size_t i = 0; i < char_codelen.size(); ++i) {
            encoded_chars.push_back({ char_codelen[i].second, char_codelen[i].first, code });
            if (i + 1 < char_codelen.size()) {
                auto it = code.end();
                --it;
                while (*it) {
                    if (it == code.begin()) {
                        throw std::runtime_error("GetCanonicalHuffmanCodes code iterator exception");
                    }
                    *it = 0;
                    --it;
                }
                *it = 1;
                size_t shift = char_codelen[i + 1].first - char_codelen[i].first;
                for (size_t j = 0; j < shift; ++j) {
                    code.push_back(0);
                }
            }
        }
        return encoded_chars;
    }

    void HuffmanTree::AddLeaf(const EncodedChar& leaf) {
        Node* current_node = root_;
        for (size_t i = 0; i < leaf.codelen; ++i) {
            if (leaf.code[i]) {
                current_node = current_node->GoRight();
            } else {
                current_node = current_node->GoLeft();
            }
        }
        current_node->SetChar(leaf.char_value);
    }
}
