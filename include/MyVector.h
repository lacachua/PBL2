#pragma once
#include <cstddef>
#include <new>

// Minimal custom vector without using std::vector
template <typename T>
class MyVector {
private:
    T* data_;
    std::size_t size_;
    std::size_t cap_;

    void reallocate(std::size_t newCap) {
        if (newCap <= cap_) return;
        T* n = static_cast<T*>(::operator new(sizeof(T) * newCap));
        for (std::size_t i = 0; i < size_; ++i) new (&n[i]) T(static_cast<T&&>(data_[i]));
        for (std::size_t i = 0; i < size_; ++i) data_[i].~T();
        ::operator delete(data_);
        data_ = n;
        cap_ = newCap;
    }

public:
    MyVector() : data_(nullptr), size_(0), cap_(0) {}
    ~MyVector() {
        clear();
        ::operator delete(data_);
    }

    MyVector(const MyVector& other) : data_(nullptr), size_(0), cap_(0) {
        if (other.size_) {
            reallocate(other.size_);
            for (std::size_t i = 0; i < other.size_; ++i) new (&data_[i]) T(other.data_[i]);
            size_ = other.size_;
        }
    }

    MyVector& operator=(const MyVector& other) {
        if (this == &other) return *this;
        clear();
        if (other.size_ > cap_) { ::operator delete(data_); data_ = nullptr; cap_ = 0; reallocate(other.size_); }
        for (std::size_t i = 0; i < other.size_; ++i) new (&data_[i]) T(other.data_[i]);
        size_ = other.size_;
        return *this;
    }

    void push_back(const T& v) {
        if (size_ >= cap_) reallocate(cap_ ? cap_ * 2 : 4);
        new (&data_[size_++]) T(v);
    }

    void push_back(T&& v) {
        if (size_ >= cap_) reallocate(cap_ ? cap_ * 2 : 4);
        new (&data_[size_++]) T(static_cast<T&&>(v));
    }

    void removeAt(std::size_t index) {
        if (index >= size_) return;
        data_[index].~T();
        for (std::size_t i = index; i + 1 < size_; ++i) {
            new (&data_[i]) T(static_cast<T&&>(data_[i+1]));
            data_[i+1].~T();
        }
        --size_;
    }

    void clear() {
        for (std::size_t i = 0; i < size_; ++i) data_[i].~T();
        size_ = 0;
    }

    std::size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

    T& operator[](std::size_t i) { return data_[i]; }
    const T& operator[](std::size_t i) const { return data_[i]; }
};

