#pragma once

#include <concepts>
#include <type_traits>
#include <utility>
#include <string>
#include <stdexcept>

inline void req(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

template <typename... ts>
struct overloaded : ts... {
    using ts::operator()...;
};