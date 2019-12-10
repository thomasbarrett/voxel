#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <initializer_list>

namespace voxel {

/**
 * A Vector represents a n-dimensional vector of uniform data type.
 * 
 * Vector is implemented as a 
 * 
 */
template <typename T, int n> struct Vector {
public:
    T buffer_[n];

    Vector() {
        memset(&buffer_, 0, sizeof(T) * n);
    }

    Vector(const Vector &v) = default;

    template <typename... U> Vector(U... ts) : buffer_{static_cast<T>(ts)...} {}

    T& operator[](int i) {
        return this->buffer_[i];
    }

    Vector add(const Vector &b) {
        Vector result;
        for (int i = 0; i < n; i++) {
            result[i] = this[i] + b[i];
        }
        return result;
    }

    Vector subtract(const Vector &b) {
        Vector result;
        for (int i = 0; i < n; i++) {
            result[i] = this[i] - b[i];
        }
        return result;
    }

    Vector multiply(const Vector &b) {
        Vector result;
        for (int i = 0; i < n; i++) {
            result[i] = this[i] * b[i];
        }
        return result;
    }

    Vector divide(const Vector &b) {
        Vector result;
        for (int i = 0; i < n; i++) {
            result[i] = this[i] / b[i];
        }
        return result;
    }

} ;

}

#endif /* VECTOR_HPP */