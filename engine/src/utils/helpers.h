#pragma once

#include <concepts>
#include <type_traits>
#include <utility>
#include <string>
#include <stdexcept>

void req(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}