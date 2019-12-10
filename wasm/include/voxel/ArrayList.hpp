#ifndef ARRAY_LIST_HPP
#define ARRAY_LIST_HPP
#define _LIBCPP_HAS_NO_THREADS 1
#include <libc/stdlib.h>
#include <initializer_list>

namespace voxel {

template <typename T> class ArrayList {
private:
    unsigned int size_ = 0;
    unsigned int capacity_ = 0;
    T *buffer_ = 0;

public:

    ArrayList() {
        capacity_ = 16;
        size_ = 0;
        buffer_ = (T*) malloc(sizeof(T) * capacity_);;
    }

    ArrayList(ArrayList &&list) {
        capacity_ = list.capacity_;
        size_ = list.size_;
        buffer_ = list.buffer_;
        list.buffer_ = NULL;
    }

    ArrayList(const ArrayList &list) {
        capacity_ = list.capacity_;
        size_ = list.size_;
        buffer_ = malloc(sizeof(T) * capacity_);
        for (int i = 0; i < size_; i++) {
            buffer_[i] = list.buffer_[i];
        }
    }

    void clear() {
        size_ = 0;
    }

    ArrayList &operator= ( const ArrayList & ) = default;	

    ArrayList(std::initializer_list<T> Es) {
        capacity_ = 64;
        size_ = 0;
        buffer_ = (T*) malloc(sizeof(T) * 64);
        for (T e: Es) {
            buffer_[size_] = e;
            size_ += 1;
        }
    }

    T& operator[](int i) {
        return buffer_[i];
    }

    unsigned int size() {
        return size_;
    }

    T* buffer() {
        return buffer_;
    }

    void append(const T &e) {
        if (size_ == capacity_) {
            capacity_ *= 2;
            buffer_ = (T *) realloc((void*) buffer_, sizeof(T) * capacity_);
        }
        
        buffer_[size_] = e;
        size_ += 1;
    }

    ~ArrayList() {
        free(buffer_);
    }
};

};

#endif /* ARRAY_LIST_HPP */