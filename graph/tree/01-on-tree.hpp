#ifndef GRAPH_TREE_01_ON_TREE_HPP
#define GRAPH_TREE_01_ON_TREE_HPP

// 根付き木上で、親が子より左に出る順序の 01 列の転倒数最小値を求める。
// 頂点 v には c0[v] 個の 0 の後に c1[v] 個の 1 を置いた列が書かれている。
// 辺は無向辺の両端で与え、root を根として親子関係を定める。
// c0[v], c1[v] は非負の個数である。計算量は O(n log n) である。

#include <cassert>
#include <queue>
#include <type_traits>
#include <utility>
#include <vector>

template <class T = void, class Count>
std::conditional_t<std::is_void_v<T>, Count, T>
solve_01_on_tree(int n, const std::vector<std::pair<int, int>> &edges,
                 const std::vector<Count> &c0, const std::vector<Count> &c1,
                 int root = 0) {
    using Answer = std::conditional_t<std::is_void_v<T>, Count, T>;
    using CompareType =
        std::conditional_t<(sizeof(Count) < sizeof(long long)),
                           std::conditional_t<std::is_signed_v<Count>,
                                              long long, unsigned long long>,
                           Count>;

    assert(n >= 1);
    assert(static_cast<int>(edges.size()) == n - 1);
    assert(static_cast<int>(c0.size()) == n);
    assert(static_cast<int>(c1.size()) == n);
    assert(0 <= root && root < n);

    std::vector<std::vector<int>> graph(n);
    for (int v = 0; v < n; ++v) {
        assert(!(c0[v] < Count{}));
        assert(!(c1[v] < Count{}));
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
        Count zero;
        Count one;
    };

    struct QueueCompare {
        bool operator()(const QueueNode &a, const QueueNode &b) const {
            if (a.one == Count{} && b.one == Count{}) {
                return a.vertex < b.vertex;
            }
            if (a.one == Count{}) {
                return false;
            }
            if (b.one == Count{}) {
                return true;
            }
            const CompareType lhs = static_cast<CompareType>(a.zero) *
                                    static_cast<CompareType>(b.one);
            const CompareType rhs = static_cast<CompareType>(b.zero) *
                                    static_cast<CompareType>(a.one);
            if (lhs < rhs) {
                return true;
            }
            if (rhs < lhs) {
                return false;
            }
            return a.vertex < b.vertex;
        }
    };

    std::vector<int> leader(n), up = parent, version(n, 0);
    std::vector<Count> zero = c0, one = c1;
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

    Answer answer{};
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

        answer += static_cast<Answer>(one[p]) * static_cast<Answer>(zero[v]);
        zero[p] += zero[v];
        one[p] += one[v];
        leader[v] = p;
        ++version[p];
        push(p);
    }

    return answer;
}

#endif
