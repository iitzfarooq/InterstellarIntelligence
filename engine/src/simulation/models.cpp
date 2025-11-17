#include "models.h"

Entity::Entity(u32 id) : id(id) {}

WormHole::WormHole(
    u32 id, const Matrix &entry, const Matrix &exit, f64 t_open, f64 t_close
)   : Entity(id), entry(entry), exit(exit), t_open(t_open), t_close(t_close) {
    req(t_open < t_close, "WormHole t_open must be less than t_close.");
    auto [m1, n1] = entry.shape();
    auto [m2, n2] = exit.shape();
    req(m1 == 2 && n1 == 1, "WormHole entry must be a 2x1 matrix.");
    req(m2 == 2 && n2 == 1, "WormHole exit must be a 2x1 matrix.");
}

CelestialBody::CelestialBody(u32 id, f64 radius, f64 mass)
: Entity(id), radius(radius), mass(mass) {
    req(radius > 0.0f, "CelestialBody radius must be positive.");
    req(mass > 0.0f, "CelestialBody mass must be positive.");
}

StationaryBody::StationaryBody(
    u32 id, f64 radius, f64 mass, const Matrix &position
)   : CelestialBody(id, radius, mass), position(position) {}

OrbitingBody::OrbitingBody(
    u32 id, f64 radius, f64 mass, std::unique_ptr<const TrajectoryStrategy> strategy
)   : CelestialBody(id, radius, mass), trajectory_strategy(std::move(strategy)) {
        req(trajectory_strategy != nullptr, "OrbitingBody requires a valid TrajectoryStrategy.");
}

EllipticalOrbit::EllipticalOrbit(
    f64 a, f64 b,
    f64 omega,
    f64 phi,
    const Matrix &center,
    f64 angle
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

Spacecraft::Spacecraft(
    u32 id, f64 mass, f64 fuel, f64 min_fuel_to_land,
        const std::vector<f64>& thrust_levels, f64 exhaust_velocity
)   : Entity(id), mass(mass), fuel(fuel), min_fuel_to_land(min_fuel_to_land), 
    thrust_levels(thrust_levels), exhaust_velocity(exhaust_velocity) {
        
    req(mass > 0.0f, "Spacecraft mass must be positive.");
    req(fuel >= 0.0f, "Spacecraft fuel cannot be negative.");
    req(!thrust_levels.empty(), "Spacecraft thrust_levels cannot be empty.");
    for (const auto& level : thrust_levels) {
        req(level >= 0.0f, "Each thrust level must be non-negative.");
    }
    req(exhaust_velocity > 0.0f, "Spacecraft exhaust_velocity must be positive.");
}