#include "Trie.h"
#include <algorithm>
#include <locale>
#include <codecvt>
#include <cstring>

// Map Vietnamese characters to lowercase without diacritics
string normalizeString(const string& str) {
    string result;
    
    for (size_t i = 0; i < str.length(); ) {
        unsigned char c = str[i];
        
        // ASCII characters - just lowercase
        if (c < 128) {
            result += tolower(c);
            i++;
        }
        // 2-byte UTF-8 sequences
        else if ((c & 0xE0) == 0xC0 && i + 1 < str.length()) {
            unsigned char c1 = str[i], c2 = str[i + 1];
            
            // Đ/đ (U+0110/U+0111) -> d
            if (c1 == 0xC4 && (c2 == 0x90 || c2 == 0x91)) {
                result += 'd';
            }
            // À/à (U+00C0/U+00E0) -> a
            else if (c1 == 0xC3 && (c2 == 0x80 || c2 == 0xA0)) {
                result += 'a';
            }
            // Á/á (U+00C1/U+00E1) -> a
            else if (c1 == 0xC3 && (c2 == 0x81 || c2 == 0xA1)) {
                result += 'a';
            }
            // Â/â (U+00C2/U+00E2) -> a
            else if (c1 == 0xC3 && (c2 == 0x82 || c2 == 0xA2)) {
                result += 'a';
            }
            // Ã/ã (U+00C3/U+00E3) -> a
            else if (c1 == 0xC3 && (c2 == 0x83 || c2 == 0xA3)) {
                result += 'a';
            }
            // È/è (U+00C8/U+00E8) -> e
            else if (c1 == 0xC3 && (c2 == 0x88 || c2 == 0xA8)) {
                result += 'e';
            }
            // É/é (U+00C9/U+00E9) -> e
            else if (c1 == 0xC3 && (c2 == 0x89 || c2 == 0xA9)) {
                result += 'e';
            }
            // Ê/ê (U+00CA/U+00EA) -> e
            else if (c1 == 0xC3 && (c2 == 0x8A || c2 == 0xAA)) {
                result += 'e';
            }
            // Ì/ì (U+00CC/U+00EC) -> i
            else if (c1 == 0xC3 && (c2 == 0x8C || c2 == 0xAC)) {
                result += 'i';
            }
            // Í/í (U+00CD/U+00ED) -> i
            else if (c1 == 0xC3 && (c2 == 0x8D || c2 == 0xAD)) {
                result += 'i';
            }
            // Ò/ò (U+00D2/U+00F2) -> o
            else if (c1 == 0xC3 && (c2 == 0x92 || c2 == 0xB2)) {
                result += 'o';
            }
            // Ó/ó (U+00D3/U+00F3) -> o
            else if (c1 == 0xC3 && (c2 == 0x93 || c2 == 0xB3)) {
                result += 'o';
            }
            // Ô/ô (U+00D4/U+00F4) -> o
            else if (c1 == 0xC3 && (c2 == 0x94 || c2 == 0xB4)) {
                result += 'o';
            }
            // Õ/õ (U+00D5/U+00F5) -> o
            else if (c1 == 0xC3 && (c2 == 0x95 || c2 == 0xB5)) {
                result += 'o';
            }
            // Ù/ù (U+00D9/U+00F9) -> u
            else if (c1 == 0xC3 && (c2 == 0x99 || c2 == 0xB9)) {
                result += 'u';
            }
            // Ú/ú (U+00DA/U+00FA) -> u
            else if (c1 == 0xC3 && (c2 == 0x9A || c2 == 0xBA)) {
                result += 'u';
            }
            // Ý/ý (U+00DD/U+00FD) -> y
            else if (c1 == 0xC3 && (c2 == 0x9D || c2 == 0xBD)) {
                result += 'y';
            }
            // Ơ/ơ (U+01A0/U+01A1) -> o
            else if (c1 == 0xC6 && (c2 == 0xA0 || c2 == 0xA1)) {
                result += 'o';
            }
            // Ư/ư (U+01AF/U+01B0) -> u
            else if (c1 == 0xC6 && (c2 == 0xAF || c2 == 0xB0)) {
                result += 'u';
            }
            // Ă/ă (U+0102/U+0103) -> a
            else if (c1 == 0xC4 && (c2 == 0x82 || c2 == 0x83)) {
                result += 'a';
            }
            else {
                // Keep other 2-byte sequences as-is (lowercase if possible)
                result += tolower(c1);
                result += tolower(c2);
            }
            i += 2;
        }
        // 3-byte UTF-8 sequences (Vietnamese with tone marks)
        else if ((c & 0xF0) == 0xE0 && i + 2 < str.length()) {
            unsigned char c1 = str[i], c2 = str[i + 1], c3 = str[i + 2];
            
            // Vietnamese characters in E1 BA/BB range
            if (c1 == 0xE1) {
                // Ạ-Ặ, ạ-ặ (U+1EA0-U+1EB7) -> a
                if ((c2 == 0xBA && c3 >= 0xA0) || (c2 == 0xBB && c3 <= 0x97)) {
                    result += 'a';
                }
                // Ẹ-Ệ, ẹ-ệ (U+1EB8-U+1EC7) -> e
                else if (c2 == 0xBA && c3 >= 0xB8) {
                    result += 'e';
                }
                else if (c2 == 0xBB && c3 <= 0x87) {
                    result += 'e';
                }
                // Ỉ-Ị, ỉ-ị (U+1EC8-U+1ECB) -> i
                else if (c2 == 0xBB && c3 >= 0x88 && c3 <= 0x8B) {
                    result += 'i';
                }
                // Ọ-Ợ, ọ-ợ (U+1ECC-U+1EE3) -> o
                else if (c2 == 0xBB && c3 >= 0x8C && c3 <= 0xA3) {
                    result += 'o';
                }
                // Ụ-Ự, ụ-ự (U+1EE4-U+1EF1) -> u
                else if (c2 == 0xBB && c3 >= 0xA4 && c3 <= 0xB1) {
                    result += 'u';
                }
                // Ỳ-Ỵ, ỳ-ỵ (U+1EF2-U+1EF9) -> y
                else if (c2 == 0xBB && c3 >= 0xB2 && c3 <= 0xB9) {
                    result += 'y';
                }
                else {
                    // Keep as-is
                    result += c1; result += c2; result += c3;
                }
            }
            else {
                // Keep as-is
                result += c1; result += c2; result += c3;
            }
            i += 3;
        }
        // 4-byte UTF-8
        else if ((c & 0xF8) == 0xF0 && i + 3 < str.length()) {
            result += str[i]; result += str[i+1]; result += str[i+2]; result += str[i+3];
            i += 4;
        }
        else {
            result += c;
            i++;
        }
    }
    
    return result;
}

