#pragma once
#include <cstddef>
#include "DLL.h"

template <typename Key, typename Value, int MinDegree = 3>
class BTree {
private:
    template <typename T>
    static void dllInsertAt(DLL<T>& list, int index, const T& value) {
        const int n = list.getSize();
        if (index < 0 || index > n) throw out_of_range("BTree dllInsertAt index out of range");

        list.push_back(value);
        for (int i = n - 1; i >= index; --i) {
            list[i + 1] = list[i];
        }
        list[index] = value;
    }

    template <typename T>
    static void dllTruncate(DLL<T>& list, int newSize) {
        if (newSize < 0) newSize = 0;
        while (list.getSize() > newSize) {
            list.pop_back();
        }
    }

    struct Node {
        bool leaf;
        DLL<Key> keys;
        DLL<Value> values;
        DLL<Node*> children;

        explicit Node(bool isLeaf) : leaf(isLeaf) {}
        ~Node() {
            for (auto* cur = children.getHead(); cur; cur = cur->next) {
                delete cur->data;
            }
        }
    };

    Node* root;

    void clear(Node* node) {
        delete node;
    }

    void splitChild(Node* parent, size_t childIndex) {
        Node* fullChild = parent->children[static_cast<int>(childIndex)];
        Node* newChild = new Node(fullChild->leaf);

        const size_t mid = MinDegree - 1;

        const Key promotedKey = fullChild->keys[static_cast<int>(mid)];
        const Value promotedValue = fullChild->values[static_cast<int>(mid)];

        // Move keys and values to new child
        for (size_t i = 0; i < MinDegree - 1; ++i) {
            newChild->keys.push_back(fullChild->keys[static_cast<int>(mid + 1 + i)]);
            newChild->values.push_back(fullChild->values[static_cast<int>(mid + 1 + i)]);
        }

        // Move children if not leaf
        if (!fullChild->leaf) {
            for (size_t i = 0; i < MinDegree; ++i)
                newChild->children.push_back(fullChild->children[static_cast<int>(mid + 1 + i)]);
            dllTruncate(fullChild->children, static_cast<int>(mid + 1));
        }

        dllTruncate(fullChild->keys, static_cast<int>(mid));
        dllTruncate(fullChild->values, static_cast<int>(mid));

        dllInsertAt(parent->children, static_cast<int>(childIndex + 1), newChild);
        dllInsertAt(parent->keys, static_cast<int>(childIndex), promotedKey);
        dllInsertAt(parent->values, static_cast<int>(childIndex), promotedValue);
    }

    void insertNonFull(Node* node, const Key& key, const Value& value) {
        int i = node->keys.getSize() - 1;

        if (node->leaf) {
            // Insert key/value in order
            node->keys.push_back(key);
            node->values.push_back(value);
            while (i >= 0 && key < node->keys[i]) {
                node->keys[i + 1] = node->keys[i];
                node->values[i + 1] = node->values[i];
                --i;
            }
            node->keys[i + 1] = key;
            node->values[i + 1] = value;
        } 
        else {
            while (i >= 0 && key < node->keys[i]) {
                --i;
            }
            ++i;

            if (node->children[i]->keys.getSize() == 2 * MinDegree - 1) {
                splitChild(node, static_cast<size_t>(i));
                if (key > node->keys[i]) {
                    ++i;
                }
            }
            insertNonFull(node->children[i], key, value);
        }
    }

    void rangeCollect(Node* node, const Key& start, const Key& end, DLL<Value>& out) const {
        if (!node)
            return;

        int i = 0;
        const int keyCount = node->keys.getSize();
        const int childCount = node->children.getSize();

        while (i < keyCount && node->keys[i] < start) {
            if (!node->leaf) {
                rangeCollect(node->children[i], start, end, out);
            }
            ++i;
        }

        while (i < keyCount && node->keys[i] <= end) {
            if (!node->leaf) {
                rangeCollect(node->children[i], start, end, out);
            }
            out.push_back(node->values[i]);
            ++i;
        }

        if (!node->leaf && i < childCount) {
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

        if (root->keys.getSize() == 2 * MinDegree - 1) {
            Node* newRoot = new Node(false);
            newRoot->children.push_back(root);
            splitChild(newRoot, 0);

            size_t i = 0;
            if (newRoot->keys[0] < key) {
                i = 1;
            }
            insertNonFull(newRoot->children[static_cast<int>(i)], key, value);
            root = newRoot;
        } 
        else insertNonFull(root, key, value);
    }

    DLL<Value> rangeQuery(const Key& start, const Key& end) const {
        DLL<Value> results;
        rangeCollect(root, start, end, results);
        return results;
    }
};
