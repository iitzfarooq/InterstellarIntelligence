#pragma once

#include <cmath> 
#include <numeric>
#include <limits>
#include <concepts>
#include "matrix.h"
#include "types.h"

inline constexpr f64 PI = 3.14159265358979323846f;
inline constexpr f64 EPS = 1e-6f;
inline constexpr f64 DEG2RAD = PI / 180.0f;
inline constexpr f64 RAD2DEG = 180.0f / PI;
inline constexpr f64 G = 6.67430e-11f; // Gravitational constant
inline constexpr f64 C = 299792458.0f; // Speed of light in m/s
inline constexpr f64 INF = std::numeric_limits<f64>::infinity();

// Clamp
template <typename T>
requires std::totally_ordered<T>
inline T clamp(T value, T min_val, T max_val) {
    return std::max(min_val, std::min(value, max_val));
}

inline bool f64Equals(f64 a, f64 b, f64 epsilon = EPS) {
    return std::fabs(a - b) < epsilon;
}

inline f64 safeDiv(f64 n, f64 d, f64 fallback = 0.0f) {
    return (std::fabs(d) < EPS) ? fallback : (n / d);
}

inline f64 degToRad(f64 degrees) {
    return degrees * DEG2RAD;
}

inline f64 radToDeg(f64 radians) {
    return radians * RAD2DEG;
}

/**
 * Linearly interpolates between a and b by t.
 * Pre: t in [0, 1].
 * Post: returns a when t == 0, b when t == 1. else linearly interpolated value.
 */
inline f64 lerp(f64 a, f64 b, f64 t) {
    return a + t * (b - a);
}

/**
 * Inverse linear interpolation.
 * Post: returns t in [0, 1] such that lerp(a, b, t) == v.
 */
inline f64 inverseLerp(f64 a, f64 b, f64 v) {
    if (f64Equals(a, b)) {
        return 0.0f; // Avoid division by zero
    }
    return (v - a) / (b - a);
}

/**
 * Normalizes a vector represented as a Matrix.
 * Precondition: vec must be a column matrix (n x 1) or row matrix (1 x n).
 * Throws std::invalid_argument if the vector is zero.
 */
inline Matrix normalized(Matrix vec) {
    auto norm = std::sqrt((vec.T() * vec)(0, 0));
    if (f64Equals(norm, 0.0f)) {
        throw std::invalid_argument("Cannot normalize zero vector.");
    }

    return vec * (1.0f / norm);
}

/**
 * Element-wise mathematical functions for Matrix.
 */

inline Matrix round(const Matrix& mat) {
    Matrix result(mat.rows(), mat.cols());
    for (size_t i = 0; i < mat.rows(); ++i) {
        for (size_t j = 0; j < mat.cols(); ++j) {
            result(i, j) = std::round(mat(i, j));
        }
    }
    return result;
}

inline Matrix ceil(const Matrix& mat) {
    Matrix result(mat.rows(), mat.cols());
    for (size_t i = 0; i < mat.rows(); ++i) {
        for (size_t j = 0; j < mat.cols(); ++j) {
            result(i, j) = std::ceil(mat(i, j));
        }
    }
    return result;
}

inline Matrix floor(const Matrix& mat) {
    Matrix result(mat.rows(), mat.cols());
    for (size_t i = 0; i < mat.rows(); ++i) {
        for (size_t j = 0; j < mat.cols(); ++j) {
            result(i, j) = std::floor(mat(i, j));
        }
    }
    return result;
}

inline f64 normp(Matrix M, int p = 2) {
    f64 sum = 0.0f;
    for (size_t i = 0; i < M.rows(); ++i) {
        for (size_t j = 0; j < M.cols(); ++j) {
            sum += std::pow(std::fabs(M(i, j)), p);
        }
    }
    return std::pow(sum, 1.0f / p);
}

inline f64 dot(Matrix a, Matrix b) {
    return (a.T() * b)(0, 0);
}

inline bool vecEquals(Matrix a, Matrix b, f64 epsilon = EPS) {
    if (a.rows() != b.rows() || a.cols() != b.cols()) {
        return false;
    }
    for (size_t i = 0; i < a.rows(); ++i) {
        for (size_t j = 0; j < a.cols(); ++j) {
            if (!f64Equals(a(i, j), b(i, j), epsilon)) {
                return false;
            }
        }
    }
    return true;
}

inline Matrix clamp(const Matrix& mat, f64 min_val, f64 max_val) {
    Matrix result(mat.rows(), mat.cols());
    for (size_t i = 0; i < mat.rows(); ++i) {
        for (size_t j = 0; j < mat.cols(); ++j) {
            result(i, j) = clamp(mat(i, j), min_val, max_val);
        }
    }
    return result;
}


inline f64 clampAngleDeg(f64 angle) {
    angle = std::fmod(angle, 360.0f);
    if (angle < 0.0f) {
        angle += 360.0f;
    }
    return angle;
}

inline f64 clampAngleRad(f64 angle) {
    angle = std::fmod(angle, 2.0f * PI);
    if (angle < 0.0f) {
        angle += 2.0f * PI;
    }
    return angle;
}

/**
 * Computes the angle (in radians) of a 2D vector represented as a Matrix.
 * Pre: vec in R^2 (2x1 matrix).
 * Post: returns angle in radians in range [-pi, pi].
 */
inline f64 angle(Matrix vec) {
    if (vec.rows() != 2 || vec.cols() != 1) {
        throw std::invalid_argument("Angle can only be computed for 2D column vectors.");
    }
    return std::atan2(vec(1, 0), vec(0, 0));
}