#ifndef GRAPH_OTHERS_GRAPH_ISOMORPHISM_HPP
#define GRAPH_OTHERS_GRAPH_ISOMORPHISM_HPP

// 重みなし一般無向グラフの同型性を判定する。
// 頂点は 0-based indexing、自己ループと多重辺を許す。
// 不正な頂点番号は assert で落とす。
// 1-WL 色分割と個別化による探索を用いる。
// 最悪指数時間である。

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <set>
#include <utility>
#include <vector>

struct GraphIsomorphism {
  public:
    struct Graph {
        int n;
        std::vector<std::vector<std::pair<int, int>>> adjacency;
        std::vector<int> loop_count;
        std::vector<int> degree;
        int edge_count;

        Graph(int n_, const std::vector<std::pair<int, int>> &input_edges)
            : n(n_), edge_count(static_cast<int>(input_edges.size())) {
            assert(n >= 0);
            adjacency.assign(n, {});
            loop_count.assign(n, 0);
            degree.assign(n, 0);

            std::vector<std::array<int, 2>> sorted_edges;
            sorted_edges.reserve(input_edges.size());
            for (const auto &[a, b] : input_edges) {
                assert(0 <= a && a < n);
                assert(0 <= b && b < n);
                int u = a;
                int v = b;
                if (v < u) {
                    std::swap(u, v);
                }
                sorted_edges.push_back({u, v});
            }

            std::sort(sorted_edges.begin(), sorted_edges.end());
            for (int i = 0; i < static_cast<int>(sorted_edges.size());) {
                int j = i + 1;
                while (j < static_cast<int>(sorted_edges.size()) &&
                       sorted_edges[i] == sorted_edges[j]) {
                    ++j;
                }
                const int u = sorted_edges[i][0];
                const int v = sorted_edges[i][1];
                const int count = j - i;
                if (u == v) {
                    adjacency[u].push_back({v, count});
                    loop_count[u] += count;
                    degree[u] += count;
                } else {
                    adjacency[u].push_back({v, count});
                    adjacency[v].push_back({u, count});
                    degree[u] += count;
                    degree[v] += count;
                }
                i = j;
            }
        }
    };

    struct StateKey {
        std::uint64_t hash_value;
        std::vector<int> colors;

        friend bool operator<(const StateKey &lhs, const StateKey &rhs) {
            if (lhs.hash_value != rhs.hash_value) {
                return lhs.hash_value < rhs.hash_value;
            }
            return lhs.colors < rhs.colors;
        }
    };

    int n;
    std::array<Graph, 2> graph;
    std::set<StateKey> dead_states;

    GraphIsomorphism(int n_, const std::vector<std::pair<int, int>> &edges_1,
                     const std::vector<std::pair<int, int>> &edges_2)
        : n(n_), graph{Graph(n_, edges_1), Graph(n_, edges_2)} {}

    bool run() {
        if (graph[0].edge_count != graph[1].edge_count) {
            return false;
        }
        if (n == 0) {
            return true;
        }

        std::vector<std::array<int, 4>> keys;
        keys.reserve(2 * n);
        for (int t = 0; t < 2; ++t) {
            for (int v = 0; v < n; ++v) {
                keys.push_back(
                    {graph[t].loop_count[v], graph[t].degree[v], t, v});
            }
        }
        std::sort(keys.begin(), keys.end());

        std::array<std::vector<int>, 2> color{std::vector<int>(n),
                                              std::vector<int>(n)};
        int color_count = 0;
        for (int i = 0; i < static_cast<int>(keys.size()); ++i) {
            if (i == 0 || keys[i][0] != keys[i - 1][0] ||
                keys[i][1] != keys[i - 1][1]) {
                ++color_count;
            }
            color[keys[i][2]][keys[i][3]] = color_count - 1;
        }
        if (!same_color_count(color)) {
            return false;
        }

        dead_states.clear();
        return dfs(std::move(color));
    }

  private:
    struct RefineWorkspace {
        std::vector<std::vector<int>> signatures;
        std::vector<int> multiplicity;
        std::vector<int> touched_colors;
        std::vector<int> order;
        std::vector<std::array<int, 2>> count;
        std::array<std::vector<int>, 2> next_color;
    };

    RefineWorkspace refine_workspace;

