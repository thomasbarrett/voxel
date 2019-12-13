#ifndef VOXEL_ARRAY_HPP
#define VOXEL_ARRAY_HPP

/**
 * \file Array.hpp
 * \brief A generic constant size array implementation 
 * \author Thomas Barrett <tbarrett@caltech.edu>s
 * \date Dec 12, 2019
 */

namespace voxel {

/**
 * A generic constant size array implementation.
 */
template <typename T, int n> struct Array {
private:
    T buffer_[n] = {0};

public:

    /**
     * Constructs an array with the given elements.
     * \param E... Es: the elements to put in the array
     */
    template <typename... E> Array(E... Es) : buffer_{static_cast<T>(Es)...} {}

    /**
     * Returns a reference to the element at index i
     * \param i: the index to reference.
     * \returns a reference to the ith element.
     */
    T& operator[](int i) {
        return this->buffer_[i];
    }

    /**
     * Returns a reference to the element at index i
     * \param i: the index to reference.
     * \returns a reference to the ith element.
     */
    const T& operator[](int i) const {
        return this->buffer_[i];
    }
};

}

#endif /* VOXEL_ARRAY_HPP */