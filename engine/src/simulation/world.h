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
#include "simulation/strategies.h"

/**
 * Defines the world in which the simulation takes place.
 * Contains celestial bodies, wormholes, and artifacts.
 */
class WorldData {
public:
    WorldData(
        std::vector<const CelestialBody> bodies,
        std::vector<const WormHole> wormholes,
        std::vector<const Artifact> artifacts
    );

    /**
     * Returns the entities of the specified type.
     * If id is provided (not -1), returns only the entity with that id.
     * Elsewise, returns all entities of that type.
     */

    std::span<const CelestialBody> bodies(i32 id = -1) const;
    std::span<const WormHole> wormholes(i32 id = -1) const;
    std::span<const Artifact> artifacts(i32 id = -1) const;

private:
    std::vector<const CelestialBody> bodies_;
    std::vector<const WormHole> wormholes_;
    std::vector<const Artifact> artifacts_;
};

class EnvironmentModel {
public:
    EnvironmentModel(const WorldData& world_data);

    /**
     * Returns the gravitational acceleration at the given position and global time.
     * Pre: position is a 2x1 matrix representing (x, y) coordinates.
     * Post: returns a 2x1 matrix representing the (gx, gy) components
     */
    virtual Matrix gravity(
        const Matrix& position, float t_u
    ) const;
    /**
     * Returns the gravitational potential at the given position and global time.
     * Pre: position is a 2x1 matrix representing (x, y) coordinates
     * Post: returns the gravitational potential (a scalar)
     */
    virtual float potential(
        const Matrix& position, float t_u
    ) const;
    /**
     * Returns the time dilation factor (gamma) at the given position, velocity, and global time.
     * Pre: position and velocity are 2x1 matrices representing (x, y) and (vx, vy) respectively.
     * Post: returns the time dilation factor (a scalar) (dt_proper / dt_global)
     */
    virtual float gamma(
        const Matrix& position, const Matrix& velocity, float t_u
    ) const;

protected:
    const WorldData& world_data_;
};

class WorldIndex {
public:
    WorldIndex(const WorldData& world_data);

    /**
     * Returns entities within the specified radius of the given position at time t_u.
     * Pre: position is a 2x1 matrix representing (x, y) coordinates
     * Post: returns spans of entities within the radius
     */
    
    virtual std::span<const CelestialBody> queryCelestials(
        const Matrix& position,
        float radius, 
        float t_u
    );

    virtual std::span<const WormHole> queryWormHoles(
        const Matrix& position,
        float radius,
        float t_u
    );

    virtual std::span<const Artifact> queryArtifacts(
        const Matrix& position,
        float radius,
        float t_u
    );

protected:
    const WorldData& world_data_;
};

class TimePolicy {
public:
    TimePolicy(const EnvironmentModel& env_model, float tmax, float dt_u = 1.0f);

    virtual float toProper(
        float t_u, const Matrix& position, const Matrix& velocity
    ) const;
    virtual float toGlobal(
        float t_p, const Matrix& position, const Matrix& velocity
    ) const;
    
    virtual float tmax() const;
    inline float dtu() const { return dt_u_; }

protected:
    float dt_u_;
    float tmax_;

    const EnvironmentModel& env_model_;
};

struct ShipFrame {
    Matrix x, v;
    float fuel;
    float t_p; 
    std::set<int> collected_artifacts;
};

struct BodyFrame {
    int id;
    Matrix x;
    float radius;
    float mass;
};

struct WormHoleFrame {
    int id;
    Matrix entry;
    Matrix exit;
    float t_open;
    float t_close;
};

struct ArtifactFrame {
    int id;
    Matrix position;
};

struct WorldFrame {
    float t_u;
    ShipFrame ship;
    std::vector<BodyFrame> bodies;
    std::vector<WormHoleFrame> wormholes;
    std::vector<ArtifactFrame> artifacts;
};