    bool refine(std::array<std::vector<int>, 2> &color) {
        auto &signatures = refine_workspace.signatures;
        auto &multiplicity = refine_workspace.multiplicity;
        auto &touched_colors = refine_workspace.touched_colors;
        auto &order = refine_workspace.order;
        auto &count = refine_workspace.count;
        auto &next_color = refine_workspace.next_color;
        signatures.resize(2 * n);
        order.resize(2 * n);
        next_color[0].resize(n);
        next_color[1].resize(n);
        while (true) {
            int color_count = 0;
            for (int v = 0; v < n; ++v) {
                color_count = std::max(color_count, color[0][v] + 1);
            }

            multiplicity.assign(color_count, 0);
            touched_colors.clear();
            touched_colors.reserve(color_count);
            for (int t = 0; t < 2; ++t) {
                for (int v = 0; v < n; ++v) {
                    for (const auto &[to, count] : graph[t].adjacency[v]) {
                        if (to != v) {
                            const int neighbor_color = color[t][to];
                            if (multiplicity[neighbor_color] == 0) {
                                touched_colors.push_back(neighbor_color);
                            }
                            multiplicity[neighbor_color] += count;
                        }
                    }
                    std::sort(touched_colors.begin(), touched_colors.end());

                    std::vector<int> &signature = signatures[t * n + v];
                    signature.clear();
                    signature.reserve(2 + 2 * touched_colors.size());
                    signature.push_back(color[t][v]);
                    signature.push_back(graph[t].loop_count[v]);
                    for (int neighbor_color : touched_colors) {
                        signature.push_back(neighbor_color);
                        signature.push_back(multiplicity[neighbor_color]);
                        multiplicity[neighbor_color] = 0;
                    }
                    touched_colors.clear();
                }
            }

            for (int i = 0; i < 2 * n; ++i) {
                order[i] = i;
            }
            std::sort(order.begin(), order.end(), [&](int lhs, int rhs) {
                return signatures[lhs] < signatures[rhs];
            });

            int next_color_count = 0;
            for (int i = 0; i < 2 * n; ++i) {
                if (i == 0 ||
                    signatures[order[i - 1]] != signatures[order[i]]) {
                    ++next_color_count;
                }
                const int t = order[i] / n;
                const int v = order[i] % n;
                next_color[t][v] = next_color_count - 1;
            }

            count.assign(next_color_count, std::array<int, 2>{});
            for (int t = 0; t < 2; ++t) {
                for (int v = 0; v < n; ++v) {
                    ++count[next_color[t][v]][t];
                }
            }
            for (const auto &color_count_pair : count) {
                if (color_count_pair[0] != color_count_pair[1]) {
                    return false;
                }
            }
            if (next_color_count == color_count) {
                return true;
            }
            color.swap(next_color);
        }
    }

    bool dfs(std::array<std::vector<int>, 2> color) {
        if (!refine(color)) {
            return false;
        }

        int color_count = 0;
        for (int v = 0; v < n; ++v) {
            color_count = std::max(color_count, color[0][v] + 1);
        }

        std::vector<int> count(color_count, 0);
        for (int v = 0; v < n; ++v) {
            ++count[color[0][v]];
        }

        int branch_color = -1;
        int branch_size = n + 1;
        for (int c = 0; c < color_count; ++c) {
            if (1 < count[c] && count[c] < branch_size) {
                branch_color = c;
                branch_size = count[c];
            }
        }

        if (branch_color == -1) {
            return true;
        }

        StateKey key = make_state_key(color);
        if (dead_states.contains(key)) {
            return false;
        }

        int u = -1;
        for (int v = 0; v < n; ++v) {
            if (color[0][v] == branch_color) {
                u = v;
                break;
            }
        }

        const int new_color = color_count;
        for (int v = 0; v < n; ++v) {
            if (color[1][v] == branch_color) {
                auto next_color = color;
                next_color[0][u] = new_color;
                next_color[1][v] = new_color;
                if (dfs(std::move(next_color))) {
                    return true;
                }
            }
        }
        dead_states.insert(std::move(key));
        return false;
    }

    bool same_color_count(const std::array<std::vector<int>, 2> &color) const {
        int color_count = 0;
        for (int t = 0; t < 2; ++t) {
            for (int v = 0; v < n; ++v) {
                color_count = std::max(color_count, color[t][v] + 1);
            }
        }
        std::vector<std::array<int, 2>> count(color_count);
        for (int v = 0; v < n; ++v) {
            ++count[color[0][v]][0];
            ++count[color[1][v]][1];
        }
        for (const auto &color_count_pair : count) {
            if (color_count_pair[0] != color_count_pair[1]) {
                return false;
            }
        }
        return true;
    }

    static std::uint64_t mix(std::uint64_t x) {
        x += 0x9e3779b97f4a7c15ULL;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
        return x ^ (x >> 31);
    }

    StateKey
    make_state_key(const std::array<std::vector<int>, 2> &color) const {
        StateKey key;
        key.colors.reserve(2 * n);

        std::uint64_t hash_value = 0x6a09e667f3bcc909ULL;
        for (int t = 0; t < 2; ++t) {
            for (int v = 0; v < n; ++v) {
                const int c = color[t][v];
                key.colors.push_back(c);
                const std::uint64_t x =
                    static_cast<std::uint64_t>(c + 1) +
                    static_cast<std::uint64_t>(key.colors.size()) *
                        0x9e3779b97f4a7c15ULL;
                hash_value ^= mix(x);
                hash_value = (hash_value << 7) | (hash_value >> 57);
            }
        }
        key.hash_value = hash_value;
        return key;
    }
};

inline bool
is_graph_isomorphic(int n, const std::vector<std::pair<int, int>> &edges_1,
                    const std::vector<std::pair<int, int>> &edges_2) {
    return GraphIsomorphism(n, edges_1, edges_2).run();
}

#endif
