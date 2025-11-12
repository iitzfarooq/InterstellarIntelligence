#pragma once

#include <cmath>
#include <cstdint>
#include <memory>
#include <string>
#include <variant>
#include "utils/matrix.h"
#include "utils/math.h"
#include "utils/helpers.h"
#include "utils/types.h"
#include "simulation/strategies.h"

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
 * Artifact entity implementation.
 * Rep-inv: position is a 2x1 matrix representing fixed (x, y) coordinates.
 */
struct Artifact : Entity {
    const Matrix position;

    Artifact(u32 id, const Matrix& position);

    inline Matrix pos(float t) const { return position; }
};