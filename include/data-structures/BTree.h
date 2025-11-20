#pragma once

#include <vector>
#include <algorithm>
#include <cstddef>
#include "DLL.h"

/**
 * Generic B-Tree implementation used for fast range queries on revenue data.
 * Key must be comparable with < and > operators.
 */
template <typename Key, typename Value, int MinDegree = 3>
class BTree {
private:
    struct Node {
        bool leaf;
        std::vector<Key> keys;
        std::vector<Value> values;
        std::vector<Node*> children;

        explicit Node(bool isLeaf) : leaf(isLeaf) {}
        ~Node() {
            for (Node* child : children) {
                delete child;
            }
        }
    };

    Node* root;

    void clear(Node* node) {
        delete node;
    }

    void splitChild(Node* parent, std::size_t childIndex) {
        Node* fullChild = parent->children[childIndex];
        Node* newChild = new Node(fullChild->leaf);

        const std::size_t mid = MinDegree - 1;

        // Move keys and values to new child
        for (std::size_t i = 0; i < MinDegree - 1; ++i) {
            newChild->keys.push_back(fullChild->keys[mid + 1 + i]);
            newChild->values.push_back(fullChild->values[mid + 1 + i]);
        }

        // Move children if not leaf
        if (!fullChild->leaf) {
            for (std::size_t i = 0; i < MinDegree; ++i) {
                newChild->children.push_back(fullChild->children[mid + 1 + i]);
            }
            fullChild->children.resize(mid + 1);
        }

        fullChild->keys.resize(mid);
        fullChild->values.resize(mid);

        parent->children.insert(parent->children.begin() + childIndex + 1, newChild);
        parent->keys.insert(parent->keys.begin() + childIndex, fullChild->keys[mid]);
        parent->values.insert(parent->values.begin() + childIndex, fullChild->values[mid]);
    }

    void insertNonFull(Node* node, const Key& key, const Value& value) {
        int i = static_cast<int>(node->keys.size()) - 1;

        if (node->leaf) {
            // Insert key/value in order
            node->keys.emplace_back();
            node->values.emplace_back();
            while (i >= 0 && key < node->keys[static_cast<std::size_t>(i)]) {
                node->keys[static_cast<std::size_t>(i + 1)] = node->keys[static_cast<std::size_t>(i)];
                node->values[static_cast<std::size_t>(i + 1)] = node->values[static_cast<std::size_t>(i)];
                --i;
            }
            node->keys[static_cast<std::size_t>(i + 1)] = key;
            node->values[static_cast<std::size_t>(i + 1)] = value;
        } else {
            while (i >= 0 && key < node->keys[static_cast<std::size_t>(i)]) {
                --i;
            }
            ++i;

            if (node->children[static_cast<std::size_t>(i)]->keys.size() == 2 * MinDegree - 1) {
                splitChild(node, static_cast<std::size_t>(i));
                if (key > node->keys[static_cast<std::size_t>(i)]) {
                    ++i;
                }
            }
            insertNonFull(node->children[static_cast<std::size_t>(i)], key, value);
        }
    }

    void rangeCollect(Node* node, const Key& start, const Key& end, DLL<Value>& out) const {
        if (!node) {
            return;
        }

        std::size_t i = 0;
        while (i < node->keys.size() && node->keys[i] < start) {
            if (!node->leaf) {
                rangeCollect(node->children[i], start, end, out);
            }
            ++i;
        }

        while (i < node->keys.size() && node->keys[i] <= end) {
            if (!node->leaf) {
                rangeCollect(node->children[i], start, end, out);
            }
            out.push_back(node->values[i]);
            ++i;
        }

        if (!node->leaf && i < node->children.size()) {
            rangeCollect(node->children[i], start, end, out);
        }
    }

public:
    BTree() : root(nullptr) {}
    ~BTree() { clear(root); }

    void clear() {
        clear(root);
        root = nullptr;
    }

    bool empty() const { return root == nullptr; }

    void insert(const Key& key, const Value& value) {
        if (!root) {
            root = new Node(true);
            root->keys.push_back(key);
            root->values.push_back(value);
            return;
        }

        if (root->keys.size() == 2 * MinDegree - 1) {
            Node* newRoot = new Node(false);
            newRoot->children.push_back(root);
            splitChild(newRoot, 0);

            std::size_t i = 0;
            if (newRoot->keys[0] < key) {
                i = 1;
            }
            insertNonFull(newRoot->children[i], key, value);
            root = newRoot;
        } else {
            insertNonFull(root, key, value);
        }
    }

    DLL<Value> rangeQuery(const Key& start, const Key& end) const {
        DLL<Value> results;
        rangeCollect(root, start, end, results);
        return results;
    }
};
