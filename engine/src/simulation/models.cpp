#include "models.h"

Entity::Entity(u32 id) : id(id) {}

WormHole::WormHole(
    u32 id, const Matrix &entry, const Matrix &exit, float t_open, float t_close
)   : Entity(id), entry(entry), exit(exit), t_open(t_open), t_close(t_close) {
    req(t_open < t_close, "WormHole t_open must be less than t_close.");
    auto [m1, n1] = entry.shape();
    auto [m2, n2] = exit.shape();
    req(m1 == 2 && n1 == 1, "WormHole entry must be a 2x1 matrix.");
    req(m2 == 2 && n2 == 1, "WormHole exit must be a 2x1 matrix.");
}

CelestialBody::CelestialBody(u32 id, float radius, float mass)
: Entity(id), radius(radius), mass(mass) {
    req(radius > 0.0f, "CelestialBody radius must be positive.");
    req(mass > 0.0f, "CelestialBody mass must be positive.");
}

StationaryBody::StationaryBody(
    u32 id, float radius, float mass, const Matrix &position
)   : CelestialBody(id, radius, mass), position(position) {}

OrbitingBody::OrbitingBody(
    u32 id, float radius, float mass, std::unique_ptr<const OrbitStrategy> strategy
)   : CelestialBody(id, radius, mass), orbit_strategy(std::move(strategy)) {
        req(orbit_strategy != nullptr, "OrbitingBody requires a valid OrbitStrategy.");
}

EllipticalOrbit::EllipticalOrbit(
    float a, float b,
    float omega,
    float phi,
    const Matrix &center,
    float angle
)   : center(center), a(a), b(b), omega(omega), phi(phi), angle(angle) {
    req(a > 0.0f, "EllipticalOrbit semi-major axis a must be positive.");
    req(b > 0.0f, "EllipticalOrbit semi-minor axis b must be positive.");
    req(omega > 0.0f, "EllipticalOrbit angular velocity omega must be positive.");
    auto [m, n] = center.shape();
    req(m == 2 && n == 1, "EllipticalOrbit center must be a 2x1 matrix.");
    req(angle >= 0.0f && angle < 2*PI, "EllipticalOrbit angle must be in [0, 2π).");
}

Artifact::Artifact(u32 id, const Matrix &position) 
    : Entity(id), position(position) {

    auto [m, n] = position.shape();
    req(m == 2 && n == 1, "Artifact position must be a 2x1 matrix.");
}