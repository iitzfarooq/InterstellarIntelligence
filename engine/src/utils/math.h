#pragma once

#include <cmath> 
#include <numeric>
#include <limits>
#include <concepts>
#include "utils/matrix.h"
#include "utils/types.h"

/**
 * The configuration struct for math utilities.
 * Provides constants, helpers, tolerances, and settings for mathematical operations.
 */
struct MathConfig {
    static constexpr f64 epsilon = 1e-12f; 
    static constexpr f64 pi = 3.14159265358979323846f;
    static constexpr f64 infinity = std::numeric_limits<f64>::infinity();
    static constexpr f64 G = 6.67430e-11f * 1e-9f; // G in km^3 kg^-1 s^-2
    static constexpr f64 c = 299792.458f; // Speed of light in km/s
    static constexpr f64 DEG2RAD = pi / 180.0f;
    static constexpr f64 RAD2DEG = 180.0f / pi;

    MathConfig() = default;
    virtual ~MathConfig() = default;

    // Common numerical operations

    static inline f64 degToRad(f64 degrees)  {
        return degrees * MathConfig::DEG2RAD;
    }
    
    static inline f64 radToDeg(f64 radians)  {
        return radians * MathConfig::RAD2DEG;
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

    static inline f64 round(f64 value)  {
        return std::round(value);
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

    // operations related to general matrices.

    static inline Matrix round(const Matrix& mat)  {
        Matrix result(mat.rows(), mat.cols());
        for (size_t i = 0; i < mat.rows(); ++i) {
            for (size_t j = 0; j < mat.cols(); ++j) {
                result(i, j) = std::round(mat(i, j));
            }
        }
        return result;
    }

    // more to be added later.

};