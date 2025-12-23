// #pragma once
// #include <cstddef>
// #include "DLL.h"

// template <typename Key, typename Value, int MinDegree = 3>
// class BTree {
// private:
//     template <typename T>
//     static void dllInsertAt(DLL<T>& list, int index, const T& value) {
//         const int n = list.getSize();
//         if (index < 0 || index > n) throw out_of_range("BTree dllInsertAt index out of range");

//         list.push_back(value);
//         for (int i = n - 1; i >= index; --i) {
//             list[i + 1] = list[i];
//         }
//         list[index] = value;
//     }

//     template <typename T>
//     static void dllTruncate(DLL<T>& list, int newSize) {
//         if (newSize < 0) newSize = 0;
//         while (list.getSize() > newSize) {
//             list.pop_back();
//         }
//     }

//     struct Node {
//         bool leaf;
//         DLL<Key> keys;
//         DLL<Value> values;
//         DLL<Node*> children;

//         explicit Node(bool isLeaf) : leaf(isLeaf) {}
//         ~Node() {
//             for (auto* cur = children.getHead(); cur; cur = cur->next) {
//                 delete cur->data;
//             }
//         }
//     };

//     Node* root;

//     void clear(Node* node) {
//         delete node;
//     }

//     void splitChild(Node* parent, size_t childIndex) {
//         Node* fullChild = parent->children[static_cast<int>(childIndex)];
//         Node* newChild = new Node(fullChild->leaf);

//         const size_t mid = MinDegree - 1;

//         const Key promotedKey = fullChild->keys[static_cast<int>(mid)];
//         const Value promotedValue = fullChild->values[static_cast<int>(mid)];

//         // Move keys and values to new child
//         for (size_t i = 0; i < MinDegree - 1; ++i) {
//             newChild->keys.push_back(fullChild->keys[static_cast<int>(mid + 1 + i)]);
//             newChild->values.push_back(fullChild->values[static_cast<int>(mid + 1 + i)]);
//         }

//         // Move children if not leaf
//         if (!fullChild->leaf) {
//             for (size_t i = 0; i < MinDegree; ++i)
//                 newChild->children.push_back(fullChild->children[static_cast<int>(mid + 1 + i)]);
//             dllTruncate(fullChild->children, static_cast<int>(mid + 1));
//         }

//         dllTruncate(fullChild->keys, static_cast<int>(mid));
//         dllTruncate(fullChild->values, static_cast<int>(mid));

//         dllInsertAt(parent->children, static_cast<int>(childIndex + 1), newChild);
//         dllInsertAt(parent->keys, static_cast<int>(childIndex), promotedKey);
//         dllInsertAt(parent->values, static_cast<int>(childIndex), promotedValue);
//     }

//     void insertNonFull(Node* node, const Key& key, const Value& value) {
//         int i = node->keys.getSize() - 1;

//         if (node->leaf) {
//             // Insert key/value in order
//             node->keys.push_back(key);
//             node->values.push_back(value);
//             while (i >= 0 && key < node->keys[i]) {
//                 node->keys[i + 1] = node->keys[i];
//                 node->values[i + 1] = node->values[i];
//                 --i;
//             }
//             node->keys[i + 1] = key;
//             node->values[i + 1] = value;
//         } 
//         else {
//             while (i >= 0 && key < node->keys[i]) {
//                 --i;
//             }
//             ++i;

//             if (node->children[i]->keys.getSize() == 2 * MinDegree - 1) {
//                 splitChild(node, static_cast<size_t>(i));
//                 if (key > node->keys[i]) {
//                     ++i;
//                 }
//             }
//             insertNonFull(node->children[i], key, value);
//         }
//     }

//     void rangeCollect(Node* node, const Key& start, const Key& end, DLL<Value>& out) const {
//         if (!node)
//             return;

//         int i = 0;
//         const int keyCount = node->keys.getSize();
//         const int childCount = node->children.getSize();

//         while (i < keyCount && node->keys[i] < start) {
//             if (!node->leaf) {
//                 rangeCollect(node->children[i], start, end, out);
//             }
//             ++i;
//         }

//         while (i < keyCount && node->keys[i] <= end) {
//             if (!node->leaf) {
//                 rangeCollect(node->children[i], start, end, out);
//             }
//             out.push_back(node->values[i]);
//             ++i;
//         }

//         if (!node->leaf && i < childCount) {
//             rangeCollect(node->children[i], start, end, out);
//         }
//     }

// public:
//     BTree() : root(nullptr) {}
//     ~BTree() { clear(root); }

//     void clear() {
//         clear(root);
//         root = nullptr;
//     }

//     bool empty() const { return root == nullptr; }

