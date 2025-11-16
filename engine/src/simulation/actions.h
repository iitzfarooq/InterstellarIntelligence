#pragma once

#include <vector>
#include <memory>
#include <set>
#include <functional>
#include <compare>
#include <utility>
#include <optional>
#include <stdexcept>
#include <ranges>

#include "simulation/models.h"
#include "simulation/world.h"
#include "utils/types.h"
#include "utils/matrix.h"
#include "utils/math.h"
#include "utils/helpers.h"

// -----------------------------------------------------------------
// --------------------- StateVertex Definition --------------------
// -----------------------------------------------------------------

struct StateVertex {
    const Matrix x;
    const Matrix v;
    const f64 t_u;
    const f64 fuel;
    const std::set<u32> collected_artifacts;

    const auto operator<=>(const StateVertex& other) const = default;

    inline bool isValid() const {
        return (x.rows() == 2 && x.cols() == 1) &&
               (v.rows() == 2 && v.cols() == 1) &&
               (fuel >= 0.0f);
    }

    StateVertex(
        const Matrix& position, const Matrix& velocity,
        f64 t_u, f64 fuel,
        const std::set<u32>& collected_artifacts = {}
    );
};

template <>
struct std::hash<StateVertex> {
    inline size_t operator()(const StateVertex& sv) const {
        size_t h = 0;
        auto mat_hasher = std::hash<Matrix>();
        auto flt_hasher = std::hash<f64>();
        auto uint_hasher = std::hash<u32>();

        hash_combine(h, mat_hasher(sv.x));
        hash_combine(h, mat_hasher(sv.v));
        hash_combine(h, flt_hasher(sv.t_u));
        hash_combine(h, flt_hasher(sv.fuel));
        for (const auto& artifact_id : sv.collected_artifacts) {
            hash_combine(h, uint_hasher(artifact_id));
        }
        return h;
    }
};

// ----------------------------------------------------------------
// -------------- Action and ActionModel Definitions --------------
// ----------------------------------------------------------------

struct Action {
    virtual f64 cost() const = 0;
    virtual ~Action() = default;
};

class ActionModel {
public:
    ActionModel(
        const EnvironmentModel& env_model,
        const TimePolicy& time_policy,
        const WorldIndex& world_index,
        const WorldData& world_data,
        const MathConfig& math_config
    );
    
    virtual ~ActionModel() = default;

    virtual shared_vec<Action> enumerate(
        const StateVertex& from
    ) const = 0;

    virtual std::optional<StateVertex> apply(
        const StateVertex& from, std::shared_ptr<Action> action
    ) = 0;

protected:
    const EnvironmentModel& env_model_;
    const TimePolicy& time_policy_;
    const WorldIndex& world_index_;
    const WorldData& world_data_;
    const MathConfig& math_config_;
};

// --------------------- Thrust Actions ---------------------

struct ThrustAction : public Action {
    const f64 thrust_level;
    const Matrix direction; // normalized 2x1 matrix
    const f64 dt_global;
    const MathConfig& math_config;

    ThrustAction(
        f64 thrust_level, f64 dt_global, 
        const Matrix& direction, const MathConfig& config
    );

    inline f64 cost() const override {
        return dt_global; // simple time cost for now. Using dt_global.
    }
};

class ThrustActionModel : public ActionModel {
public:
    ThrustActionModel(
        const EnvironmentModel& env_model,
        const TimePolicy& time_policy,
        const WorldIndex& world_index,
        const WorldData& world_data,
        const MathConfig& math_config,
        const Spacecraft& spacecraft,
        const std::vector<f64>& possible_directions // in radians
    );
    
    virtual ~ThrustActionModel() = default;

    virtual shared_vec<Action> enumerate(
        const StateVertex& from
    ) const override;

    virtual std::optional<StateVertex> apply(
        const StateVertex& from, std::shared_ptr<Action> action
    ) override;

private:
    const Spacecraft& spacecraft_;
    const std::vector<f64> possible_directions_;

private:
    struct State {
        Matrix x, v;
        f64 fuel, t_global;

        State() : x(2, 1), v(2, 1), fuel(0.0f), t_global(0.0f) {}
        State(const Matrix& x, const Matrix& v, f64 fuel, f64 t_global)
            : x(x), v(v), fuel(fuel), t_global(t_global) {}

        State operator+(const State& other) const {
            return {
                x + other.x,
                v + other.v,
                fuel + other.fuel,
                t_global + other.t_global
            };
        }

        State operator*(f64 scalar) const {
            return {
                x * scalar,
                v * scalar,
                fuel * scalar,
                t_global * scalar
            };
        }
    };

    State findNewState(
        const StateVertex& from,
        const ThrustAction& ptr
    ) const;

    const std::set<u32> artifactsHere(
        const Matrix& position,
        f64 t_u
    ) const;

    const bool detectCollision(
        const Matrix& position,
        f64 t_u
    ) const;
};