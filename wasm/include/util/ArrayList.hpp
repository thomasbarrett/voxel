#ifndef ARRAY_LIST_HPP
#define ARRAY_LIST_HPP

/**
 * \file Array.hpp
 * \brief A generic array-list implementation
 * \author Thomas Barrett <tbarrett@caltech.edu>s
 * \date Dec 12, 2019
 */

#include <libc/stdlib.hpp>


namespace voxel {

/**
 * A generic array-list implementation.
 */
template <typename T> class ArrayList {
private:
    const static unsigned int DEFAULT_CAPACITY = 16;
    unsigned int size_ = 0;
    unsigned int capacity_ = 0;
    T *buffer_ = 0;

public:

    /**
     * Constructs an ArrayList with an initial capacity of the default size.
     */
    ArrayList() {
        capacity_ = DEFAULT_CAPACITY;
        size_ = 0;
        buffer_ = (T*) malloc(sizeof(T) * capacity_);;
    }

    /**
     * Move constructor.
     * Performs a shallow copy of all fields and sets the buffer_ field of the
     * original object to nullptr to prevent it from being freed. The initial
     * ArrayList is unusable after this constructor is called.
     */
    ArrayList(ArrayList &&list) {
        capacity_ = list.capacity_;
        size_ = list.size_;
        buffer_ = list.buffer_;
        list.buffer_ = nullptr;
    }

    /**
     * Copy constructor.
     * Copies all fields. A new buffer_ field is allocated with the same
     * capacity of the original ArrayList. All elements from the original
     * ArrayList are copied into the buffer.
     */
    ArrayList(const ArrayList &list) {
        capacity_ = list.capacity_;
        size_ = list.size_;
        buffer_ = (T*) malloc(sizeof(T) * capacity_);
        for (int i = 0; i < size_; i++) {
            buffer_[i] = list.buffer_[i];
        }
    }

    /**
     * Assignment Operator.
     * Default assignment operator calls custom copy constructor.
     */
    ArrayList &operator=(const ArrayList &) = default;	

    /**
     * Removes all elements from the ArrayList.
     * This operation is implemented by simply setting the 'size_' field to
     * zero indicating that all old value can be overwritten. Before this is
     * done, the destructor for each element is called.
     */
    void clear() {
        for (int i = 0; i < size_; i++) {
            buffer_[i].~T();
        }
        size_ = 0;
    }

    T* begin() {
        return &buffer_[0];
    }

    T* end() {
        return &buffer_[size_];
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

    /**
     * Appends an element to the list.
     * If there is enoughg capacity to insert the element, then the element
     * is inserted at the end of the buffer and the `size_` field is
     * incremented. Otherwise, the `buffer_` is reallocated with double the
     * capacity. 
     */
    void append(const T &e) {
        if (size_ == capacity_) {
            capacity_ *= 2;
            buffer_ = (T *) realloc((void*) buffer_, sizeof(T) * capacity_);
        }
        
        buffer_[size_] = (T &&) e;
        size_ += 1;
    }

    /**
     * Deconstructor.
     * Calls the deconstructors of all elements in the buffer_ and frees the
     * buffer memory.
     */
    ~ArrayList() {
        for (int i = 0; i < size_; i++) {
            buffer_[i].~T();
        }
        free(buffer_);
    }
};

};

#endif /* ARRAY_LIST_HPP */