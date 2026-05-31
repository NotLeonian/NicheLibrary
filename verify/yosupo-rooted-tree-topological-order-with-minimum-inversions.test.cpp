// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/rooted_tree_topological_order_with_minimum_inversions

#include <iostream>
#include <queue>
#include <utility>
#include <vector>

#include "../graph/tree/01-on-tree.hpp"

std::vector<int>
build_01_on_tree_order(int n, const std::vector<std::pair<int, int>> &edges,
                       const std::vector<long long> &c0,
                       const std::vector<long long> &c1, int root) {
    std::vector<std::vector<int>> graph(n);
    for (const auto &[from, to] : edges) {
        graph[from].push_back(to);
        graph[to].push_back(from);
    }

    std::vector<int> parent(n, -2);
    parent[root] = -1;
    std::vector<int> stack{root};
    while (!stack.empty()) {
        const int v = stack.back();
        stack.pop_back();
        for (int to : graph[v]) {
            if (to == parent[v]) {
                continue;
            }
            if (parent[to] != -2) {
                continue;
            }
            parent[to] = v;
            stack.push_back(to);
        }
    }

    struct QueueNode {
        int vertex;
        int version;
        long long zero;
        long long one;
    };

    struct QueueCompare {
        bool operator()(const QueueNode &a, const QueueNode &b) const {
            if (a.one == 0 && b.one == 0) {
                return a.vertex < b.vertex;
            }
            if (a.one == 0) {
                return false;
            }
            if (b.one == 0) {
                return true;
            }
            const long long lhs = a.zero * b.one;
            const long long rhs = b.zero * a.one;
            if (lhs < rhs) {
                return true;
            }
            if (rhs < lhs) {
                return false;
            }
            return a.vertex < b.vertex;
        }
    };

    std::vector<int> leader(n), up = parent, version(n, 0), head(n), tail(n),
                                next(n, -1);
    std::vector<long long> zero = c0, one = c1;
    for (int v = 0; v < n; ++v) {
        leader[v] = v;
        head[v] = v;
        tail[v] = v;
    }

    auto find = [&](int v) {
        while (leader[v] != v) {
            leader[v] = leader[leader[v]];
            v = leader[v];
        }
        return v;
    };

    std::priority_queue<QueueNode, std::vector<QueueNode>, QueueCompare> que;
    auto push = [&](int v) {
        if (up[v] != -1) {
            que.push(QueueNode{v, version[v], zero[v], one[v]});
        }
    };

    for (int v = 0; v < n; ++v) {
        push(v);
    }

    for (int merge_count = 0; merge_count < n - 1; ++merge_count) {
        QueueNode current{};
        while (true) {
            current = que.top();
            que.pop();
            const int v = find(current.vertex);
            if (v == current.vertex && current.version == version[v]) {
                break;
            }
        }

        const int v = current.vertex;
        const int p = find(up[v]);
        next[tail[p]] = head[v];
        tail[p] = tail[v];
        zero[p] += zero[v];
        one[p] += one[v];
        leader[v] = p;
        ++version[p];
        push(p);
    }

    std::vector<int> order;
    order.reserve(n);
    for (int v = head[root]; v != -1; v = next[v]) {
        order.push_back(v);
    }
    return order;
}

int main() {
    int n;
    std::cin >> n;

    const int root = n;
    std::vector<std::pair<int, int>> edges;
    edges.reserve(n);
    for (int i = 1; i < n; ++i) {
        int p;
        std::cin >> p;
        edges.push_back({p, i});
    }
    edges.push_back({root, 0});

    std::vector<long long> c0(n + 1, 0), c1(n + 1, 0);
    for (int i = 0; i < n; ++i) {
        std::cin >> c0[i];
    }
    for (int i = 0; i < n; ++i) {
        std::cin >> c1[i];
    }

    const long long answer =
        solve_01_on_tree<long long>(n + 1, edges, c0, c1, root);
    const std::vector<int> order =
        build_01_on_tree_order(n + 1, edges, c0, c1, root);

    std::cout << answer << '\n';
    bool first = true;
    for (int v : order) {
        if (v == root) {
            continue;
        }
        if (!first) {
            std::cout << ' ';
        }
        first = false;
        std::cout << v;
    }
    std::cout << '\n';
    return 0;
}
