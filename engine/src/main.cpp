#include <iostream>
#include <concepts>

#include "core/cli.h"
#include "core/orchestrator.h"

int main(int argc, char** argv) {
    auto config = parseCli(argc, argv);
    auto orc = createOrchestrator(config);

    orc->initialize();
    orc->run();
    orc->shutdown();

    return 0;
}