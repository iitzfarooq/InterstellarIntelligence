#pragma once

#include <memory>
#include <vector>
#include <ranges>
#include <functional>
#include <tuple>
#include <optional>
#include <ranges>

#include "simulation/actions.h"
#include "simulation/models.h"
#include "simulation/world.h"
#include "simulation/strategies.h"
#include "utils/types.h"
#include "utils/matrix.h"
#include "utils/math.h"
#include "utils/helpers.h"

// ------------------------------------------------------------------
// ------------------------ Discrete State --------------------------
// ------------------------------------------------------------------

struct DiscreteState {
    const Matrix qx;
    const Matrix qv;
    const f64 qt_u;
    const f64 qfuel;
    const uset<u32> collected_artifacts;

    inline DiscreteState(
        const Matrix& qx, const Matrix& qv,
        f64 qt_u, f64 qfuel,
        const uset<u32>& collected_artifacts = {}
    ) : qx(qx), qv(qv), qt_u(qt_u), qfuel(qfuel), 
        collected_artifacts(collected_artifacts) {}

    inline bool operator==(const DiscreteState& other) const {
        return (qx == other.qx) &&
               (qv == other.qv) &&
               (qt_u == other.qt_u) &&
               (qfuel == other.qfuel) &&
               (collected_artifacts == other.collected_artifacts);
    }
};

template <>
struct std::hash<DiscreteState> {
    inline size_t operator()(const DiscreteState& ds) const {
        size_t h = 0;
        auto mat_hasher = std::hash<Matrix>();
        auto flt_hasher = std::hash<f64>();
        auto uint_hasher = std::hash<u32>();

        h = hash_combine(h, mat_hasher(ds.qx));
        h = hash_combine(h, mat_hasher(ds.qv));
        h = hash_combine(h, flt_hasher(ds.qt_u));
        h = hash_combine(h, flt_hasher(ds.qfuel));

        for (const auto& artifact : ds.collected_artifacts) {
            h = hash_combine(h, uint_hasher(artifact));
        }

        return h;
    }
};

// -----------------------------------------------------------------
// -------------------------- Quantizer ----------------------------
// -----------------------------------------------------------------

struct QuantizerConfig {
    const f64 pos_bin;
    const f64 vel_bin;
    const f64 time_bin;
    const f64 fuel_bin;

    inline QuantizerConfig(
        f64 pos_bin, f64 vel_bin,
        f64 time_bin, f64 fuel_bin
    ) : pos_bin(pos_bin), vel_bin(vel_bin),
        time_bin(time_bin), fuel_bin(fuel_bin) {}
};

struct Quantizer {
    const QuantizerConfig config;

    inline Quantizer(const QuantizerConfig& config) : config(config) {}

    inline DiscreteState q(const StateVertex& sv) const {
        auto qx = MathConfig::round(sv.x * (1.0 / config.pos_bin));
        auto qv = MathConfig::round(sv.v * (1.0 / config.vel_bin));
        auto qt_u = MathConfig::round(sv.t_u / config.time_bin);
        auto qfuel = MathConfig::round(sv.fuel / config.fuel_bin);

        return DiscreteState(qx, qv, qt_u, qfuel, sv.collected_artifacts);
    }
};

// ------------------------------------------------------------------
// ---------------------------- Solver ------------------------------
// ------------------------------------------------------------------

struct StateAction {
    std::shared_ptr<StateVertex> state;
    std::shared_ptr<Action> action;

    inline StateAction(
        const std::shared_ptr<StateVertex>& state,
        const std::shared_ptr<Action>& action
    ) : state(state), action(action) {}
    
    StateAction(const StateAction& other) = default;
    StateAction(StateAction&& other) = default;
    StateAction& operator=(const StateAction& other) = default;
    StateAction& operator=(StateAction&& other) = default;
};

struct SolverResult {
    std::vector<StateAction> path;
    f64 total_cost;
};

struct Solver {
    using Strategy = GreedyStrategy<std::shared_ptr<StateVertex>>;
    using ParentMap = umap<DiscreteState, StateAction>;
    using SeenSet = uset<DiscreteState>;
    
    const Quantizer quantizer;
    const std::shared_ptr<Strategy> strategy;
    const shared_vec<ActionModel> action_models;
    
    inline Solver(
        const Quantizer& quantizer,
        const std::shared_ptr<Strategy>& strategy,
        const shared_vec<ActionModel>& action_models
    ) : strategy(strategy),
        quantizer(quantizer),
        action_models(action_models)
    {}

    std::optional<SolverResult> solve(
        const StateVertex& start,
        const std::function<bool(const StateVertex&)>& isGoal,
        f64 max_cost = MathConfig::infinity
    ) const;

private:
    std::vector<StateAction> neighbors(const StateVertex& sv) const;
    std::vector<StateAction> reconstruct(
        const StateVertex& goal, 
        const umap<DiscreteState, StateAction>& parent_map
    ) const;
    
};