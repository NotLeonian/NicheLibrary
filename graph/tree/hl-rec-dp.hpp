#ifndef GRAPH_TREE_HL_REC_DP_HPP
#define GRAPH_TREE_HL_REC_DP_HPP

// 重軽分解の重い子だけを同じ初期状態で再帰する DP の骨格である。
// Spec は State, K と 6 個の関数を持つ必要がある。
// 入力は木、または根から全頂点へ到達する根付き木である。
// 軽い子へ潜る再帰だけを積むため、再帰の段数は O(log N) である。
// 計算量は Spec の処理時間と呼び出し回数に依存する。

// 参考文献:
// 1. Soh Kumabe, Takanori Maehara, and Ryoma Sin'ya. Linear Pseudo-Polynomial Factor Algorithm for Automaton Constrained Tree Knapsack Problem. In WALCOM: Algorithms and Computation, Lecture Notes in Computer Science, Vol. 11355, pp. 248–260. Springer, 2019. doi:10.1007/978-3-030-10564-8_20.
//    arXiv: https://arxiv.org/abs/1807.04942

#include <array>
#include <cassert>
#include <utility>
#include <vector>

namespace hl_rec_dp_impl {
inline void put_heavy_child_first(std::vector<std::vector<int>> &child,
                                  const std::vector<int> &order) {
    const int n = static_cast<int>(child.size());
    std::vector<int> subtree_size(n, 1);
    for (int i = static_cast<int>(order.size()) - 1; i >= 0; --i) {
        const int v = order[i];
        for (int to : child[v]) {
            subtree_size[v] += subtree_size[to];
        }
        if (!child[v].empty()) {
            int heavy_index = 0;
            for (int j = 1; j < static_cast<int>(child[v].size()); ++j) {
                const int a = child[v][j];
                const int b = child[v][heavy_index];
                if (subtree_size[a] > subtree_size[b] ||
                    (subtree_size[a] == subtree_size[b] && a < b)) {
                    heavy_index = j;
                }
            }
            std::swap(child[v][0], child[v][heavy_index]);
        }
    }
}

inline std::vector<std::vector<int>>
make_rooted_tree(int n, const std::vector<std::pair<int, int>> &edges,
                 int root) {
    assert(n >= 1);
    assert(static_cast<int>(edges.size()) == n - 1);
    assert(0 <= root && root < n);
    std::vector<std::vector<int>> graph(n);
    for (const auto &[u, v] : edges) {
        assert(0 <= u && u < n);
        assert(0 <= v && v < n);
        assert(u != v);
        graph[u].push_back(v);
        graph[v].push_back(u);
    }

    std::vector<int> parent(n, -2);
    std::vector<int> order;
    order.reserve(n);
    std::vector<int> stack;
    stack.reserve(n);
    stack.push_back(root);
    parent[root] = -1;
    while (!stack.empty()) {
        const int v = stack.back();
        stack.pop_back();
        order.push_back(v);
        int child_count = 0;
        for (int to : graph[v]) {
            if (to == parent[v]) {
                continue;
            }
            assert(parent[to] == -2);
            parent[to] = v;
            graph[v][child_count] = to;
            ++child_count;
            stack.push_back(to);
        }
        graph[v].resize(child_count);
    }
    assert(static_cast<int>(order.size()) == n);
    put_heavy_child_first(graph, order);
    return graph;
}

template <class Spec> struct hl_rec_dp_runner {
    using State = typename Spec::State;
    static constexpr int K = Spec::K;
    static_assert(K >= 1, "Spec::K must be positive.");
    using Pack = std::array<State, K>;

    std::vector<std::vector<int>> child;
    std::vector<int> path;
    Spec &spec;

    hl_rec_dp_runner(std::vector<std::vector<int>> child, Spec &spec)
        : child(std::move(child)), spec(spec) {
        path.reserve(this->child.size());
    }

    Pack run(int root, const State &initial_state) {
        return dfs_heavy_path<true>(root, initial_state);
    }

    template <bool Collect> Pack dfs_heavy_path(int start, const State &in) {
        const int path_begin = static_cast<int>(path.size());
        for (int v = start;; v = child[v][0]) {
            path.push_back(v);
            if (child[v].empty()) {
                break;
            }
        }

        const int path_end = static_cast<int>(path.size());
        Pack cur = spec.make_pack(path[path_end - 1], in);
        for (int i = path_end - 1; i >= path_begin; --i) {
            const int v = path[i];
            if (i + 1 < path_end) {
                cur = spec.take_heavy(v, path[i + 1], std::move(cur));
            }
            for (int idx = 1; idx < static_cast<int>(child[v].size()); ++idx) {
                const int to = child[v][idx];
                for (int lane = 0; lane < K; ++lane) {
                    cur[lane] = spec.take_light(
                        v, to, lane, dfs_heavy_path<false>(to, cur[lane]));
                }
            }
            if constexpr (Collect) {
                spec.before_vertex(v, cur);
            }
            spec.add_vertex(v, cur);
            if constexpr (Collect) {
                spec.after_vertex(v, cur);
                for (int idx = 1; idx < static_cast<int>(child[v].size());
                     ++idx) {
                    dfs_heavy_path<true>(child[v][idx], in);
                }
            }
        }
        path.resize(path_begin);
        return cur;
    }
};
} // namespace hl_rec_dp_impl

template <class Spec>
std::array<typename Spec::State, Spec::K>
hl_rec_dp(int n, const std::vector<std::pair<int, int>> &edges, int root,
          const typename Spec::State &initial_state, Spec &spec) {
    auto child = hl_rec_dp_impl::make_rooted_tree(n, edges, root);
    hl_rec_dp_impl::hl_rec_dp_runner<Spec> runner(std::move(child), spec);
    return runner.run(root, initial_state);
}

#endif
