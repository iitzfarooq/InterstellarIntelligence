#pragma once

#include <cmath>
#include <cstdint>
#include <memory>
#include <string>
#include <variant>
#include "utils/matrix.h"
#include "utils/math.h"
#include "utils/helpers.h"

using u32 = uint32_t;
using f32 = float;
using f64 = double;
using byte = uint8_t;
using i32 = int32_t;
using i64 = int64_t;

/**
 * Base class for all entities in the space simulation.
 * Each entity has a unique identifier.
 */
struct Entity {
    const u32 id;

    Entity(u32 id);
    virtual ~Entity() = default;
};

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
 * Base class for celestial bodies.
 * Each celestial body has a radius and mass, and position defined by subclasses.
 * Rep-inv: radius > 0, mass > 0. 
 */
struct CelestialBody : Entity {
    const float radius;
    const float mass;

    CelestialBody(u32 id, float radius, float mass);

    virtual Matrix pos(float t) const = 0;
    virtual ~CelestialBody() = default;
};

struct OrbitingBody : CelestialBody {
    const std::unique_ptr<const OrbitStrategy> orbit_strategy;

    OrbitingBody(
        u32 id, float radius, float mass,
        std::unique_ptr<const OrbitStrategy> strategy
    );

    inline Matrix pos(float t) const override { return orbit_strategy->pos(t); }
};

/**
 * Stationary body implementation.
 * Rep-inv: position is a 2x1 matrix representing fixed (x, y) coordinates.
 */
struct StationaryBody : CelestialBody {
    const Matrix position;

    StationaryBody(u32 id, float radius, float mass, const Matrix& position);

    inline Matrix pos(float t) const override { return position; }
};

/**
 * WormHole entity implementation.
 * Rep-inv: entry, exit in R^2 represented as 2x1 matrices; t_open < t_close.
 */
struct WormHole : Entity {
    const Matrix entry;
    const Matrix exit;
    float t_open;
    float t_close;

    WormHole(u32 id, const Matrix& entry, const Matrix& exit, float t_open, float t_close);

    inline bool isOpen(float t) const {
        return t >= t_open && t <= t_close;
    }
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

struct EllipticalParams {
    float a;
    float b;
    float omega;
    float phi;
    Matrix center;
    float angle;
};

enum class OrbitType {
    ELLIPTICAL,
    // Future orbit types can be added here
};

// Variant to hold different orbit parameter types
using OrbitParams = std::variant<EllipticalParams>;

inline std::unique_ptr<const OrbitStrategy> createOrbitStrategy(
    OrbitType type, const OrbitParams& params
) {
    switch (type) {
        case OrbitType::ELLIPTICAL: {
            const auto& p = std::get<EllipticalParams>(params);
            return std::make_unique<const EllipticalOrbit>(
                p.a, p.b, p.omega, p.phi, p.center, p.angle
            );
        }
        // Future orbit types can be handled here
        default:
            throw std::invalid_argument("Unsupported orbit type");
    }
}