#pragma once

#include <cmath> 
#include <numeric>
#include <limits>
#include <concepts>
#include "utils/matrix.h"
#include "utils/types.h"

inline constexpr f64 PI = 3.14159265358979323846f;
inline constexpr f64 EPS = 1e-12f;
inline constexpr f64 DEG2RAD = PI / 180.0f;
inline constexpr f64 RAD2DEG = 180.0f / PI;
inline constexpr f64 G = 6.67430e-11f; // Gravitational constant
inline constexpr f64 C = 299792458.0f; // Speed of light in m/s
inline constexpr f64 INF = std::numeric_limits<f64>::infinity();


/**
 * The configuration struct for math utilities.
 * Provides constants, helpers, tolerances, and settings for mathematical operations.
 */
struct MathConfig {
    static constexpr f64 epsilon = EPS; 
    static constexpr f64 pi = PI;
    static constexpr f64 infinity = INF;
    static constexpr f64 G = G;
    static constexpr f64 c = C;

    MathConfig() = default;
    virtual ~MathConfig() = default;

    // Common numerical operations

    static inline f64 degToRad(f64 degrees)  {
        return degrees * DEG2RAD;
    }
    
    static inline f64 radToDeg(f64 radians)  {
        return radians * RAD2DEG;
    }

    static inline bool floatEquals(f64 a, f64 b)  {
        return std::fabs(a - b) < epsilon;
    }

    static inline f64 safeDiv(f64 n, f64 d, f64 fallback = 0.0f)  {
        return (std::fabs(d) < epsilon) ? fallback : (n / d);
    }

    static inline f64 epsilonDiv(f64 n, f64 d)  {
        return n / (d + epsilon);
    }

    static inline f64 clampAngleDeg(f64 angle)  {
        angle = std::fmod(angle, 360.0f);
        if (angle < 0.0f) {
            angle += 360.0f;
        }
        return angle;
    }

    static inline f64 clampAngleRad(f64 angle)  {
        angle = std::fmod(angle, 2.0f * pi);
        if (angle < 0.0f) {
            angle += 2.0f * pi;
        }
        return angle;
    }

    static inline f64 clamp(f64 value, f64 min_val, f64 max_val = MathConfig::infinity)  {
        return std::max(min_val, std::min(value, max_val));
    }

    // Operations related to vectors

    static inline f64 normp(Matrix v, int p = 2)  {
        f64 sum = 0.0f;
        for (size_t i = 0; i < v.rows(); ++i) {
            sum += std::pow(std::fabs(v(i, 0)), p);
        }
        return std::pow(sum, 1.0f / p);
    }

    static inline Matrix normalized(Matrix v)  {
        auto n = normp(v, 2);
        if (MathConfig::floatEquals(n, 0.0f)) {
            throw std::invalid_argument("Cannot normalize zero vector.");
        }
        return v * (1.0f / n);
    }

    static inline f64 dot(Matrix a, Matrix b)  {
        return (a.T() * b)(0, 0);
    }

    // Numerical integration using 4th-order Runge-Kutta method

    template <typename T>
    static inline T rk4Integrate(
        const T& x0,
        double t, 
        double dt,
        const std::function<T(const T&, double)>& f
    )  {
        T k1 = f(x0, t);
        T k2 = f(x0 + k1 * (dt / 2.0), t + dt / 2.0);
        T k3 = f(x0 + k2 * (dt / 2.0), t + dt / 2.0);
        T k4 = f(x0 + k3 * dt, t + dt);

        return x0 + (k1 + k2 * 2.0 + k3 * 2.0 + k4) * (dt / 6.0);
    }

    // more to be added later.

};

// // Clamp
// template <typename T>
// requires std::totally_ordered<T>
// inline T clamp(T value, T min_val, T max_val) {
//     return std::max(min_val, std::min(value, max_val));
// }

// inline bool floatEquals(f64 a, f64 b, f64 epsilon = EPS) {
//     return std::fabs(a - b) < epsilon;
// }

// inline f64 safeDiv(f64 n, f64 d, f64 fallback = 0.0f) {
//     return (std::fabs(d) < EPS) ? fallback : (n / d);
// }

// inline f64 degToRad(f64 degrees) {
//     return degrees * DEG2RAD;
// }

// inline f64 radToDeg(f64 radians) {
//     return radians * RAD2DEG;
// }

