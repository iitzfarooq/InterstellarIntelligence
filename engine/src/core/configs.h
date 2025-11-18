#pragma once

#include <string>
#include <vector>
#include <variant>
#include "utils/types.h"
#include "utils/matrix.h"

struct StationaryBodyConfig {
    u32 id;
    f64 mass;
    f64 radius;
    std::vector<f64> position;
};

struct TrajectoryConfig {
    u32 id;
    f64 mass;
    f64 radius;

    f64 a, b, omega, phi, angle;
    std::vector<f64> center;
};

using BodyConfig = std::variant<StationaryBodyConfig, TrajectoryConfig>;

struct WormHoleConfig {
    u32 id;
    std::vector<u32> entry, exit;
    f64 t_open;
    f64 t_close;
};

struct ArtifactConfig {
    u32 id;
    std::vector<f64> position;
};

struct WorldConfig {
    std::vector<BodyConfig> bodies;
    std::vector<WormHoleConfig> wormholes;
    std::vector<ArtifactConfig> artifacts;
};

struct TimeConfig {
    f64 tmax_u;
    f64 dt_u;
};

struct QuantizationConfig {
    f64 pos_bin;
    f64 vel_bin;
    f64 time_bin;
    f64 fuel_bin;
};

struct SpaceCraftConfig {
    f64 mass;
    f64 max_fuel;
    std::vector<f64> thrust_levels;
    f64 exhaust_speed;

    std::vector<f64> possible_directions; // in radians
    std::vector<f64> initial_position;
    std::vector<f64> initial_velocity;
};

struct EngineConfig {
    WorldConfig world_config;
    TimeConfig time_config;
    QuantizationConfig quantization_config;
    SpaceCraftConfig spacecraft_config;
};