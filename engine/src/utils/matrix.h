#pragma once

#include <vector>
#include <cstdint>
#include <stdexcept>
#include <functional>
#include <cmath>
#include <utility>
#include <compare>
#include <initializer_list>
#include <vector>

#include "utils/types.h"
#include "utils/helpers.h"

/**
 * A simple matrix class for basic linear algebra operations. 
 * Represents a matrix of f64s with m rows and n columns.
 * Abstraction function:
 * Matrix(m, n, fill) represents an m x n matrix where each element is initialized to 'fill'. Requires m,n > 0.
 * The element at row i and column j is accessed via operator()(i, j).
 * where 0 <= i < m and 0 <= j < n.
 */
class Matrix {
public:
    Matrix(size_t rows, size_t cols, f64 fill = 0.0f);
    Matrix(size_t rows, size_t cols, const std::initializer_list<f64>& values);
    Matrix(size_t rows, size_t cols, const std::vector<f64>& values);

    virtual f64& operator()(size_t i, size_t j);
    virtual const f64& operator()(size_t i, size_t j) const;

    virtual inline size_t rows() const { return m; }
    virtual inline size_t cols() const { return n; }

    inline std::pair<size_t, size_t> shape() const {
        return {m, n};
    }

    /**
     * Returns the transpose of the matrix.
     */
    Matrix T() const;
    /**
     * Returns the trace of the matrix. 
     * Precondition: matrix must be square (m == n).
     * Throws std::invalid_argument if the matrix is not square.
     */
    f64 trace() const;

    /**
     * Returns a hash value for the matrix.
     */
    size_t hash() const;

    Matrix() = delete;
    Matrix(const Matrix&) = default;
    Matrix(Matrix&&) = default;
    Matrix& operator=(const Matrix&) = default;
    Matrix& operator=(Matrix&&) = default;
    ~Matrix() = default;

    auto operator<=>(const Matrix& other) const = default;

    // ----------------------------------------------------
    // ------------------ Static Methods ------------------
    // ----------------------------------------------------

    static Matrix scale(const Matrix& mat, f64 scalar);
    static Matrix add(const Matrix& a, const Matrix& b);
    static Matrix mul(const Matrix& a, const Matrix& b);

    friend Matrix operator*(const Matrix& mat, f64 scalar);
    friend Matrix operator*(f64 scalar, const Matrix& mat);
    friend Matrix operator+(const Matrix& a, const Matrix& b);
    friend Matrix operator-(const Matrix& a, const Matrix& b);
    friend Matrix operator*(const Matrix& a, const Matrix& b);
    
    static Matrix eye(size_t size);
    static Matrix zero(size_t rows, size_t cols);
    
    /**
     * Given a 2x1 matrix, convert it to a 3x1 homogeneous coordinate matrix.
     */
    static Matrix toHomogeneous(const Matrix& mat);
    
    /**
     * Given a 3x1 homogeneous coordinate matrix, convert it to a 2x1 matrix.
     */
    static Matrix fromHomogeneous(const Matrix& mat);
    
    /* ****************************************************************
     * Following functions create 3x3 affine transformation matrices.
     *************************************************************** */
    
    static Matrix translate2d(f64 tx, f64 ty);
    static Matrix rotate2d(f64 angle_rad);
    static Matrix scale2d(f64 sx, f64 sy);

private:
    /**
     * Representation invariant:
     * - data_ is a flat vector storing matrix elements in row-major order.
     * - data_.size() == m * n, m -> rows, n -> columns
     */

    std::vector<f64> data_;
    size_t m, n;
};


template <>
struct std::hash<Matrix> {
    inline size_t operator()(const Matrix& mat) const {
        return mat.hash();
    }
};