//     void insert(const Key& key, const Value& value) {
//         if (!root) {
//             root = new Node(true);
//             root->keys.push_back(key);
//             root->values.push_back(value);
//             return;
//         }

//         if (root->keys.getSize() == 2 * MinDegree - 1) {
//             Node* newRoot = new Node(false);
//             newRoot->children.push_back(root);
//             splitChild(newRoot, 0);

//             size_t i = 0;
//             if (newRoot->keys[0] < key) {
//                 i = 1;
//             }
//             insertNonFull(newRoot->children[static_cast<int>(i)], key, value);
//             root = newRoot;
//         } 
//         else insertNonFull(root, key, value);
//     }

//     DLL<Value> rangeQuery(const Key& start, const Key& end) const {
//         DLL<Value> results;
//         rangeCollect(root, start, end, results);
//         return results;
//     }
// };

#pragma once

#include <cstddef>
#include <utility>
#include "DLL.h"

/**
 * Generic B-Tree implementation used for fast range queries on revenue data.
 * Key must be comparable with < and > operators.
 * 
 * Implemented without std::vector - uses static arrays for optimal performance.
 * Each node has fixed-size arrays based on MinDegree:
 * - Maximum keys per node: 2*MinDegree - 1
 * - Maximum children per node: 2*MinDegree
 */
template <typename Key, typename Value, int MinDegree = 3>
class BTree {
private:
    // Constants for array sizes
    static constexpr int MAX_KEYS = 2 * MinDegree - 1;
    static constexpr int MAX_CHILDREN = 2 * MinDegree;

    struct Node {
        bool leaf;
        int keyCount;                    // Current number of keys
        Key keys[MAX_KEYS];              // Array of keys
        Value values[MAX_KEYS];          // Array of values corresponding to keys
        Node* children[MAX_CHILDREN];    // Array of child pointers

        explicit Node(bool isLeaf) : leaf(isLeaf), keyCount(0) {
            // Initialize all child pointers to nullptr
            for (int i = 0; i < MAX_CHILDREN; ++i) {
                children[i] = nullptr;
            }
        }

        ~Node() {
            // Recursively delete all children
            for (int i = 0; i <= keyCount; ++i) {
                if (children[i] != nullptr) {
                    delete children[i];
                    children[i] = nullptr;
                }
            }
        }

        // Insert key-value at specific position, shifting existing elements
        void insertKeyValueAt(int pos, const Key& key, const Value& value) {
            // Shift keys and values to the right
            for (int i = keyCount; i > pos; --i) {
                keys[i] = keys[i - 1];
                values[i] = values[i - 1];
            }
            keys[pos] = key;
            values[pos] = value;
            ++keyCount;
        }

        // Insert child at specific position, shifting existing children
        void insertChildAt(int pos, Node* child) {
            // Shift children to the right
            for (int i = keyCount + 1; i > pos; --i) {
                children[i] = children[i - 1];
            }
            children[pos] = child;
        }

        // Remove key-value at specific position, shifting elements left
        void removeKeyValueAt(int pos) {
            for (int i = pos; i < keyCount - 1; ++i) {
                keys[i] = keys[i + 1];
                values[i] = values[i + 1];
            }
            --keyCount;
        }

        // Remove child at specific position, shifting children left
        void removeChildAt(int pos) {
            for (int i = pos; i < keyCount; ++i) {
                children[i] = children[i + 1];
            }
            children[keyCount] = nullptr;
        }
    };

    Node* root;

    void clear(Node* node) {
        delete node;
    }

    void splitChild(Node* parent, int childIndex) {
        Node* fullChild = parent->children[childIndex];
        Node* newChild = new Node(fullChild->leaf);

        const int mid = MinDegree - 1;

        // Copy the second half of keys and values to new child
        newChild->keyCount = MinDegree - 1;
        for (int i = 0; i < MinDegree - 1; ++i) {
            newChild->keys[i] = fullChild->keys[mid + 1 + i];
            newChild->values[i] = fullChild->values[mid + 1 + i];
        }

        // If not a leaf, copy the second half of children to new child
        if (!fullChild->leaf) {
            for (int i = 0; i < MinDegree; ++i) {
                newChild->children[i] = fullChild->children[mid + 1 + i];
                fullChild->children[mid + 1 + i] = nullptr;
            }
        }

        // Store the middle key and value before modifying fullChild
        Key midKey = fullChild->keys[mid];
        Value midValue = fullChild->values[mid];

        // Reduce the key count of the full child
        fullChild->keyCount = mid;

        // Insert the new child into parent
        parent->insertChildAt(childIndex + 1, newChild);

        // Insert the middle key into parent
        parent->insertKeyValueAt(childIndex, midKey, midValue);
    }

