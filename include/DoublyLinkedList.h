#pragma once
#include <cstddef>

// Minimal doubly linked list without using std containers
template <typename T>
class DoublyLinkedList {
private:
    struct Node {
        T data;
        Node* prev;
        Node* next;
        Node(const T& d) : data(d), prev(nullptr), next(nullptr) {}
    };

    Node* head_ = nullptr;
    Node* tail_ = nullptr;
    std::size_t size_ = 0;

public:
    DoublyLinkedList() = default;
    ~DoublyLinkedList() { clear(); }

    DoublyLinkedList(const DoublyLinkedList& other) = delete;
    DoublyLinkedList& operator=(const DoublyLinkedList& other) = delete;

    std::size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

    void push_back(const T& value) {
        Node* n = new Node(value);
        if (!tail_) {
            head_ = tail_ = n;
        } else {
            tail_->next = n;
            n->prev = tail_;
            tail_ = n;
        }
        ++size_;
    }

    void push_front(const T& value) {
        Node* n = new Node(value);
        if (!head_) {
            head_ = tail_ = n;
        } else {
            n->next = head_;
            head_->prev = n;
            head_ = n;
        }
        ++size_;
    }

    // Remove node at index (0-based). No-op if out of range.
    void removeAt(std::size_t index) {
        if (index >= size_) return;
        Node* cur = head_;
        for (std::size_t i = 0; i < index; ++i) cur = cur->next;
        if (cur->prev) cur->prev->next = cur->next; else head_ = cur->next;
        if (cur->next) cur->next->prev = cur->prev; else tail_ = cur->prev;
        delete cur;
        --size_;
    }

    // Access by index (unchecked; caller must ensure valid index)
    T& get(std::size_t index) {
        Node* cur = head_;
        for (std::size_t i = 0; i < index; ++i) cur = cur->next;
        return cur->data;
    }
    const T& get(std::size_t index) const {
        const Node* cur = head_;
        for (std::size_t i = 0; i < index; ++i) cur = cur->next;
        return cur->data;
    }

    void clear() {
        Node* cur = head_;
        while (cur) {
            Node* n = cur->next;
            delete cur;
            cur = n;
        }
        head_ = tail_ = nullptr;
        size_ = 0;
    }
};