// ===== ChildNode Implementation =====
ChildNode::ChildNode(unsigned char k, TrieNode* c) 
    : key(k), child(c), next(nullptr) {}

// ===== TrieNode Implementation =====
TrieNode::TrieNode() : isEnd(false), childrenHead(nullptr) {}

TrieNode::~TrieNode() {
    ChildNode* current = childrenHead;
    while (current != nullptr) {
        ChildNode* temp = current;
        current = current->next;
        delete temp->child;  // Recursively delete
        delete temp;
    }
}

TrieNode* TrieNode::findChild(unsigned char key) {
    ChildNode* current = childrenHead;
    while (current != nullptr) {
        if (current->key == key) {
            return current->child;
        }
        current = current->next;
    }
    return nullptr;
}

TrieNode* TrieNode::addChild(unsigned char key) {
    // Check if already exists
    TrieNode* existing = findChild(key);
    if (existing != nullptr) {
        return existing;
    }
    
    // Create new child
    TrieNode* newChild = new TrieNode();
    ChildNode* newNode = new ChildNode(key, newChild);
    
    // Add to front of list (O(1))
    newNode->next = childrenHead;
    childrenHead = newNode;
    
    return newChild;
}

// ===== StringArray Implementation =====
StringArray::StringArray() : data(nullptr), size(0), capacity(0) {}

