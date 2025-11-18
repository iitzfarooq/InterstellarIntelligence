#include "solver.h"

std::vector<StateAction> Solver::neighbors(const StateVertex& sv) const {
    std::vector<StateAction> result;

    for (const auto& action_model : action_models) {
        auto actions = action_model->enumerate(sv);
        for (const auto& action : actions) {
            auto maybe_next = action_model->apply(sv, action);
            if (maybe_next.has_value()) {
                auto vertex = std::make_shared<StateVertex>(maybe_next.value());
                result.push_back(StateAction(vertex, action));
            }
        }
    }

    return result;
}

std::vector<StateAction> Solver::reconstruct(
    const StateVertex& goal,
    const umap<DiscreteState, StateAction>& parent_map
) const {
    std::vector<StateAction> path;
    auto v = std::make_shared<StateVertex>(goal);
    auto a = std::make_shared<Action>(nullptr);

    while (true) {
        path.push_back(StateAction(v, a));
        auto it = parent_map.find(quantizer.q(*v));
        if (it == parent_map.end()) {
            break;
        }
        auto [parent_v, action] = it->second;

        v = parent_v;
        a = action;
    }

    std::reverse(path.begin(), path.end());
    return path;
}

template <typename R, typename T, typename BinOp>
T foldLeft(R&& range, T init, BinOp op) {
    return std::ranges::for_each(
        range,
        [&init, &op] (const auto& value) {
            init = op(std::forward<T>(init), value);
        }
    );
}

f64 computeCost(const std::vector<StateAction>& path) {
    return foldLeft(
        path 
        | std::views::take(path.size() - 1) 
        | std::views::transform(
            [] (const StateAction& sa) {
                return sa.action ? sa.action->cost() : 0.0f;
            }
        ),
        0.0f,
        std::plus<f64>()
    );
}


std::optional<SolverResult> Solver::solve(
    const StateVertex& start,
    const std::function<bool(const StateVertex&)>& isGoal,
    f64 max_cost
) const {
    SeenSet visited;
    ParentMap parent_map;

    strategy->push(std::make_shared<StateVertex>(start));
    visited.insert(quantizer.q(start));

    while (!strategy->empty()) {
        auto current = strategy->pop();

        if (isGoal(*current)) {
            auto path = reconstruct(*current, parent_map);
            auto total_cost = computeCost(path);

            return SolverResult{path, total_cost};
        }

        for (const auto& nh : neighbors(*current)) {
            auto q_neighbor = quantizer.q(*nh.state);
            auto action = nh.action;

            if (visited.find(q_neighbor) == visited.end()) {
                visited.insert(q_neighbor);
                parent_map[q_neighbor] = StateAction(current, action);
                strategy->push(nh.state);
            }
        }
    }

    return std::nullopt;
}