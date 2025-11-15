#pragma once

#include <concepts>
#include <type_traits>
#include <utility>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <set>

inline void req(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

template <typename... ts>
struct overloaded : ts... {
    using ts::operator()...;
};

template <typename T>
std::set<T> operator|(const std::set<T>& a, const std::set<T>& b) {
    std::set<T> result;
    std::set_union(
        a.begin(), a.end(),
        b.begin(), b.end(),
        std::inserter(result, result.begin())
    );
    return result;
}

template <typename T>
std::set<T> operator&(const std::set<T>& a, const std::set<T>& b) {
    std::set<T> result;
    std::set_intersection(
        a.begin(), a.end(),
        b.begin(), b.end(),
        std::inserter(result, result.begin())
    );
    return result;
}

template <typename T>
std::set<T> operator-(const std::set<T>& a, const std::set<T>& b) {
    std::set<T> result;
    std::set_difference(
        a.begin(), a.end(),
        b.begin(), b.end(),
        std::inserter(result, result.begin())
    );
    return result;
}