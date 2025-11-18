#include "simulation.h"

namespace ref {

void ReferenceSimulation::initialize(const SimulationConfig& config) {
    config_.world_config            = config.world_config;
    config_.time_config             = config.time_config;
    config_.quantization_config     = config.quantization_config;
    config_.spacecraft_config       = config.spacecraft_config;
    config_.initial_state           = config.initial_state;
    config_.k                       = config.k;

    buildWorldData();
    buildSpacecraft();
    buildEnvironmentModel();
    buildWorldIndex();
    buildTimePolicy();
    buildSolver();
}

void ReferenceSimulation::buildWorldData() {
    const auto &world_config = config_.world_config;
    shared_vec<CelestialBody> bodies;
    for (const auto& body_config : world_config.bodies) {
        bodies.push_back(makeBody(body_config));
    }
    shared_vec<WormHole> wormholes;
    for (const auto& wh_config : world_config.wormholes) {
        wormholes.push_back(makeWormHole(wh_config));
    }
    shared_vec<Artifact> artifacts;
    for (const auto& art_config : world_config.artifacts) {
        artifacts.push_back(makeArtifact(art_config));
    }

    f64 max_radius = world_config.max_radius;
    world_data_ = std::make_unique<WorldData>(
        bodies, wormholes, artifacts, max_radius
    );
}

void ReferenceSimulation::buildSpacecraft() {
    const auto& sc_config = config_.spacecraft_config;
    spacecraft_ = std::make_unique<Spacecraft>(
        sc_config.id,
        sc_config.mass,
        sc_config.max_fuel,
        0.0f,
        sc_config.thrust_levels,
        sc_config.exhaust_speed
    );
}

void ReferenceSimulation::buildEnvironmentModel() {
    env_model_ = std::make_unique<EnvironmentModel>(
        *world_data_
    );
}

void ReferenceSimulation::buildWorldIndex() {
    world_index_ = std::make_unique<WorldIndex>(
        *world_data_
    );
}

void ReferenceSimulation::buildTimePolicy() {
    const auto& time_config = config_.time_config;
    time_policy_ = std::make_unique<TimePolicy>(
        *env_model_,
        time_config.tmax_u,
        time_config.dt_u
    );
}

void ReferenceSimulation::buildSolver() {
    Quantizer quantizer = makeQuantizer();
    shared_vec<ActionModel> action_models = makeActionModels();

    solver_ = std::make_unique<Solver>(
        quantizer,
        std::make_shared<BFSSolver<StateVertex>>(),
        action_models
    );
}

Quantizer ReferenceSimulation::makeQuantizer() const {
    const auto& qc = config_.quantization_config;
    QuantizerConfig config(qc.pos_bin, qc.vel_bin, qc.time_bin, qc.fuel_bin);
    return Quantizer(config);
}

shared_vec<ActionModel> ReferenceSimulation::makeActionModels() const {
    shared_vec<ActionModel> models;
    models.push_back(std::make_shared<ThrustActionModel>(
        *env_model_,
        *time_policy_,
        *world_index_,
        *world_data_,
        *spacecraft_,
        config_.spacecraft_config.possible_directions
    ));
    return models;
}

void ReferenceSimulation::compute() {
    auto iState = config_.initial_state;
    StateVertex start(
        Matrix(2, 1, iState.position),
        Matrix(2, 1, iState.velocity),
        0.0f,
        iState.fuel
    );
    auto goal = [&] (const StateVertex& sv) {
        return sv.collected_artifacts.size() >= config_.k;
    };

    auto result = solver_->solve(start, goal);
    if (result) {
        last_result_ = *result;
        current_step_ = 0;
    } else {
        throw SimulationFailed("No valid path found by solver.");
    }
}

WorldFrame ReferenceSimulation::step() {
    if (!last_result_) {
        throw SimulationFailed("Simulation has not been computed yet.");
    }

    if (current_step_ >= last_result_->path.size()) {
        throw SimulationCompleted("Simulation has already reached the final step.");
    }
    StateAction sa = last_result_->path[current_step_++];

    return toFrame(*sa.state);
}

void ReferenceSimulation::shutdown() {
    // Clean up resources if necessary
}

WorldFrame ReferenceSimulation::toFrame(const StateVertex& state) const {
    WorldFrame frame;
    frame.t_u = state.t_u;

    ShipFrame ship_frame;
    ship_frame.x = state.x;
    ship_frame.v = state.v;
    ship_frame.fuel = state.fuel;
    ship_frame.t_p = 0; // Proper time can be computed if needed
    ship_frame.collected_artifacts = state.collected_artifacts;
    frame.ship = ship_frame;

    for (const auto& body : world_data_->bodies()) {
        BodyFrame body_frame;
        body_frame.id = body->id;
        body_frame.x = body->pos(state.t_u);
        body_frame.radius = body->radius;
        body_frame.mass = body->mass;
        frame.bodies.push_back(body_frame);
    }

    for (const auto& wh : world_data_->wormholes()) {
        WormHoleFrame wh_frame;
        wh_frame.id = wh->id;
        wh_frame.entry = wh->entry;
        wh_frame.exit = wh->exit;
        wh_frame.t_open = wh->t_open;
        wh_frame.t_close = wh->t_close;
        frame.wormholes.push_back(wh_frame);
    }

    for (const auto& art : world_data_->artifacts()) {
        ArtifactFrame art_frame;
        art_frame.id = art->id;
        art_frame.position = art->pos(0.0f); // Artifacts are stationary
        frame.artifacts.push_back(art_frame);
    }

    return frame;
}

std::shared_ptr<CelestialBody> ReferenceSimulation::makeBody(
    const BodyConfig& body_config
) const {
    auto result = std::visit(overloaded{
        [&](const StationaryBodyConfig& sbc) -> std::shared_ptr<CelestialBody> {
            Matrix position(2, 1, sbc.position);
            return std::make_shared<StationaryBody>(
                sbc.id, sbc.radius, sbc.mass, position
            );
        },
        [&](const TrajectoryConfig& tc) -> std::shared_ptr<CelestialBody> {
            auto strategy = std::make_unique<EllipticalOrbit>(
                tc.a, tc.b, tc.omega, tc.phi,
                Matrix(2, 1, tc.center), tc.angle
            );
            return std::make_shared<OrbitingBody>(
                tc.id, tc.radius, tc.mass, std::move(strategy)
            );
        }
    }, body_config);

    return result;
}

std::shared_ptr<WormHole> ReferenceSimulation::makeWormHole(
    const WormHoleConfig& wh_config
) const {
    Matrix entry(2, 1, wh_config.entry);
    Matrix exit(2, 1, wh_config.exit);
    return std::make_shared<WormHole>(
        wh_config.id, entry, exit, wh_config.t_open, wh_config.t_close
    );
}

std::shared_ptr<Artifact> ReferenceSimulation::makeArtifact(
    const ArtifactConfig& art_config
) const {
    Matrix position(2, 1, art_config.position);
    return std::make_shared<Artifact>(
        art_config.id, position
    );
}

} // namespace ref