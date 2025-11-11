#pragma once

#include <vector>
#include <cstdint>
#include <stdexcept>
#include <functional>
#include <cmath>
#include <utility>

/**
 * A simple matrix class for basic linear algebra operations. 
 * Represents a matrix of floats with m rows and n columns.
 * Abstraction function:
 * Matrix(m, n, fill) represents an m x n matrix where each element is initialized to 'fill'. Requires m,n > 0.
 * The element at row i and column j is accessed via operator()(i, j).
 * where 0 <= i < m and 0 <= j < n.
 */
class Matrix {
public:
    Matrix(std::size_t rows, std::size_t cols, float fill = 0.0f);

    virtual float& operator()(std::size_t i, std::size_t j);
    virtual const float& operator()(std::size_t i, std::size_t j) const;

    virtual inline std::size_t rows() const { return m; }
    virtual inline std::size_t cols() const { return n; }

    inline std::pair<std::size_t, std::size_t> shape() const {
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
    float trace() const;

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

private:
    /**
     * Representation invariant:
     * - data_ is a flat vector storing matrix elements in row-major order.
     * - data_.size() == m * n, m -> rows, n -> columns
     */

    std::vector<float> data_;
    std::size_t m, n;
};

Matrix scale(const Matrix& mat, float scalar);
Matrix add(const Matrix& a, const Matrix& b);
Matrix mul(const Matrix& a, const Matrix& b);

Matrix operator*(const Matrix& mat, float scalar);
Matrix operator+(const Matrix& a, const Matrix& b);
Matrix operator*(const Matrix& a, const Matrix& b);

template <>
struct std::hash<Matrix> {
    inline std::size_t operator()(const Matrix& mat) const {
        return mat.hash();
    }
};

Matrix eye(std::size_t size);
Matrix zero(std::size_t rows, std::size_t cols);

/**
 * Given a 2x1 matrix, convert it to a 3x1 homogeneous coordinate matrix.
 */
Matrix toHomogeneous(const Matrix& mat);

/**
 * Given a 3x1 homogeneous coordinate matrix, convert it to a 2x1 matrix.
 */
Matrix fromHomogeneous(const Matrix& mat);

/* ****************************************************************
 * Following functions create 3x3 affine transformation matrices.
 *************************************************************** */

Matrix translate2d(float tx, float ty);
Matrix rotate2d(float angle_rad);
Matrix scale2d(float sx, float sy);