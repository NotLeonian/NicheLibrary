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
        std::vector<std::array<int, 3>> edges;
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
            edges.reserve(sorted_edges.size());
            for (int i = 0; i < static_cast<int>(sorted_edges.size());) {
                int j = i + 1;
                while (j < static_cast<int>(sorted_edges.size()) &&
                       sorted_edges[i] == sorted_edges[j]) {
                    ++j;
                }
                const int u = sorted_edges[i][0];
                const int v = sorted_edges[i][1];
                const int count = j - i;
                edges.push_back({u, v, count});
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
        : n(n_), graph{Graph(n_, edges_1), Graph(n_, edges_2)} {
        assert(n >= 0);
    }

    bool run() {
        if (graph[0].edge_count != graph[1].edge_count) {
            return false;
        }
        if (n == 0) {
            return true;
        }

        std::vector<std::array<int, 2>> keys;
        keys.reserve(2 * n);
        for (int t = 0; t < 2; ++t) {
            for (int v = 0; v < n; ++v) {
                keys.push_back({graph[t].loop_count[v], graph[t].degree[v]});
            }
        }
        std::sort(keys.begin(), keys.end());
        keys.erase(std::unique(keys.begin(), keys.end()), keys.end());

        std::array<std::vector<int>, 2> color{std::vector<int>(n),
                                              std::vector<int>(n)};
        for (int t = 0; t < 2; ++t) {
            for (int v = 0; v < n; ++v) {
                const std::array<int, 2> key{graph[t].loop_count[v],
                                             graph[t].degree[v]};
                color[t][v] = static_cast<int>(
                    std::lower_bound(keys.begin(), keys.end(), key) -
                    keys.begin());
            }
        }
        if (!same_color_count(color)) {
            return false;
        }

        dead_states.clear();
        return dfs(std::move(color));
    }

  private:
    bool refine(std::array<std::vector<int>, 2> &color) const {
        while (true) {
            std::vector<std::vector<int>> signatures(2 * n);
            for (int t = 0; t < 2; ++t) {
                for (int v = 0; v < n; ++v) {
                    std::vector<std::pair<int, int>> neighbor_colors;
                    neighbor_colors.reserve(graph[t].adjacency[v].size());
                    for (const auto &[to, count] : graph[t].adjacency[v]) {
                        if (to != v) {
                            neighbor_colors.push_back({color[t][to], count});
                        }
                    }
                    std::sort(neighbor_colors.begin(), neighbor_colors.end());

                    std::vector<int> signature;
                    signature.reserve(2 + 2 * neighbor_colors.size());
                    signature.push_back(color[t][v]);
                    signature.push_back(graph[t].loop_count[v]);
                    for (int i = 0;
                         i < static_cast<int>(neighbor_colors.size());) {
                        int j = i + 1;
                        int count_sum = neighbor_colors[i].second;
                        while (j < static_cast<int>(neighbor_colors.size()) &&
                               neighbor_colors[i].first ==
                                   neighbor_colors[j].first) {
                            count_sum += neighbor_colors[j].second;
                            ++j;
                        }
                        signature.push_back(neighbor_colors[i].first);
                        signature.push_back(count_sum);
                        i = j;
                    }
                    signatures[t * n + v] = std::move(signature);
                }
            }

            std::vector<std::vector<int>> keys = signatures;
            std::sort(keys.begin(), keys.end());
            keys.erase(std::unique(keys.begin(), keys.end()), keys.end());

            std::array<std::vector<int>, 2> next_color{std::vector<int>(n),
                                                       std::vector<int>(n)};
            for (int t = 0; t < 2; ++t) {
                for (int v = 0; v < n; ++v) {
                    next_color[t][v] = static_cast<int>(
                        std::lower_bound(keys.begin(), keys.end(),
                                         signatures[t * n + v]) -
                        keys.begin());
                }
            }
            if (!same_color_count(next_color)) {
                return false;
            }
            if (next_color[0] == color[0] && next_color[1] == color[1]) {
                return true;
            }
            color = std::move(next_color);
        }
    }

    bool dfs(std::array<std::vector<int>, 2> color) {
        if (!refine(color)) {
            return false;
        }

        StateKey key = make_state_key(color);
        if (dead_states.find(key) != dead_states.end()) {
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
            return check_mapping(color);
        }

        int u = -1;
        for (int v = 0; v < n; ++v) {
            if (color[0][v] == branch_color) {
                u = v;
                break;
            }
        }
        assert(u != -1);

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
        std::vector<int> count0(color_count, 0), count1(color_count, 0);
        for (int v = 0; v < n; ++v) {
            ++count0[color[0][v]];
            ++count1[color[1][v]];
        }
        return count0 == count1;
    }

    bool check_mapping(const std::array<std::vector<int>, 2> &color) const {
        int color_count = 0;
        for (int v = 0; v < n; ++v) {
            color_count = std::max(color_count, color[0][v] + 1);
        }
        std::vector<int> position(color_count, -1);
        for (int v = 0; v < n; ++v) {
            position[color[1][v]] = v;
        }

        std::vector<int> permutation(n, -1);
        for (int v = 0; v < n; ++v) {
            permutation[v] = position[color[0][v]];
            assert(permutation[v] != -1);
        }

        std::vector<std::array<int, 3>> mapped_edges;
        mapped_edges.reserve(graph[0].edges.size());
        for (const auto &edge : graph[0].edges) {
            int u = permutation[edge[0]];
            int v = permutation[edge[1]];
            if (v < u) {
                std::swap(u, v);
            }
            mapped_edges.push_back({u, v, edge[2]});
        }
        std::sort(mapped_edges.begin(), mapped_edges.end());
        return mapped_edges == graph[1].edges;
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
