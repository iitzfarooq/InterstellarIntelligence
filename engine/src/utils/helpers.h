#pragma once

#include <concepts>
#include <type_traits>
#include <utility>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <set>
#include <iterator>
#include <functional>

#include "utils/types.h"

inline void req(bool condition, const std::string& message = "Requirement failed") {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

template <typename... ts>
struct overloaded : ts... {
    using ts::operator()...;
};

template <typename T>
inline std::set<T> operator|(const std::set<T>& a, const std::set<T>& b) {
    std::set<T> result;
    std::set_union(
        a.begin(), a.end(),
        b.begin(), b.end(),
        std::inserter(result, result.begin())
    );
    return result;
}

template <typename T>
inline uset<T> operator|(const uset<T>& a, const uset<T>& b) {
    uset<T> res = a;
    res.insert(b.begin(), b.end());
    return res;
}

template <typename T>
inline std::set<T> operator&(const std::set<T>& a, const std::set<T>& b) {
    std::set<T> result;
    std::set_intersection(
        a.begin(), a.end(),
        b.begin(), b.end(),
        std::inserter(result, result.begin())
    );
    return result;
}

template <typename T>
inline uset<T> operator&(const uset<T>& a, const uset<T>& b) {
    uset<T> res;
    for (const auto& item : a) {
        if (b.count(item)) { res.insert(item); }
    }

    return res;
}

template <typename T>
inline std::set<T> operator-(const std::set<T>& a, const std::set<T>& b) {
    std::set<T> result;
    std::set_difference(
        a.begin(), a.end(),
        b.begin(), b.end(),
        std::inserter(result, result.begin())
    );
    return result;
}

template <typename T>
inline uset<T> operator-(const uset<T>& a, const uset<T>& b) {
    uset<T> res;
    for (const auto& item : a) {
        if (!b.count(item)) { res.insert(item); }
    }

    return res;
}


inline size_t hash_combine(size_t& seed, size_t hash) {
    seed ^= hash + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    return seed;
}