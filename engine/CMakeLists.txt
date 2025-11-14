find_package(nlohmann_json CONFIG REQUIRED)
find_package(CLI11 CONFIG REQUIRED)
find_package(gtest CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

# ---- Engine Core library ----
file(GLOB ENGINE_SRC "src/*.cpp")
list(FILTER ENGINE_SRC EXCLUDE REGEX "main\\.cpp$")

add_library(engine_core STATIC ${ENGINE_SRC})
target_include_directories(engine_core PUBLIC src)
target_link_libraries(engine_core PUBLIC nlohmann_json::nlohmann_json CLI11::CLI11 spdlog::spdlog)

# ---- Engine executable ----
# main.cpp for engine executable
add_executable(engine src/main.cpp)
target_link_libraries(engine PRIVATE engine_core)
target_include_directories(engine PUBLIC src)

# ---- Tests ----
file(GLOB TEST_SRC "tests/*.cpp")
if (TEST_SRC)
    add_executable(engine_tests ${TEST_SRC})
    target_include_directories(engine_tests PUBLIC src tests)
    target_link_libraries(engine_tests PRIVATE engine_core gtest::gtest gtest::gtest_main)
endif()