// /**
//  * Linearly interpolates between a and b by t.
//  * Pre: t in [0, 1].
//  * Post: returns a when t == 0, b when t == 1. else linearly interpolated value.
//  */
// inline f64 lerp(f64 a, f64 b, f64 t) {
//     return a + t * (b - a);
// }

// /**
//  * Inverse linear interpolation.
//  * Post: returns t in [0, 1] such that lerp(a, b, t) == v.
//  */
// inline f64 inverseLerp(f64 a, f64 b, f64 v) {
//     if (f64Equals(a, b)) {
//         return 0.0f; // Avoid division by zero
//     }
//     return (v - a) / (b - a);
// }

// /**
//  * Normalizes a vector represented as a Matrix.
//  * Precondition: vec must be a column matrix (n x 1) or row matrix (1 x n).
//  * Throws std::invalid_argument if the vector is zero.
//  */
// inline Matrix normalized(Matrix vec) {
//     auto norm = std::sqrt((vec.T() * vec)(0, 0));
//     if (f64Equals(norm, 0.0f)) {
//         throw std::invalid_argument("Cannot normalize zero vector.");
//     }

//     return vec * (1.0f / norm);
// }

// /**
//  * Element-wise mathematical functions for Matrix.
//  */

// inline Matrix round(const Matrix& mat) {
//     Matrix result(mat.rows(), mat.cols());
//     for (size_t i = 0; i < mat.rows(); ++i) {
//         for (size_t j = 0; j < mat.cols(); ++j) {
//             result(i, j) = std::round(mat(i, j));
//         }
//     }
//     return result;
// }

// inline Matrix ceil(const Matrix& mat) {
//     Matrix result(mat.rows(), mat.cols());
//     for (size_t i = 0; i < mat.rows(); ++i) {
//         for (size_t j = 0; j < mat.cols(); ++j) {
//             result(i, j) = std::ceil(mat(i, j));
//         }
//     }
//     return result;
// }

// inline Matrix floor(const Matrix& mat) {
//     Matrix result(mat.rows(), mat.cols());
//     for (size_t i = 0; i < mat.rows(); ++i) {
//         for (size_t j = 0; j < mat.cols(); ++j) {
//             result(i, j) = std::floor(mat(i, j));
//         }
//     }
//     return result;
// }

// inline f64 normp(Matrix M, int p = 2) {
//     f64 sum = 0.0f;
//     for (size_t i = 0; i < M.rows(); ++i) {
//         for (size_t j = 0; j < M.cols(); ++j) {
//             sum += std::pow(std::fabs(M(i, j)), p);
//         }
//     }
//     return std::pow(sum, 1.0f / p);
// }

// inline f64 dot(Matrix a, Matrix b) {
//     return (a.T() * b)(0, 0);
// }

// inline bool vecEquals(Matrix a, Matrix b, f64 epsilon = EPS) {
//     if (a.rows() != b.rows() || a.cols() != b.cols()) {
//         return false;
//     }
//     for (size_t i = 0; i < a.rows(); ++i) {
//         for (size_t j = 0; j < a.cols(); ++j) {
//             if (!f64Equals(a(i, j), b(i, j), epsilon)) {
//                 return false;
//             }
//         }
//     }
//     return true;
// }

// inline Matrix clamp(const Matrix& mat, f64 min_val, f64 max_val) {
//     Matrix result(mat.rows(), mat.cols());
//     for (size_t i = 0; i < mat.rows(); ++i) {
//         for (size_t j = 0; j < mat.cols(); ++j) {
//             result(i, j) = clamp(mat(i, j), min_val, max_val);
//         }
//     }
//     return result;
// }


// inline f64 clampAngleDeg(f64 angle) {
//     angle = std::fmod(angle, 360.0f);
//     if (angle < 0.0f) {
//         angle += 360.0f;
//     }
//     return angle;
// }

// inline f64 clampAngleRad(f64 angle) {
//     angle = std::fmod(angle, 2.0f * PI);
//     if (angle < 0.0f) {
//         angle += 2.0f * PI;
//     }
//     return angle;
// }

// /**
//  * Computes the angle (in radians) of a 2D vector represented as a Matrix.
//  * Pre: vec in R^2 (2x1 matrix).
//  * Post: returns angle in radians in range [-pi, pi].
//  */
// inline f64 angle(Matrix vec) {
//     if (vec.rows() != 2 || vec.cols() != 1) {
//         throw std::invalid_argument("Angle can only be computed for 2D column vectors.");
//     }
//     return std::atan2(vec(1, 0), vec(0, 0));
// }