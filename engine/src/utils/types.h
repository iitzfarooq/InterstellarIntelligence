#pragma once

#include <cstdint>
#include <type_traits>
#include <set>
#include <span>
#include <vector>
#include <memory>
#include <unordered_set>
#include <unordered_map>

using u32 = uint32_t;
using u64 = uint64_t;
using f32 = float;
using f64 = double;
using byte = uint8_t;
using i32 = int32_t;
using i64 = int64_t;

template <typename T>
using shared_span = std::span<std::shared_ptr<T>>;

template <typename T>
using const_shared_span = std::span<const std::shared_ptr<T>>;

template <typename T>
using shared_vec = std::vector<std::shared_ptr<T>>;

// Below alias is not valid. std::vector<const T> is not allowed.
// Reason: std::vector requires its elements to be assignable, and const T is not assignable.
// template <typename T>
// using const_shared_vec = std::vector<std::shared_ptr<const T>>;

template <
    typename T,
    typename Hash = std::hash<T>,
    typename KeyEqual = std::equal_to<T>
>
using uset = std::unordered_set<T>;

template <
    typename K, 
    typename V, 
    typename Hash = std::hash<K>,
    typename KeyEqual = std::equal_to<K>
> using umap = std::unordered_map<K, V, Hash, KeyEqual>;