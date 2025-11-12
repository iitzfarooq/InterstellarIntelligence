#pragma once

#include "utils/matrix.h"
#include "utils/types.h"

/**
 * Abstract base class for orbit strategies.
 * Defines the interface for computing the position of an object in orbit at time t.
 */
struct OrbitStrategy {
    /**
     * Returns the position of the object in orbit at time t.
     * Postcondition: returns a 2x1 matrix representing the (x, y) coordinates.
     */
    virtual Matrix pos(float t) const = 0;
    virtual ~OrbitStrategy() = default;
};


/**
 * Elliptical orbit strategy implementation.
 * Defines an elliptical orbit based on semi-major axis (a), semi-minor axis (b),
 * angular velocity (omega), phase shift (phi), center position, and rotation angle.
 * 
 * AF(a, b, omega, phi, center, angle): 
 *   x = a * cos(omega * t + phi)
 *   y = b * sin(omega * t + phi)
 *   rotate2d(angle) * [x; y; 1] + center
 */
struct EllipticalOrbit : public OrbitStrategy {
    const float a, b, omega, phi;
    const Matrix center;
    const float angle;

    /**
     * Rep-inv: 
     *   a, b, omega > 0; center is a 2x1 matrix; angle in radians;
     *   angle is in [0, 2π)
     */

    EllipticalOrbit(float a, float b, float omega, float phi, const Matrix& center, float angle);

    /**
     * Returns the position of the object in elliptical orbit at time t.
     * Pre: None
     * Post: returns a 2x1 matrix representing the (x, y) coordinates.
     */
    inline Matrix pos(float t) const override {
        float x = a * std::cos(omega * t + phi);
        float y = b * std::sin(omega * t + phi);

        Matrix point(3, 1, 1.0f);
        point(0, 0) = x;
        point(1, 0) = y;

        Matrix rotated = rotate2d(angle) * point;
        Matrix translated = center + fromHomogeneous(rotated);
        return translated;
    }
};