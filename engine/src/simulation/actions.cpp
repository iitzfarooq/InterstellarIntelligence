#include "actions.h"

// ------------------- StateVertex Definition -------------------

StateVertex::StateVertex(
    const Matrix& position, const Matrix& velocity,
    f64 t_u, f64 fuel,
    const std::set<u32>& collected_artifacts
) : x(position),
    v(velocity),
    t_u(t_u),
    fuel(fuel),
    collected_artifacts(collected_artifacts)
{}

// ------------------- ActionModel Definition -------------------

ActionModel::ActionModel(
    const EnvironmentModel& env_model,
    const TimePolicy& time_policy,
    const WorldIndex& world_index,
    const WorldData& world_data,
    const MathConfig& math_config
) : env_model_(env_model),
    time_policy_(time_policy),
    world_index_(world_index),
    world_data_(world_data),
    math_config_(math_config)
{}

// ------------------- ThrustAction and ThrustActionModel -------------------

ThrustAction::ThrustAction(
    f64 thrust_level, f64 dt_global, 
    const Matrix& direction, const MathConfig& config
) : thrust_level(thrust_level),
    direction(direction),
    dt_global(dt_global),
    math_config(config)
{}

ThrustActionModel::ThrustActionModel(
    const EnvironmentModel& env_model,
    const TimePolicy& time_policy,
    const WorldIndex& world_index,
    const WorldData& world_data,
    const MathConfig& math_config,
    const Spacecraft& spacecraft,
    const std::vector<f64>& possible_directions
) : ActionModel(env_model, time_policy, world_index, world_data, math_config),
    spacecraft_(spacecraft), possible_directions_(possible_directions)
{}

shared_vec<Action> ThrustActionModel::enumerate(
    const StateVertex& from
) const {
    shared_vec<Action> actions;
    Matrix forward(2, 1, 1.0f);

    if (math_config_.floatEquals(
        math_config_.normp(from.v, 2), 0.0f)) {
        forward = Matrix(2, 1, 1.0f); forward(1, 0) = 0.0f;
    } else {
        forward = math_config_.normalized(from.v);
    }
    Matrix d = toHomogeneous(forward);


    for (auto&& angle : possible_directions_) {
        Matrix dir = fromHomogeneous(rotate2d(angle) * d);
        for (auto&& thrust_level : spacecraft_.thrust_levels) {
            auto action = std::make_shared<ThrustAction>(
                thrust_level, time_policy_.dtu(), dir, math_config_
            );
            actions.push_back(action);
        }
    }

    auto action = std::make_shared<ThrustAction>(
        0.0f, time_policy_.dtu(), d, math_config_
    );
    actions.push_back(action);

    return actions;
}

std::optional<StateVertex> ThrustActionModel::apply(
    const StateVertex& from, std::shared_ptr<Action> action
) {
    auto ptr = std::dynamic_pointer_cast<ThrustAction>(action);
    if (!ptr) {
        return std::nullopt;
    }

    auto s_new = findNewState(from, *ptr);
    
    auto x = s_new.x;
    auto v = s_new.v;
    auto t_u = from.t_u + ptr->dt_global;
    auto fuel = s_new.fuel;
    auto artifacts = from.collected_artifacts | artifactsHere(x, t_u);
    
    StateVertex new_state(x, v, t_u, fuel, artifacts);
    if (
        !new_state.isValid() ||  
        detectCollision(x, t_u) ||
        t_u > time_policy_.tmax()
    ) {
        return std::nullopt;
    }

    return new_state;
}

// Helper methods for ThrustActionModel

ThrustActionModel::State ThrustActionModel::findNewState(
    const StateVertex& from,
    const ThrustAction& ptr
) const {
    auto dt_prop = time_policy_.toProper(
        ptr.dt_global, from.x, from.v, from.t_u
    );

    // We will use the RK4 integrator from MathConfig to compute the new state. 
    // there are three ODEs to integrate: position, velocity, and fuel.

    auto deriv = [&] (const State& s, f64 tau) {
        State ds;

        f64 t_global = from.t_u + time_policy_.toGlobal(
            tau, s.x, s.v, from.t_u
        );

        Matrix a_grav = env_model_.gravity(s.x, t_global);
        Matrix a_thrust = ptr.direction * (ptr.thrust_level / (spacecraft_.mass + s.fuel));
        f64 dfuel_dtau = math_config_.safeDiv(
            -ptr.thrust_level,
            spacecraft_.exhaust_velocity,
            0.0f
        );

        ds.x = s.v;
        ds.v = a_grav + a_thrust;
        ds.fuel = dfuel_dtau;
        
        return ds;
    };

    State s_new = MathConfig::integrate<State>(
        State(from.x, from.v, from.fuel),
        0.0f,
        dt_prop,
        deriv
    );
    
    return s_new;
}

const std::set<u32> ThrustActionModel::artifactsHere(
    const Matrix& position,
    f64 t_u
) const {
    auto artifacts_here = world_index_.queryArtifacts(
        position, math_config_.epsilon, t_u
    );
    
    auto view = artifacts_here | std::views::transform(
        [] (const std::shared_ptr<Artifact>& artifact) {
            return artifact->id;
        }
    );

    return std::set<u32>(view.begin(), view.end());
}

const bool ThrustActionModel::detectCollision(
    const Matrix& position,
    f64 t_u
) const {
    auto bodies = world_data_.bodies();
    auto it = std::ranges::max_element(bodies, {}, 
        [] (const std::shared_ptr<CelestialBody>& body) {
            return body->radius;
        }
    );

    auto max_radius = (it != bodies.end()) ? (*it)->radius : 0.0f;

    auto nearby_bodies = world_index_.queryCelestials(
        position, max_radius + 1.0f, t_u
    );

    for (const auto& body : nearby_bodies) {
        auto body_pos = body->pos(t_u);
        auto dist = math_config_.normp(position - body_pos, 2);
        if (dist <= body->radius) {
            return true;
        }
    }

    return false;
}