#pragma once
#include <utility>
#include <stdexcept>
using namespace std;

template <typename T>
class Node {
public:
    T data;
    Node* prev;
    Node* next;
    Node(const T& value) : data(value), prev(nullptr), next(nullptr) {}
    Node(T&& value) : data(move(value)), prev(nullptr), next(nullptr) {}
};

template <typename T>
class DLL {
private:
    Node<T>* head;
    Node<T>* tail;
    int size;

    void link_back(Node<T>* n) {
        if (!head) head = tail = n;
        else { tail->next = n; n->prev = tail; tail = n; }
        ++size;
    }

    void link_front(Node<T>* n) {
        if (!head) head = tail = n;
        else { n->next = head; head->prev = n; head = n; }
        ++size;
    }

public:
    DLL() : head(nullptr), tail(nullptr), size(0) {}

    DLL(const DLL& other) : head(nullptr), tail(nullptr), size(0) {
        for (Node<T>* cur = other.head; cur; cur = cur->next) {
            link_back(new Node<T>(cur->data));
        }
    }

    DLL& operator=(const DLL& other) {
        if (this == &other) return *this;
        clear();
        for (Node<T>* cur = other.head; cur; cur = cur->next) {
            link_back(new Node<T>(cur->data));
        }
        return *this;
    }

    DLL(DLL&& other) noexcept
        : head(other.head), tail(other.tail), size(other.size) {
        other.head = other.tail = nullptr;
        other.size = 0;
    }

    DLL& operator=(DLL&& other) noexcept {
        if (this == &other) return *this;
        clear();
        head = other.head; tail = other.tail; size = other.size;
        other.head = other.tail = nullptr; other.size = 0;
        return *this;
    }

    ~DLL() { clear(); }

    bool isEmpty() const { return size == 0; }
    int  getSize() const { return size; }

    void push_back(const T& value) {
        link_back(new Node<T>(value));
    }

    void push_back(T&& value) {
        link_back(new Node<T>(move(value)));
    }

    void push_front(const T& value) {
        link_front(new Node<T>(value));
    }

    void push_front(T&& value) {
        link_front(new Node<T>(move(value)));
    }

    void pop_front() {
        if (isEmpty()) return;
        Node<T>* temp = head;
        head = head->next;
        if (head) head->prev = nullptr; 
        else tail = nullptr;
        delete temp;
        --size;
    }

    void pop_back() {
        if (isEmpty()) return;
        Node<T>* temp = tail;
        tail = tail->prev;
        if (tail) tail->next = nullptr; 
        else head = nullptr;
        delete temp;
        --size;
    }

    void removeAt(int index) {
        if (index < 0 || index >= size) throw out_of_range("DLL removeAt index out of range");
        Node<T>* cur = head;
        for (int i = 0; i < index; ++i) {
            cur = cur->next;
        }
        if (cur->prev) {
            cur->prev->next = cur->next;
        } else {
            head = cur->next;
        }
        if (cur->next) {
            cur->next->prev = cur->prev;
        } else {
            tail = cur->prev;
        }
        delete cur;
        --size;
    }

    void clear() {
        while (!isEmpty()) pop_front();
    }

    T& operator[](int index) {
        if (index < 0 || index >= size) throw out_of_range("DLL index out of range");
        Node<T>* cur = head;
        for (int i = 0; i < index; ++i) cur = cur->next;
        return cur->data;
    }

    const T& operator[](int index) const {
        if (index < 0 || index >= size) throw out_of_range("DLL index out of range (const)");
        Node<T>* cur = head;
        for (int i = 0; i < index; ++i) cur = cur->next;
        return cur->data;
    }

    Node<T>* getHead() { return head; }
    const Node<T>* getHead() const { return head; }
};
