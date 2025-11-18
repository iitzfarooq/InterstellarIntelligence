#include "cli.h"

void safeParse(CLI::App& app, int argc, char** argv) {
    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        auto c = app.exit(e);
        std::exit(c);
    }
}

RunConfig parseCli(int argc, char** argv) {
    CLI::App app{"Interstellar Intelligence Contest Engine"};

    std::string mode;
    app.add_option("-m, --mode", mode, "Run mode: test | sim | final")->required();

    std::optional<u32> round_number;
    app.add_option("-r, --round", round_number, "Round number");
    
    std::string unit_keywords_str;
    app.add_option("-k, --keywords", unit_keywords_str, "Space-separated keywords for unit tests");

    std::optional<std::string> world_name;
    app.add_option("-w, --world", world_name, "World JSON file to load");
    
    bool graphics = false;
    app.add_flag("-g, --graphics", graphics, "Enable graphical visualization");

    fs::path file_path = fs::absolute(argv[0]);

    safeParse(app, argc, argv);

    if (mode == "test") {
        TestConfig config;
        config.round_number = round_number;
        config.file_path = file_path;
        if (!unit_keywords_str.empty()) {
            std::istringstream iss(unit_keywords_str);
            for (std::string keyword; std::getline(iss, keyword, ' '); ) {
                config.unit_keywords.push_back(keyword);
            }
        }

        return config;
    } else if (mode == "sim") {
        SimulationConfig config;
        config.round_number = round_number;
        config.world_name = world_name;
        config.graphics = graphics;
        config.file_path = file_path;
        return config;

    } else if (mode == "final") {
        FinalEvalConfig config;
        config.file_path = file_path;
        return config;
    
    } else {
        app.require_subcommand();
    }
}