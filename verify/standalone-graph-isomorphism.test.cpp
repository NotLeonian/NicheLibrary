// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>

#include "../graph/others/graph-isomorphism.hpp"

std::vector<std::pair<int, int>>
permute_graph(const std::vector<std::pair<int, int>> &edges,
              const std::vector<int> &permutation) {
    std::vector<std::pair<int, int>> res;
    res.reserve(edges.size());
    for (const auto &[u, v] : edges) {
        res.push_back({permutation[u], permutation[v]});
    }
    return res;
}

std::vector<std::vector<int>>
edge_count_matrix(int n, const std::vector<std::pair<int, int>> &edges) {
    std::vector<std::vector<int>> matrix(n, std::vector<int>(n, 0));
    for (const auto &[a, b] : edges) {
        int u = a;
        int v = b;
        if (v < u) {
            std::swap(u, v);
        }
        ++matrix[u][v];
    }
    return matrix;
}

std::vector<int>
edge_count_vector(int n, const std::vector<std::pair<int, int>> &edges) {
    const auto matrix = edge_count_matrix(n, edges);
    std::vector<int> res;
    res.reserve(n * (n + 1) / 2);
    for (int u = 0; u < n; ++u) {
        for (int v = u; v < n; ++v) {
            res.push_back(matrix[u][v]);
        }
    }
    return res;
}

std::vector<std::vector<int>> all_permutations(int n) {
    std::vector<int> permutation(n);
    for (int i = 0; i < n; ++i) {
        permutation[i] = i;
    }
    std::vector<std::vector<int>> res;
    do {
        res.push_back(permutation);
    } while (std::next_permutation(permutation.begin(), permutation.end()));
    return res;
}

std::vector<int>
canonical_signature(int n, const std::vector<std::pair<int, int>> &edges) {
    const auto permutations = all_permutations(n);
    std::vector<int> best;
    bool initialized = false;
    for (const auto &permutation : permutations) {
        auto current = edge_count_vector(n, permute_graph(edges, permutation));
        if (!initialized || current < best) {
            best = std::move(current);
            initialized = true;
        }
    }
    return best;
}

std::vector<std::pair<int, int>> graph_from_code(int n, int code) {
    std::vector<std::pair<int, int>> edges;
    for (int u = 0; u < n; ++u) {
        for (int v = u; v < n; ++v) {
            const int count = code % 3;
            code /= 3;
            for (int k = 0; k < count; ++k) {
                edges.push_back({u, v});
            }
        }
    }
    return edges;
}

void self_test() {
    assert(is_graph_isomorphic(0, {}, {}));
    assert(is_graph_isomorphic(2, {{0, 1}, {0, 1}, {0, 0}},
                               {{1, 0}, {1, 0}, {1, 1}}));
    assert(!is_graph_isomorphic(2, {{0, 1}, {0, 1}}, {{0, 1}, {0, 0}}));

    for (int n = 0; n <= 3; ++n) {
        int patterns = 1;
        for (int i = 0; i < n * (n + 1) / 2; ++i) {
            patterns *= 3;
        }

        std::vector<std::vector<std::pair<int, int>>> graphs(patterns);
        std::vector<std::vector<int>> signatures(patterns);
        for (int code = 0; code < patterns; ++code) {
            graphs[code] = graph_from_code(n, code);
            signatures[code] = canonical_signature(n, graphs[code]);
        }

        const auto permutations = all_permutations(n);
        for (int code = 0; code < patterns; ++code) {
            for (const auto &permutation : permutations) {
                assert(is_graph_isomorphic(
                    n, graphs[code], permute_graph(graphs[code], permutation)));
            }
        }

        std::vector<int> representatives;
        for (int code = 0; code < patterns; ++code) {
            bool seen = false;
            for (int representative : representatives) {
                if (signatures[code] == signatures[representative]) {
                    seen = true;
                }
            }
            if (!seen) {
                representatives.push_back(code);
            }
        }

        if (n <= 2) {
            for (int a = 0; a < patterns; ++a) {
                for (int b = 0; b < patterns; ++b) {
                    const bool expected = signatures[a] == signatures[b];
                    const bool actual =
                        is_graph_isomorphic(n, graphs[a], graphs[b]);
                    assert(expected == actual);
                }
            }
        } else {
            for (int a : representatives) {
                for (int b : representatives) {
                    const bool expected = signatures[a] == signatures[b];
                    const bool actual =
                        is_graph_isomorphic(n, graphs[a], graphs[b]);
                    assert(expected == actual);
                }
            }
        }
    }

    const std::vector<std::pair<int, int>> cycle_6{{0, 1}, {1, 2}, {2, 3},
                                                   {3, 4}, {4, 5}, {5, 0}};
    const std::vector<std::pair<int, int>> two_triangles{
        {0, 1}, {1, 2}, {2, 0}, {3, 4}, {4, 5}, {5, 3}};
    GraphIsomorphism regular_graphs(6, cycle_6, two_triangles);
    assert(!regular_graphs.run());
    assert(!regular_graphs.run());

    const std::vector<std::pair<int, int>> complete_bipartite_3_3{
        {0, 3}, {0, 4}, {0, 5}, {1, 3}, {1, 4}, {1, 5}, {2, 3}, {2, 4}, {2, 5}};
    const std::vector<std::pair<int, int>> triangular_prism{
        {0, 1}, {1, 2}, {2, 0}, {3, 4}, {4, 5}, {5, 3}, {0, 3}, {1, 4}, {2, 5}};
    assert(!is_graph_isomorphic(6, complete_bipartite_3_3, triangular_prism));

    const std::vector<std::pair<int, int>> edges{{0, 1}, {1, 2}, {2, 3},
                                                 {3, 4}, {4, 0}, {2, 2}};
    const std::vector<int> permutation{2, 4, 1, 3, 0};
    assert(is_graph_isomorphic(5, edges, permute_graph(edges, permutation)));
}

int main() {
    self_test();

    return 0;
}