    void insertNonFull(Node* node, const Key& key, const Value& value) {
        int i = node->keyCount - 1;

        if (node->leaf) {
            // Find position and insert key/value in sorted order
            while (i >= 0 && key < node->keys[i]) {
                --i;
            }
            node->insertKeyValueAt(i + 1, key, value);
        } else {
            // Find the child to descend into
            while (i >= 0 && key < node->keys[i]) {
                --i;
            }
            ++i;

            // Split child if it's full
            if (node->children[i]->keyCount == MAX_KEYS) {
                splitChild(node, i);
                if (key > node->keys[i]) {
                    ++i;
                }
            }
            insertNonFull(node->children[i], key, value);
        }
    }

    void rangeCollect(Node* node, const Key& start, const Key& end, DLL<Value>& out) const {
        if (!node) {
            return;
        }

        int i = 0;
        // Find the first key >= start
        while (i < node->keyCount && node->keys[i] < start) {
            ++i;
        }

        // Collect keys in range [start, end]
        while (i < node->keyCount && node->keys[i] <= end) {
            if (!node->leaf) {
                if (node->children[i] != nullptr) {
                    rangeCollect(node->children[i], start, end, out);
                }
            }
            out.push_back(node->values[i]);
            ++i;
        }

        // Check the rightmost child
        if (!node->leaf && i <= node->keyCount && node->children[i] != nullptr) {
            rangeCollect(node->children[i], start, end, out);
        }
    }

    // Helper function to find a key in a node, returns index or -1 if not found
    int findKeyIndex(Node* node, const Key& key) const {
        int idx = 0;
        while (idx < node->keyCount && node->keys[idx] < key) {
            ++idx;
        }
        return idx;
    }

    // Collect all values in a subtree (in-order traversal)
    void collectAll(Node* node, DLL<Value>& out) const {
        if (!node) return;
        
        for (int i = 0; i < node->keyCount; ++i) {
            if (!node->leaf) {
                collectAll(node->children[i], out);
            }
            out.push_back(node->values[i]);
        }
        if (!node->leaf) {
            collectAll(node->children[node->keyCount], out);
        }
    }

public:
    BTree() : root(nullptr) {}
    BTree(const BTree&) = delete;
    BTree& operator=(const BTree&) = delete;

    BTree(BTree&& other) noexcept : root(other.root) {
        other.root = nullptr;
    }

    BTree& operator=(BTree&& other) noexcept {
        if (this == &other) return *this;
        clear(root);
        root = other.root;
        other.root = nullptr;
        return *this;
    }

    ~BTree() { clear(root); }

    void clear() {
        clear(root);
        root = nullptr;
    }

    bool empty() const { return root == nullptr; }

    void insert(const Key& key, const Value& value) {
        if (!root) {
            root = new Node(true);
            root->keys[0] = key;
            root->values[0] = value;
            root->keyCount = 1;
            return;
        }

        // If root is full, split it
        if (root->keyCount == MAX_KEYS) {
            Node* newRoot = new Node(false);
            newRoot->children[0] = root;
            splitChild(newRoot, 0);

            // Decide which child to insert into
            int i = 0;
            if (newRoot->keys[0] < key) {
                i = 1;
            }
            insertNonFull(newRoot->children[i], key, value);
            root = newRoot;
        } else {
            insertNonFull(root, key, value);
        }
    }

    // Search for a value by key, returns pointer to value or nullptr if not found
    Value* search(const Key& key) {
        Node* current = root;
        while (current != nullptr) {
            int i = findKeyIndex(current, key);
            
            // Check if key is found at index i
            if (i < current->keyCount && !(key < current->keys[i]) && !(current->keys[i] < key)) {
                return &current->values[i];
            }
            
            // If leaf node, key doesn't exist
            if (current->leaf) {
                return nullptr;
            }
            
            // Move to appropriate child
            current = current->children[i];
        }
        return nullptr;
    }

    // Const version of search
    const Value* search(const Key& key) const {
        const Node* current = root;
        while (current != nullptr) {
            int i = 0;
            while (i < current->keyCount && current->keys[i] < key) {
                ++i;
            }
            
            if (i < current->keyCount && !(key < current->keys[i]) && !(current->keys[i] < key)) {
                return &current->values[i];
            }
            
            if (current->leaf) {
                return nullptr;
            }
            
            current = current->children[i];
        }
        return nullptr;
    }

    // Range query: returns all values with keys in [start, end]
    DLL<Value> rangeQuery(const Key& start, const Key& end) const {
        DLL<Value> results;
        rangeCollect(root, start, end, results);
        return results;
    }

    // Get all values in the tree (in-order)
    DLL<Value> getAllValues() const {
        DLL<Value> results;
        collectAll(root, results);
        return results;
    }

    // Check if a key exists in the tree
    bool contains(const Key& key) const {
        return search(key) != nullptr;
    }
};