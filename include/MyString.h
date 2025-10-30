#pragma once
#include <cstddef>
#include <cstring>
using namespace std;
// Minimal custom string (ASCII/UTF-8 bytes) without using string
class MyString {
private:
    char* data_;
    size_t size_;
    size_t cap_;

    void reserve(size_t newCap) {
        if (newCap <= cap_) return;
        char* n = new char[newCap];
        if (data_) memcpy(n, data_, size_);
        n[size_] = '\0';
        delete[] data_;
        data_ = n;
        cap_ = newCap;
    }

public:
    MyString() : data_(nullptr), size_(0), cap_(0) {
        reserve(16);
        data_[0] = '\0';
    }

    MyString(const char* s) : data_(nullptr), size_(0), cap_(0) {
        if (!s) { reserve(16); data_[0] = '\0'; return; }
        size_t n = strlen(s);
        reserve(n + 1);
        memcpy(data_, s, n + 1);
        size_ = n;
    }

    MyString(const MyString& other) : data_(nullptr), size_(0), cap_(0) {
        reserve(other.size_ + 1);
        memcpy(data_, other.data_, other.size_ + 1);
        size_ = other.size_;
    }

    MyString(MyString&& other) noexcept : data_(other.data_), size_(other.size_), cap_(other.cap_) {
        other.data_ = nullptr; other.size_ = 0; other.cap_ = 0;
    }

    ~MyString() { delete[] data_; }

    MyString& operator=(const MyString& other) {
        if (this == &other) return *this;
        reserve(other.size_ + 1);
        memcpy(data_, other.data_, other.size_ + 1);
        size_ = other.size_;
        return *this;
    }

    MyString& operator=(MyString&& other) noexcept {
        if (this == &other) return *this;
        delete[] data_;
        data_ = other.data_; size_ = other.size_; cap_ = other.cap_;
        other.data_ = nullptr; other.size_ = 0; other.cap_ = 0;
        return *this;
    }

    const char* c_str() const { return data_; }
    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

    void clear() { size_ = 0; if (data_) data_[0] = '\0'; }

    void push_back(char c) {
        if (size_ + 1 >= cap_) reserve(cap_ ? cap_ * 2 : 16);
        data_[size_++] = c;
        data_[size_] = '\0';
    }

    void append(const char* s) {
        if (!s) return;
        size_t n = strlen(s);
        if (size_ + n + 1 > cap_) reserve(size_ + n + 1);
        memcpy(data_ + size_, s, n + 1);
        size_ += n;
    }

    bool operator==(const MyString& rhs) const {
        if (size_ != rhs.size_) return false;
        return memcmp(data_, rhs.data_, size_) == 0;
    }

    bool operator!=(const MyString& rhs) const { return !(*this == rhs); }
};

