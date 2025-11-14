#include "orchestrator.h"

// temporarily for now
#include <iostream>


SimulatorOrchestrator::SimulatorOrchestrator(const SimulationConfig& config)
    : config_(config) {}

void SimulatorOrchestrator::initialize() {
    std::cout << "SimulatorOrchestrator: Initializing with world '"
              << (config_.world_name.has_value() ? *config_.world_name : "default")
              << "' for round "
              << (config_.round_number.has_value() ? std::to_string(*config_.round_number) : "N/A")
              << (config_.graphics ? " with graphics." : " without graphics.")
              << std::endl;
}

void SimulatorOrchestrator::run() {
    std::cout << "SimulatorOrchestrator: Running simulation..." << std::endl;
}

void SimulatorOrchestrator::shutdown() {
    std::cout << "SimulatorOrchestrator: Shutting down simulation." << std::endl;
}

TestOrchestrator::TestOrchestrator(const TestConfig& config)
    : config_(config) {}

void TestOrchestrator::initialize() {
    std::cout << "TestOrchestrator: Initializing for round "
              << (config_.round_number.has_value() ? std::to_string(*config_.round_number) : "N/A")
              << " with unit keywords: ";
    for (const auto& keyword : config_.unit_keywords) {
        std::cout << keyword << " ";
    }
    std::cout << std::endl;
}

void TestOrchestrator::run() {
    std::cout << "TestOrchestrator: Running tests..." << std::endl;
}

void TestOrchestrator::shutdown() {
    std::cout << "TestOrchestrator: Shutting down tests." << std::endl;
}

FinalEvalOrchestrator::FinalEvalOrchestrator(const FinalEvalConfig& config)
    : config_(config) {}

void FinalEvalOrchestrator::initialize() {
    std::cout << "FinalEvalOrchestrator: Initializing final evaluation." << std::endl;
}

void FinalEvalOrchestrator::run() {
    std::cout << "FinalEvalOrchestrator: Running final evaluation..." << std::endl;
}

void FinalEvalOrchestrator::shutdown() {
    std::cout << "FinalEvalOrchestrator: Shutting down final evaluation." << std::endl;
}

std::unique_ptr<I_Orchestrator> MakeOrchestrator::operator()(TestConfig config) {
    return std::make_unique<TestOrchestrator>(std::move(config));
}

std::unique_ptr<I_Orchestrator> MakeOrchestrator::operator()(SimulationConfig config) {
    return std::make_unique<SimulatorOrchestrator>(std::move(config));
}

std::unique_ptr<I_Orchestrator> MakeOrchestrator::operator()(FinalEvalConfig config) {
    return std::make_unique<FinalEvalOrchestrator>(std::move(config));
}

std::unique_ptr<I_Orchestrator> createOrchestrator(RunConfig config) {
    MakeOrchestrator maker;
    return std::visit(maker, std::move(config));
}