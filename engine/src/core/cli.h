#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <optional>
#include <variant>
#include <sstream>

#include <CLI/CLI.hpp>
#include "utils/types.h"

namespace fs = std::filesystem;

struct CLIConfig {
    fs::path file_path;
};

struct TestConfig : CLIConfig {
    std::optional<u32> round_number;
    std::vector<std::string> unit_keywords;
};

struct SimulationConfig : CLIConfig {
    std::optional<u32> round_number;
    std::optional<std::string> world_name;
    bool graphics = false;
};

struct FinalEvalConfig : CLIConfig {

};

using RunConfig = std::variant<TestConfig, SimulationConfig, FinalEvalConfig>;

template <typename T>
class ConfigVisitor {
public:
    virtual T operator()(TestConfig config) = 0;
    virtual T operator()(SimulationConfig config) = 0;
    virtual T operator()(FinalEvalConfig config) = 0;

    virtual ~ConfigVisitor() = default;
};

RunConfig parseCli(int argc, char** argv);