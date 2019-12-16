#ifndef VOXEL_TENSOR_HPP
#define VOXEL_TENSOR_HPP

/**
 * \file Matrix.hpp
 * \brief Contains definitions for a 4-dimensional matrix.
 * \author Thomas Barrett <tbarrett@caltech.edu>s
 * \date Dec 13, 2019
 */
#include <util/Array.hpp>

namespace voxel {

/**
 * 4-dimension matrix implementation.
 * 
 */
struct Matrix: Array<Array<float, 4>, 4> {
public:

    Matrix() = default;

    constexpr Matrix(
        Array<float, 4> r1,
        Array<float, 4> r2,
        Array<float, 4> r3,
        Array<float, 4> r4
    ): Array<Array<float, 4>, 4>{r1, r2, r3, r4} {}

    static Matrix identity() {
        return {
            {1, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 1},
        };
    }

    static Matrix Translate(const voxel::Array<float, 3> &x) {
        return {
            {1, 0, 0, x[0]},
            {0, 1, 0, x[1]},
            {0, 0, 1, x[2]},
            {0, 0, 0, 1},
        };
    }

    static Matrix Scale(const voxel::Array<float, 3> &x) {
        return {
            {x[0], 0, 0, 0},
            {0, x[1], 0, 0},
            {0, 0, x[2], 0},
            {0, 0, 0, 1},
        };
    }
    
    static Matrix RotateX(float theta) {
        float c = cos(theta);
        float s = sin(theta);
        return {
            {1, 0, 0, 0},
            {0, c, -s, 0},
            {0, s, c, 0},
            {0, 0, 0, 1},
        };
    }

    static Matrix RotateY(float theta) {
        float c = cos(theta);
        float s = sin(theta);
        return {
            {c, 0, s, 0},
            {0, 1, 0, 0},
            {-s, 0, c, 0},
            {0, 0, 0, 1},
        };
    }

    static Matrix RotateZ(float theta) {
        float c = cos(theta);
        float s = sin(theta);
        return {
            {c, -s, 0, 0},
            {s, c, 0, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 1},
        };
    }
    
    Matrix operator*(const Matrix &b) const {
        Matrix res;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                res[i][j] = 0; 
                for (int k = 0; k < 4; k++) {
                    res[i][j] += (*this)[k][j] * b[i][k]; 
                }
            } 
        }
        return res;
    }

    Matrix tranpose() const {
        Matrix res;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                res[i][j] = (*this)[j][i]; 
            } 
        }
        return res;
    }

};

}

#endif /* VOXEL_TENSOR_HPP */