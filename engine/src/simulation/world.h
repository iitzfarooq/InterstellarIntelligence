#pragma once

#include <vector>
#include <string>
#include <span>
#include <ranges>
#include <memory>
#include <algorithm>

#include "utils/types.h"
#include "utils/math.h"
#include "utils/matrix.h"
#include "utils/helpers.h"
#include "simulation/models.h"

/**
 * Defines the world in which the simulation takes place.
 * Contains celestial bodies, wormholes, and artifacts.
 */
class WorldData {
public:
    WorldData(
        shared_vec<CelestialBody> bodies,
        shared_vec<WormHole> wormholes,
        shared_vec<Artifact> artifacts
    );

    /**
     * Returns the entities of the specified type.
     * If id is provided (not -1), returns only the entity with that id.
     * Elsewise, returns all entities of that type.
     */

    const shared_vec<CelestialBody>& bodies(i32 id = -1) const;
    const shared_vec<WormHole>& wormholes(i32 id = -1) const;
    const shared_vec<Artifact>& artifacts(i32 id = -1) const;

private:
    shared_vec<CelestialBody> bodies_;
    shared_vec<WormHole> wormholes_;
    shared_vec<Artifact> artifacts_;
};

/**
 * Models the environmental effects in the world, such as gravity and time dilation.
 */
class EnvironmentModel {
public:
    EnvironmentModel(const WorldData& world_data, const MathConfig& math_config);

    /**
     * Returns the gravitational acceleration at the given position and global time.
     * Pre: position is a 2x1 matrix representing (x, y) coordinates.
     * Post: returns a 2x1 matrix representing the (gx, gy) components
     */
    virtual Matrix gravity(
        const Matrix& position, f64 t_u
    ) const = 0;
    /**
     * Returns the gravitational potential at the given position and global time.
     * Pre: position is a 2x1 matrix representing (x, y) coordinates
     * Post: returns the gravitational potential (a scalar)
     */
    virtual f64 potential(
        const Matrix& position, f64 t_u
    ) const = 0;
    /**
     * Returns the time dilation factor (gamma) at the given position, velocity, and global time.
     * Pre: position and velocity are 2x1 matrices representing (x, y) and (vx, vy) respectively.
     * Post: returns the time dilation factor (a scalar) (dt_global / dt_proper)
     */
    virtual f64 gamma(
        const Matrix& position, const Matrix& velocity, f64 t_u
    ) const = 0;

    /**
     * Returns the inverse of the time dilation factor (1 / gamma).
     * Pre: position and velocity are 2x1 matrices representing (x, y) and (vx, vy) respectively.
     * Post: returns the inverse time dilation factor (a scalar) (dt_proper / dt_global)
     */
    virtual f64 invGamma(
        const Matrix& position, const Matrix& velocity, f64 t_u
    ) const = 0;

    virtual ~EnvironmentModel() = default;

protected:
    const WorldData& world_data_;
    const MathConfig& math_config_;
};

/**
 * Spatial index for efficient querying of entities in the world.
 */
class WorldIndex {
public:
    WorldIndex(const WorldData& world_data, const MathConfig& math_config);

    /**
     * Returns entities within the specified radius of the given position at time t_u.
     * Pre: position is a 2x1 matrix representing (x, y) coordinates
     * Post: returns spans of entities within the radius
     */
    
    virtual const shared_vec<CelestialBody> queryCelestials(
        const Matrix& position,
        f64 radius, 
        f64 t_u
    ) const = 0;
    
    virtual const shared_vec<WormHole> queryWormHoles(
        const Matrix& position,
        f64 radius,
        f64 t_u
    ) const = 0;
    
    virtual const shared_vec<Artifact> queryArtifacts(
        const Matrix& position,
        f64 radius,
        f64 t_u
    ) const = 0;

    virtual ~WorldIndex() = default;

protected:
    const WorldData& world_data_;
    const MathConfig& math_config_;
};

/**
 * Manages time conversions between global and proper time in the world.
 */
class TimePolicy {
public:
    TimePolicy(const EnvironmentModel& env_model, f64 tmax, f64 dt_u = 1.0f);

    virtual f64 toProper(
        f64 dt_u, const Matrix& position, const Matrix& velocity, f64 t_u
    ) const = 0;
    virtual f64 toGlobal(
        f64 dt_p, const Matrix& position, const Matrix& velocity, f64 t_u
    ) const = 0;
    
    inline virtual f64 tmax() const { return tmax_; }
    inline virtual f64 dtu() const { return dt_u_; }

    virtual ~TimePolicy() = default;

protected:
    f64 dt_u_;
    f64 tmax_;

    const EnvironmentModel& env_model_;
};

struct ShipFrame {
    Matrix x, v;
    f64 fuel;
    f64 t_p; 
    std::set<int> collected_artifacts;
};

struct BodyFrame {
    int id;
    Matrix x;
    f64 radius;
    f64 mass;
};

struct WormHoleFrame {
    int id;
    Matrix entry;
    Matrix exit;
    f64 t_open;
    f64 t_close;
};

struct ArtifactFrame {
    int id;
    Matrix position;
};

struct WorldFrame {
    f64 t_u;
    ShipFrame ship;
    std::vector<BodyFrame> bodies;
    std::vector<WormHoleFrame> wormholes;
    std::vector<ArtifactFrame> artifacts;
};

/**
 * The namespace contains reference implementations of the above abstract classes. 
 */
namespace ref {

class ConcreteEnvironment : public ::EnvironmentModel {
public:
    explicit ConcreteEnvironment(const WorldData&, const MathConfig&);

    Matrix gravity(
        const Matrix& position, f64 t_u
    ) const override;
    f64 potential(
        const Matrix& position, f64 t_u
    ) const override;
    f64 gamma(
        const Matrix& position, const Matrix& velocity, f64 t_u
    ) const override;
    f64 invGamma(
        const Matrix& position, const Matrix& velocity, f64 t_u
    ) const override;
};

class NaiveWorldIndex : public ::WorldIndex {
public:
    explicit NaiveWorldIndex(
        const WorldData& world_data, const MathConfig& math_config
    );

    const shared_vec<CelestialBody> queryCelestials(
        const Matrix& position, f64 radius, f64 t_u
    ) const override;
    const shared_vec<WormHole> queryWormHoles(
        const Matrix& position, f64 radius, f64 t_u
    ) const override;
    const shared_vec<Artifact> queryArtifacts(
        const Matrix& position, f64 radius, f64 t_u
    ) const override;
};

class SimpleTimePolicy : public ::TimePolicy {
public:
    SimpleTimePolicy(const EnvironmentModel& env_model, f64 tmax, f64 dt_u = 1.0f);

    f64 toProper(
        f64 dt_u, const Matrix& position, const Matrix& velocity, f64 t_u
    ) const override;
    f64 toGlobal(
        f64 dt_p, const Matrix& position, const Matrix& velocity, f64 t_u
    ) const override;
};

} 