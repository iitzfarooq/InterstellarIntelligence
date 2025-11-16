#include "world.h"

// ---------------- WorldData ----------------

WorldData::WorldData(
    shared_vec<CelestialBody> bodies,
    shared_vec<WormHole> wormholes,
    shared_vec<Artifact> artifacts
) : bodies_(std::move(bodies)),
    wormholes_(std::move(wormholes)),
    artifacts_(std::move(artifacts)) {}

const shared_vec<CelestialBody>& WorldData::bodies(i32 id) const {
    if (id == -1) {
        return bodies_;
    }
    static shared_vec<CelestialBody> result;
    result.clear();

    auto it = std::find_if(
        bodies_.begin(), bodies_.end(),
        [id](const std::shared_ptr<CelestialBody>& body) {
            return body->id == static_cast<u32>(id);
        }
    );
    if (it != bodies_.end()) {
        result.push_back(*it);
    }
    return result;
}

const shared_vec<WormHole>& WorldData::wormholes(i32 id) const {
    if (id == -1) {
        return wormholes_;
    }
    static shared_vec<WormHole> result;
    result.clear();

    auto it = std::find_if(
        wormholes_.begin(), wormholes_.end(),
        [id](const std::shared_ptr<WormHole>& wh) {
            return wh->id == static_cast<u32>(id);
        }
    );
    if (it != wormholes_.end()) {
        result.push_back(*it);
    }
    return result;
}

const shared_vec<Artifact>& WorldData::artifacts(i32 id) const {
    if (id == -1) {
        return artifacts_;
    }
    static shared_vec<Artifact> result;
    result.clear();

    auto it = std::find_if(
        artifacts_.begin(), artifacts_.end(),
        [id](const std::shared_ptr<Artifact>& art) {
            return art->id == static_cast<u32>(id);
        }
    );
    if (it != artifacts_.end()) {
        result.push_back(*it);
    }
    return result;
}

// ---------------- EnvironmentModel ----------------

EnvironmentModel::EnvironmentModel(
    const WorldData& world_data, const MathConfig& math_config
) : world_data_(world_data), math_config_(math_config) {}

// ------------------- WorldIndex -------------------

WorldIndex::WorldIndex(const WorldData& world_data, const MathConfig& math_config)
    : world_data_(world_data), math_config_(math_config) {}

// ------------------- TimePolicy -------------------

TimePolicy::TimePolicy(
    const EnvironmentModel& env_model, f64 tmax, f64 dt_u
) : env_model_(env_model), tmax_(tmax), dt_u_(dt_u) {}



/**
 * Reference implementations of abstract classes.
 */

namespace ref {

// ---------------- ConcreteEnvironment ----------------

ConcreteEnvironment::ConcreteEnvironment(
    const WorldData& world_data, const MathConfig& math_config
)   : ConcreteEnvironment::EnvironmentModel(world_data, math_config) {}

Matrix ConcreteEnvironment::gravity(const Matrix& position, f64 t_u) const {
    Matrix a(2, 1, 0.0f);
    auto r = position;

    for (const auto& body : world_data_.bodies()) {
        auto ri = body->pos(t_u);
        auto Ri = ri - r;
        auto d = math_config_.normp(Ri, 2);
        auto inv_d = math_config_.epsilonDiv(1.0f, d*d*d);
        a = a + Ri * (math_config_.G * body->mass * inv_d);
    }

    return a;
}

f64 ConcreteEnvironment::potential(const Matrix& position, f64 t_u) const {
    f64 phi = 0.0f;
    auto r = position;

    for (const auto& body : world_data_.bodies()) {
        auto ri = body->pos(t_u);
        auto Ri = ri - r;
        auto d = math_config_.normp(Ri, 2);
        phi += math_config_.epsilonDiv(math_config_.G * body->mass, d);
    }

    return phi * -1.0f;
}

f64 ConcreteEnvironment::gamma(const Matrix& position, const Matrix& velocity, f64 t_u) const {
    auto v2 = math_config_.dot(velocity, velocity);
    auto phi = potential(position, t_u);
    auto c2 = math_config_.c * math_config_.c;

    return 1.0 / (1.0 + phi / c2 - v2 / (2.0 * c2));
}

f64 ConcreteEnvironment::invGamma(const Matrix& position, const Matrix& velocity, f64 t_u) const {
    auto v2 = math_config_.dot(velocity, velocity);
    auto phi = potential(position, t_u);
    auto c2 = math_config_.c * math_config_.c;

    return 1.0 + phi / c2 - v2 / (2.0 * c2);
}

// ---------------- NaiveWorldIndex ----------------

NaiveWorldIndex::NaiveWorldIndex(
    const WorldData& world_data, const MathConfig& math_config
)   : NaiveWorldIndex::WorldIndex(world_data, math_config) {}

const shared_vec<CelestialBody> NaiveWorldIndex::queryCelestials(
    const Matrix& position, f64 radius, f64 t_u
) const {
    shared_vec<CelestialBody> result;
    for (const auto& body : world_data_.bodies()) {
        auto body_pos = body->pos(t_u);
        if (math_config_.normp(body_pos - position, 2) <= radius) {
            result.push_back(body);
        }
    }
    return result;
}

const shared_vec<WormHole> NaiveWorldIndex::queryWormHoles(
    const Matrix& position, f64 radius, f64 t_u
) const {
    shared_vec<WormHole> result;
    for (const auto& wh : world_data_.wormholes()) {
        auto entry_pos = wh->entry;
        if (math_config_.normp(entry_pos - position, 2) <= radius) {
            result.push_back(wh);
        }
    }
    return result;
}

const shared_vec<Artifact> NaiveWorldIndex::queryArtifacts(
    const Matrix& position, f64 radius, f64 t_u
) const {
    shared_vec<Artifact> result;
    for (const auto& art : world_data_.artifacts()) {
        auto art_pos = art->position;
        if (math_config_.normp(art_pos - position, 2) <= radius) {
            result.push_back(art);
        }
    }
    return result;
}

// ---------------- SimpleTimePolicy ----------------

SimpleTimePolicy::SimpleTimePolicy(
    const EnvironmentModel& env_model, f64 tmax, f64 dt_u
) : SimpleTimePolicy::TimePolicy(env_model, tmax, dt_u) {}

f64 SimpleTimePolicy::toProper(f64 dt_u, const Matrix& position, const Matrix& velocity, f64 t_u) const {
    f64 dt_p = 0.0;
    f64 step = 0.01;

    for (f64 t = t_u; t < t_u + dt_u; t += step) {
        auto g = env_model_.invGamma(position, velocity, t);
        dt_p += step * g;
    }

    return dt_p;
}

f64 SimpleTimePolicy::toGlobal(f64 dt_p, const Matrix& position, const Matrix& velocity, f64 t_u) const {
    f64 dt_u = 0.0;
    f64 step = 0.01;

    for (f64 t = t_u; dt_u < dt_p; t += step) {
        auto g = env_model_.gamma(position, velocity, t);
        dt_u += step * g;
    }

    return dt_u;
}

} 