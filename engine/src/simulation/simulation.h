#pragma once

#include <memory>
#include <optional>
#include <stdexcept>
#include "simulation/models.h"
#include "simulation/world.h"
#include "simulation/actions.h"
#include "simulation/solver.h"
#include "simulation/strategies.h"
#include "core/configs.h"
#include "utils/helpers.h"
#include "utils/math.h"

class Simulation {
public:
    virtual void initialize(const SimulationConfig& config) = 0;
    virtual void compute() = 0;
    virtual WorldFrame step() = 0;
    virtual void shutdown() = 0;

    virtual ~Simulation() = default;
};

class SimulationFailed : public std::runtime_error {
public:
    explicit SimulationFailed(const std::string& message)
        : std::runtime_error(message) {}
};

class SimulationCompleted : public std::runtime_error {
public:
    explicit SimulationCompleted(const std::string& message)
        : std::runtime_error(message) {}
};

// ------------------- ReferenceSimulation ------------------

namespace ref {
    class ReferenceSimulation : public Simulation {
    public:
        ReferenceSimulation() = default;
    
        virtual void initialize(const SimulationConfig& config) override;
        virtual void compute() override;
        virtual WorldFrame step() override;
        virtual void shutdown() override;
    
    private:
        WorldFrame toFrame(const StateVertex& state) const;
    
        SimulationConfig                    config_;
        std::unique_ptr<WorldData>          world_data_;
        std::unique_ptr<WorldIndex>         world_index_;
        std::unique_ptr<TimePolicy>         time_policy_;
        std::unique_ptr<EnvironmentModel>   env_model_;
        std::unique_ptr<Solver>             solver_;
        std::unique_ptr<Spacecraft>         spacecraft_;

        std::optional<SolverResult>         last_result_;
        size_t                              current_step_ = 0;

        void buildWorldData();
        void buildSpacecraft();
        void buildEnvironmentModel();
        void buildWorldIndex();
        void buildTimePolicy();
        void buildSolver();

        std::shared_ptr<CelestialBody> makeBody(
            const BodyConfig& body_config
        ) const;
        std::shared_ptr<WormHole> makeWormHole(
            const WormHoleConfig& wh_config
        ) const;
        std::shared_ptr<Artifact> makeArtifact(
            const ArtifactConfig& art_config
        ) const;
        Quantizer makeQuantizer() const;
        shared_vec<ActionModel> makeActionModels() const;
    };
}    

