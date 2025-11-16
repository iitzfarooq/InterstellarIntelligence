#pragma once

#include <vector>
#include <memory>
#include <set>
#include <functional>
#include <compare>
#include <utility>

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
};

template <>
struct std::hash<StateVertex> {
    size_t operator()(const StateVertex& sv) const {
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
        const MathConfig& math_config
    );
    
    virtual ~ActionModel() = default;

    virtual shared_vec<Action> enumerate(
        const StateVertex& from
    ) const = 0;

    virtual StateVertex apply(
        const StateVertex& from, std::shared_ptr<Action> action
    ) = 0;

    virtual bool applicable(
        const StateVertex& from, std::shared_ptr<Action> action
    ) const = 0;

protected:
    const EnvironmentModel& env_model_;
    const TimePolicy& time_policy_;
    const WorldIndex& world_index_;
    const MathConfig& math_config_;
};