StringArray::~StringArray() {
    clear();
}

void StringArray::add(const string& str) {
    add(str.c_str());
}

void StringArray::add(const char* str) {
    if (size >= capacity) {
        int newCapacity = (capacity == 0) ? 10 : capacity * 2;
        char** newData = new char*[newCapacity];
        
        for (int i = 0; i < size; i++) {
            newData[i] = data[i];
        }
        
        delete[] data;
        data = newData;
        capacity = newCapacity;
    }
    
    // Copy string
    int len = strlen(str);
    data[size] = new char[len + 1];
    strcpy(data[size], str);
    size++;
}

const char* StringArray::get(int index) const {
    if (index >= 0 && index < size) {
        return data[index];
    }
    return nullptr;
}

void StringArray::clear() {
    for (int i = 0; i < size; i++) {
        delete[] data[i];
    }
    delete[] data;
    data = nullptr;
    size = 0;
    capacity = 0;
}

// ===== Trie Implementation =====
Trie::Trie() {
    root = new TrieNode();
}

Trie::~Trie() {
    delete root;
}

void Trie::insert(const string& word) {
    if (word.empty()) return;
    
    string normalized = normalizeString(word);
    TrieNode* current = root;
    
    for (size_t i = 0; i < normalized.length(); i++) {
        unsigned char c = (unsigned char)normalized[i];
        current = current->addChild(c);
    }
    current->isEnd = true;
}

bool Trie::search(const string& word) {
    if (word.empty()) return false;
    
    string normalized = normalizeString(word);
    TrieNode* current = root;
    
    for (size_t i = 0; i < normalized.length(); i++) {
        unsigned char c = (unsigned char)normalized[i];
        current = current->findChild(c);
        
        if (current == nullptr) {
            return false;
        }
    }
    return current->isEnd;
}

bool Trie::startsWith(const string& prefix) {
    if (prefix.empty()) return true;
    
    string normalized = normalizeString(prefix);
    TrieNode* current = root;
    
    for (size_t i = 0; i < normalized.length(); i++) {
        unsigned char c = (unsigned char)normalized[i];
        current = current->findChild(c);
        
        if (current == nullptr) {
            return false;
        }
    }
    return true;
}

void Trie::dfs(TrieNode* node, char* current, int currentLen, StringArray& results, int limit) {
    if (results.size >= limit) return;
    
    if (node->isEnd) {
        current[currentLen] = '\0';  // Null terminate
        results.add(current);
    }
    
    // Duyệt qua tất cả children trong linked list
    ChildNode* child = node->childrenHead;
    while (child != nullptr && results.size < limit) {
        current[currentLen] = (char)child->key;
        dfs(child->child, current, currentLen + 1, results, limit);
        child = child->next;
    }
}

StringArray Trie::getSuggestions(const string& prefix, int limit) {
    StringArray results;
    
    if (prefix.empty()) return results;
    
    string normalized = normalizeString(prefix);
    TrieNode* current = root;
    
    // Navigate to the prefix node
    for (size_t i = 0; i < normalized.length(); i++) {
        unsigned char c = (unsigned char)normalized[i];
        current = current->findChild(c);
        
        if (current == nullptr) {
            return results;  // Prefix not found
        }
    }
    
    // DFS to find all words with this prefix
    char buffer[256];  // Buffer for building words
    
    // Copy prefix to buffer
    for (size_t i = 0; i < normalized.length(); i++) {
        buffer[i] = normalized[i];
    }
    
    dfs(current, buffer, normalized.length(), results, limit);
    
    return results;
}
