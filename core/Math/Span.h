#pragma once
#include "stdlibInclude.h"

template<class T>
class Span {
public:
    Span(T* ptr = nullptr, size_t size = 0): ptr_(ptr), size_(size) {}

    T* begin() const { return ptr_; }
    T* end() const { return ptr_ + size_; }

    T& operator[](size_t i) const { return ptr_[i]; }

    size_t size() const { return size_; }
private:
    T* ptr_;
    size_t size_;
};