// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <limits>
#include <utility>
#include <vector>

#include "../graph/tree/01-on-tree.hpp"

long long brute_01_on_tree(int n, const std::vector<std::pair<int, int>> &edges,
                           const std::vector<long long> &c0,
                           const std::vector<long long> &c1, int root) {
    std::vector<std::vector<int>> graph(n);
    for (const auto &[u, v] : edges) {
        graph[u].push_back(v);
        graph[v].push_back(u);
    }

    std::vector<int> parent(n, -2), stack{root};
    parent[root] = -1;
    while (!stack.empty()) {
        const int v = stack.back();
        stack.pop_back();
        for (int to : graph[v]) {
            if (to == parent[v]) {
                continue;
            }
            parent[to] = v;
            stack.push_back(to);
        }
    }

    std::vector<int> order(n), position(n);
    for (int v = 0; v < n; ++v) {
        order[v] = v;
    }

    long long answer = std::numeric_limits<long long>::max();
    do {
        for (int i = 0; i < n; ++i) {
            position[order[i]] = i;
        }
        bool valid = true;
        for (int v = 0; v < n; ++v) {
            if (parent[v] != -1 && position[parent[v]] > position[v]) {
                valid = false;
            }
        }
        if (!valid) {
            continue;
        }

        long long current = 0;
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                current += c1[order[i]] * c0[order[j]];
            }
        }
        answer = std::min(answer, current);
    } while (std::next_permutation(order.begin(), order.end()));
    return answer;
}

void test_01_on_tree(int n, const std::vector<std::pair<int, int>> &edges,
                     const std::vector<long long> &c0,
                     const std::vector<long long> &c1, int root) {
    const long long expected = brute_01_on_tree(n, edges, c0, c1, root);
    const long long actual = solve_01_on_tree(n, edges, c0, c1, root);
    assert(actual == expected);
}

void test_all_small_counts(int n,
                           const std::vector<std::pair<int, int>> &edges) {
    int pattern_count = 1;
    for (int i = 0; i < 2 * n; ++i) {
        pattern_count *= 3;
    }
    for (int code = 0; code < pattern_count; ++code) {
        int current_code = code;
        std::vector<long long> c0(n), c1(n);
        for (int v = 0; v < n; ++v) {
            c0[v] = current_code % 3;
            current_code /= 3;
            c1[v] = current_code % 3;
            current_code /= 3;
        }
        test_01_on_tree(n, edges, c0, c1, 0);
    }
}

void enumerate_parent_trees(int n, int v,
                            std::vector<std::pair<int, int>> &edges) {
    if (v == n) {
        test_all_small_counts(n, edges);
        return;
    }
    for (int parent = 0; parent < v; ++parent) {
        edges.push_back({parent, v});
        enumerate_parent_trees(n, v + 1, edges);
        edges.pop_back();
    }
}

int main() {
    for (int n = 1; n <= 4; ++n) {
        std::vector<std::pair<int, int>> edges;
        enumerate_parent_trees(n, 1, edges);
    }

    const std::vector<std::pair<int, int>> edges{
        {0, 1}, {0, 2}, {2, 3}, {2, 4}};
    const std::vector<long long> c0{0, 3, 0, 2, 1};
    const std::vector<long long> c1{0, 0, 4, 1, 0};
    for (int root = 0; root < 5; ++root) {
        test_01_on_tree(5, edges, c0, c1, root);
    }

    const std::vector<unsigned long long> large_c0{0, 4'000'000'000ULL};
    const std::vector<unsigned long long> large_c1{4'000'000'000ULL, 0};
    const auto large_answer = solve_01_on_tree(2, {{0, 1}}, large_c0, large_c1);
    assert(large_answer == 16'000'000'000'000'000'000ULL);

    return 0;
}
