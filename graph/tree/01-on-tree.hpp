#ifndef GRAPH_TREE_01_ON_TREE_HPP
#define GRAPH_TREE_01_ON_TREE_HPP

// 根付き木上で、親が子より左に出る順序の 01 列の転倒数最小値を求める。
// 頂点 v には c[v] 個の 0 の後に 1 を置いた列が書かれている。
// 辺は無向辺の両端で与え、root を根として親子関係を定める。
// 計算量は O(n log n) である。

#include <cassert>
#include <queue>
#include <utility>
#include <vector>

template <class T>
T solve_01_on_tree(int n, const std::vector<std::pair<int, int>> &edges,
                   const std::vector<T> &c, int root = 0) {
    assert(n >= 1);
    assert(static_cast<int>(edges.size()) == n - 1);
    assert(static_cast<int>(c.size()) == n);
    assert(0 <= root && root < n);

    std::vector<std::vector<int>> graph(n);
    for (int v = 0; v < n; ++v) {
        assert(!(c[v] < T{}));
    }
    for (const auto &[from, to] : edges) {
        assert(0 <= from && from < n);
        assert(0 <= to && to < n);
        assert(from != to);
        graph[from].push_back(to);
        graph[to].push_back(from);
    }

    std::vector<int> parent(n, -2);
    parent[root] = -1;
    int visited = 0;
    std::vector<int> stack{root};
    while (!stack.empty()) {
        const int v = stack.back();
        stack.pop_back();
        ++visited;
        for (int to : graph[v]) {
            if (to == parent[v]) {
                continue;
            }
            assert(parent[to] == -2);
            parent[to] = v;
            stack.push_back(to);
        }
    }
    assert(visited == n);

    struct QueueNode {
        int vertex;
        int version;
        T zero;
        int one;
    };

    struct QueueCompare {
        bool operator()(const QueueNode &a, const QueueNode &b) const {
            const T lhs = a.zero * static_cast<T>(b.one);
            const T rhs = b.zero * static_cast<T>(a.one);
            if (lhs < rhs) {
                return true;
            }
            if (rhs < lhs) {
                return false;
            }
            return a.vertex < b.vertex;
        }
    };

    std::vector<int> leader(n), one(n, 1), up = parent, version(n, 0);
    std::vector<T> zero = c;
    for (int v = 0; v < n; ++v) {
        leader[v] = v;
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

    T answer{};
    for (int merge_count = 0; merge_count < n - 1; ++merge_count) {
        QueueNode current{};
        while (true) {
            assert(!que.empty());
            current = que.top();
            que.pop();
            const int v = find(current.vertex);
            if (v == current.vertex && current.version == version[v]) {
                break;
            }
        }

        const int v = current.vertex;
        const int p = find(up[v]);
        assert(p != v);

        answer += static_cast<T>(one[p]) * zero[v];
        zero[p] += zero[v];
        one[p] += one[v];
        leader[v] = p;
        ++version[p];
        push(p);
    }

    return answer;
}

#endif
