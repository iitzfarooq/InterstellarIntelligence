#pragma once

#include "utils/matrix.h"
#include "utils/types.h"

/**
 * Abstract base class for trajectory strategies.
 * Defines the interface for computing the position of an object in trajectory at time t.
 */
struct TrajectoryStrategy {
    /**
     * Returns the position of the object in trajectory at time t.
     * Postcondition: returns a 2x1 matrix representing the (x, y) coordinates.
     */
    virtual Matrix pos(f64 t) const = 0;

    /**
     * Returns the velocity of the object in trajectory at time t.
     * Postcondition: returns a 2x1 matrix representing the (vx, vy) components.
     */
    inline virtual Matrix vel(f64 t, f64 delta = 0.001f) const {
        Matrix pos1 = pos(t);
        Matrix pos2 = pos(t + delta);
        return (pos2 - pos1) * (1.0f / delta);
    }

    virtual ~TrajectoryStrategy() = default;
};


/**
 * Elliptical trajectory strategy implementation.
 * Defines an elliptical trajectory based on semi-major axis (a), semi-minor axis (b),
 * angular velocity (omega), phase shift (phi), center position, and rotation angle.
 * 
 * AF(a, b, omega, phi, center, angle): 
 *   x = a * cos(omega * t + phi)
 *   y = b * sin(omega * t + phi)
 *   Matrix::rotate2d(angle) * [x; y; 1] + center
 */
struct EllipticalOrbit : public TrajectoryStrategy {
    const f64 a, b, omega, phi;
    const Matrix center;
    const f64 angle;

    /**
     * Rep-inv: 
     *   a, b, omega > 0; center is a 2x1 matrix; angle in radians;
     *   angle is in [0, 2π)
     */

    EllipticalOrbit(f64 a, f64 b, f64 omega, f64 phi, const Matrix& center, f64 angle);

    /**
     * Returns the position of the object in elliptical trajectory at time t.
     * Pre: None
     * Post: returns a 2x1 matrix representing the (x, y) coordinates.
     */
    inline Matrix pos(f64 t) const override {
        f64 x = a * std::cos(omega * t + phi);
        f64 y = b * std::sin(omega * t + phi);

        Matrix point(3, 1, 1.0f);
        point(0, 0) = x;
        point(1, 0) = y;

        Matrix rotated = Matrix::rotate2d(angle) * point;
        Matrix translated = center + Matrix::fromHomogeneous(rotated);
        return translated;
    }
};