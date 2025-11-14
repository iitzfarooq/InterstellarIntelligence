#pragma once

#include <memory>
#include <string>
#include <variant>

#include "core/cli.h"
#include "utils/helpers.h"

class I_Orchestrator {
public:
    virtual void initialize() = 0;
    virtual void run() = 0;
    virtual void shutdown() = 0;

    virtual ~I_Orchestrator() = default;
};

class SimulatorOrchestrator : public I_Orchestrator {
private:
    SimulationConfig config_;

public: 
    SimulatorOrchestrator(const SimulationConfig& config);

    void initialize() override;
    void run() override;
    void shutdown() override;
};

class TestOrchestrator : public I_Orchestrator {
private:
    TestConfig config_;
public:
    TestOrchestrator(const TestConfig& config);

    void initialize() override;
    void run() override;
    void shutdown() override;
};

class FinalEvalOrchestrator : public I_Orchestrator {
private:
    FinalEvalConfig config_;
public:
    FinalEvalOrchestrator(const FinalEvalConfig& config);

    void initialize() override;
    void run() override;
    void shutdown() override;
};

std::unique_ptr<I_Orchestrator> createOrchestrator(RunConfig config);

class MakeOrchestrator : public ConfigVisitor<std::unique_ptr<I_Orchestrator>> {
public:
    std::unique_ptr<I_Orchestrator> operator()(TestConfig config) override;
    std::unique_ptr<I_Orchestrator> operator()(SimulationConfig config) override; 
    std::unique_ptr<I_Orchestrator> operator()(FinalEvalConfig config) override;
};