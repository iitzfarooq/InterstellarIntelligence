#include "actions.h"

// ------------------- StateVertex Definition -------------------

StateVertex::StateVertex(
    const Matrix& position, const Matrix& velocity,
    f64 t_u, f64 fuel,
    const uset<u32>& collected_artifacts
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
    const WorldData& world_data
) : env_model_(env_model),
    time_policy_(time_policy),
    world_index_(world_index),
    world_data_(world_data)
{}

// ------------------- ThrustAction and ThrustActionModel -------------------

ThrustAction::ThrustAction(
    f64 thrust_level, f64 dt_global, 
    const Matrix& direction
) : thrust_level(thrust_level),
    direction(direction),
    dt_global(dt_global)
{}

ThrustActionModel::ThrustActionModel(
    const EnvironmentModel& env_model,
    const TimePolicy& time_policy,
    const WorldIndex& world_index,
    const WorldData& world_data,
    const Spacecraft& spacecraft,
    const std::vector<f64>& possible_directions
) : ActionModel(env_model, time_policy, world_index, world_data),
    spacecraft_(spacecraft), possible_directions_(possible_directions)
{}

Matrix direction(const StateVertex& from) {
    auto v = MathConfig::normp(from.v, 2);
    Matrix forward(2, 1, {1.0f, 0.0f});

    if (!MathConfig::floatEquals(v, 0.0f)) {
        forward = MathConfig::normalized(from.v);
    } 

    return forward;
}

shared_vec<Action> ThrustActionModel::enumerate(
    const StateVertex& from
) const {
    shared_vec<Action> actions;
    Matrix forward = direction(from);
    Matrix d = Matrix::toHomogeneous(forward);

    for (auto&& angle : possible_directions_) {
        Matrix dir = Matrix::fromHomogeneous(Matrix::rotate2d(angle) * d);

        for (auto&& thrust_level : spacecraft_.thrust_levels) {
            auto action = std::make_shared<ThrustAction>(
                thrust_level, time_policy_.dtu(), dir
            );
            actions.push_back(action);
        }
        
    }

    auto action = std::make_shared<ThrustAction>(
        0.0f, time_policy_.dtu(), Matrix::fromHomogeneous(d)
    );
    actions.push_back(action);

    return actions;
}

bool ThrustActionModel::checkConstraints(
    const StateVertex& state
) const {
    return !detectCollision(state.x, state.t_u) &&
            (state.isValid()) &&
            (state.t_u <= time_policy_.tmax()) &&
            (world_data_.max_radius() >= MathConfig::normp(state.x, 2));

    return true;
}

std::optional<StateVertex> ThrustActionModel::apply(
    const StateVertex& from, std::shared_ptr<Action> action
) {
    auto ptr = std::dynamic_pointer_cast<ThrustAction>(action);
    if (!ptr) {
        return std::nullopt;
    }

    auto s_new = findIntState(from, *ptr);
    
    auto x         = s_new.x;
    auto v         = s_new.v;
    auto t_u       = s_new.t_u;
    auto fuel      = MathConfig::clamp(s_new.fuel, 0.0f);
    auto artifacts = from.collected_artifacts | artifactsHere(x, t_u);
    
    StateVertex new_state(x, v, t_u, fuel, artifacts);
    if (checkConstraints(new_state)) {
        return new_state;
    }
    
    return std::nullopt;
}

// Helper methods for ThrustActionModel

ThrustActionModel::IntState ThrustActionModel::findIntState(
    const StateVertex& from,
    const ThrustAction& ptr
) const {
    // We will use the RK4 integrator from MathConfig to compute the new state. 
    // there are three ODEs to integrate: position, velocity, and fuel.
    auto deriv = [&] (
        const IntState& s, 
        f64 /* tau offset. But the system is autonomous */
    ) {
        IntState ds;
        
        auto y = env_model_.gamma(s.x, s.v, s.t_u);    // dt_u / dτ
        auto total_mass = spacecraft_.mass + s.fuel;
        
        
        Matrix a_g = env_model_.gravity(s.x, s.t_u);   // dv/dt_u
        Matrix a_th = Matrix(2, 1, 0.0f);              // F/m 
        
        if (s.fuel > 0.0f) {
            a_th = ptr.direction * (ptr.thrust_level / total_mass);
        }
        
        ds.x = s.v * y;                                // dx/dτ = v * (dt_u/dτ)
        ds.v = (a_g + a_th) * y;                       // dv/dτ = (dv/dt_u) (dt_u/dτ)
        ds.fuel = MathConfig::safeDiv(
            -ptr.thrust_level, spacecraft_.exhaust_velocity, 0.0f
        );                                             // dfuel/dτ
        ds.t_u = y;                                    // dt_u/dτ
        
        return ds;
    };
    
    auto dt_prop = time_policy_.toProper(
        ptr.dt_global, from.x, from.v, from.t_u
    );

    IntState s_new = MathConfig::rk4Integrate<IntState>(
        IntState(from.x, from.v, from.fuel, from.t_u),
        0.0f,
        dt_prop,
        deriv
    );
    
    return s_new;
}

const uset<u32> ThrustActionModel::artifactsHere(
    const Matrix& position,
    f64 t_u
) const {
    auto artifacts_here = world_index_.queryArtifacts(
        position, MathConfig::epsilon, t_u
    );
    
    auto view = artifacts_here | std::views::transform(
        [] (const std::shared_ptr<Artifact>& artifact) {
            return artifact->id;
        }
    );

    return uset<u32>(view.begin(), view.end());
}

bool ThrustActionModel::detectCollision(
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
        auto dist = MathConfig::normp(position - body_pos, 2);
        if (dist <= body->radius) {
            return true;
        }
    }

    return false;
}