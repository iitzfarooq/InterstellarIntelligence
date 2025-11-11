#pragma once

#include <cmath> 
#include <numeric>
#include <limits>
#include <concepts>
#include "matrix.h"

inline constexpr float PI = 3.14159265358979323846f;
inline constexpr float EPS = 1e-6f;
inline constexpr float DEG2RAD = PI / 180.0f;
inline constexpr float RAD2DEG = 180.0f / PI;
inline constexpr float G = 6.67430e-11f; // Gravitational constant
inline constexpr float INF = std::numeric_limits<float>::infinity();

// Clamp
template <typename T>
requires std::totally_ordered<T>
inline T clamp(T value, T min_val, T max_val) {
    return std::max(min_val, std::min(value, max_val));
}

inline bool floatEquals(float a, float b, float epsilon = EPS) {
    return std::fabs(a - b) < epsilon;
}

inline float safeDiv(float n, float d, float fallback = 0.0f) {
    return (std::fabs(d) < EPS) ? fallback : (n / d);
}

inline float degToRad(float degrees) {
    return degrees * DEG2RAD;
}

inline float radToDeg(float radians) {
    return radians * RAD2DEG;
}

/**
 * Linearly interpolates between a and b by t.
 * Pre: t in [0, 1].
 * Post: returns a when t == 0, b when t == 1. else linearly interpolated value.
 */
inline float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

/**
 * Inverse linear interpolation.
 * Post: returns t in [0, 1] such that lerp(a, b, t) == v.
 */
inline float inverseLerp(float a, float b, float v) {
    if (floatEquals(a, b)) {
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
    if (floatEquals(norm, 0.0f)) {
        throw std::invalid_argument("Cannot normalize zero vector.");
    }

    return vec * (1.0f / norm);
}

/**
 * Element-wise mathematical functions for Matrix.
 */

inline Matrix round(const Matrix& mat) {
    Matrix result(mat.rows(), mat.cols());
    for (std::size_t i = 0; i < mat.rows(); ++i) {
        for (std::size_t j = 0; j < mat.cols(); ++j) {
            result(i, j) = std::round(mat(i, j));
        }
    }
    return result;
}

inline Matrix ceil(const Matrix& mat) {
    Matrix result(mat.rows(), mat.cols());
    for (std::size_t i = 0; i < mat.rows(); ++i) {
        for (std::size_t j = 0; j < mat.cols(); ++j) {
            result(i, j) = std::ceil(mat(i, j));
        }
    }
    return result;
}

inline Matrix floor(const Matrix& mat) {
    Matrix result(mat.rows(), mat.cols());
    for (std::size_t i = 0; i < mat.rows(); ++i) {
        for (std::size_t j = 0; j < mat.cols(); ++j) {
            result(i, j) = std::floor(mat(i, j));
        }
    }
    return result;
}

inline float normp(Matrix vec, int p = 2) {
    float sum = 0.0f;
    for (std::size_t i = 0; i < vec.rows(); ++i) {
        for (std::size_t j = 0; j < vec.cols(); ++j) {
            sum += std::pow(std::fabs(vec(i, j)), p);
        }
    }
    return std::pow(sum, 1.0f / p);
}

inline float dot(Matrix a, Matrix b) {
    return (a.T() * b)(0, 0);
}

inline Matrix clamp(const Matrix& mat, float min_val, float max_val) {
    Matrix result(mat.rows(), mat.cols());
    for (std::size_t i = 0; i < mat.rows(); ++i) {
        for (std::size_t j = 0; j < mat.cols(); ++j) {
            result(i, j) = clamp(mat(i, j), min_val, max_val);
        }
    }
    return result;
}


inline float clampAngleDeg(float angle) {
    angle = std::fmod(angle, 360.0f);
    if (angle < 0.0f) {
        angle += 360.0f;
    }
    return angle;
}

inline float clampAngleRad(float angle) {
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
inline float angle(Matrix vec) {
    if (vec.rows() != 2 || vec.cols() != 1) {
        throw std::invalid_argument("Angle can only be computed for 2D column vectors.");
    }
    return std::atan2(vec(1, 0), vec(0, 0));
}