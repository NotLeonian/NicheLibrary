#ifndef GRAPH_TREE_ZERO_ONE_ON_TREE_HPP
#define GRAPH_TREE_ZERO_ONE_ON_TREE_HPP

// 根付き木上で、親が子より左に出る順序の 01 列の転倒数最小値を求める。
// 頂点 v には c0[v] 個の 0 の後に c1[v] 個の 1 を置いた列が書かれている。
// 辺は無向辺の両端で与え、root を根として親子関係を定める。
// c0[v], c1[v] は非負の個数である。計算量は O(n log n) である。

#include <cassert>
#include <queue>
#include <type_traits>
#include <utility>
#include <vector>

namespace zero_one_on_tree_impl {
template <class InputCount, class Preferred>
using count_type_t = std::conditional_t<
    !std::is_class_v<Preferred> && !std::is_floating_point_v<Preferred> &&
        (sizeof(InputCount) < sizeof(Preferred)),
    Preferred,
    std::conditional_t<(sizeof(InputCount) < sizeof(long long)),
                       std::conditional_t<std::is_signed_v<InputCount>,
                                          long long, unsigned long long>,
                       InputCount>>;

template <class Count>
int compare_fraction(Count a_num, Count a_den, Count b_num, Count b_den) {
    bool reversed = false;
    while (true) {
        const Count a_quot = a_num / a_den;
        const Count b_quot = b_num / b_den;
        if (a_quot != b_quot) {
            const int result = a_quot < b_quot ? -1 : 1;
            return reversed ? -result : result;
        }

        a_num %= a_den;
        b_num %= b_den;
        if (a_num == Count{} || b_num == Count{}) {
            int result = 0;
            if (a_num != b_num) {
                result = a_num == Count{} ? -1 : 1;
            }
            return reversed ? -result : result;
        }

        std::swap(a_num, a_den);
        std::swap(b_num, b_den);
        reversed = !reversed;
    }
}
} // namespace zero_one_on_tree_impl

template <class T = void, class InputCount>
std::conditional_t<std::is_void_v<T>,
                   zero_one_on_tree_impl::count_type_t<InputCount, InputCount>,
                   T>
solve_01_on_tree(int n, const std::vector<std::pair<int, int>> &edges,
                 const std::vector<InputCount> &c0,
                 const std::vector<InputCount> &c1, int root = 0) {
    using Preferred = std::conditional_t<std::is_void_v<T>, InputCount, T>;
    using Count = zero_one_on_tree_impl::count_type_t<InputCount, Preferred>;
    using Answer = std::conditional_t<std::is_void_v<T>, Count, T>;

    assert(n >= 1);
    assert(static_cast<int>(edges.size()) == n - 1);
    assert(static_cast<int>(c0.size()) == n);
    assert(static_cast<int>(c1.size()) == n);
    assert(0 <= root && root < n);

    std::vector<std::vector<int>> graph(n);
    std::vector<Count> zero(n), one(n);
    for (int v = 0; v < n; ++v) {
        assert(!(c0[v] < InputCount{}));
        assert(!(c1[v] < InputCount{}));
        zero[v] = static_cast<Count>(c0[v]);
        one[v] = static_cast<Count>(c1[v]);
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
    [[maybe_unused]] int visited = 0;
    std::vector<int> stack;
    stack.reserve(n);
    stack.push_back(root);
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
            if (a.zero == b.zero && a.one == b.one) {
                return a.vertex < b.vertex;
            }
            if (a.one == Count{} && b.one == Count{}) {
                return a.vertex < b.vertex;
            }
            if (a.one == Count{}) {
                return false;
            }
            if (b.one == Count{}) {
                return true;
            }
            if (a.zero == Count{} && b.zero == Count{}) {
                return a.vertex < b.vertex;
            }
            if (a.zero == Count{}) {
                return true;
            }
            if (b.zero == Count{}) {
                return false;
            }
            const int result = zero_one_on_tree_impl::compare_fraction(
                a.zero, a.one, b.zero, b.one);
            if (result < 0) {
                return true;
            }
            if (result > 0) {
                return false;
            }
            return a.vertex < b.vertex;
        }
    };

    std::vector<int> leader(n), up = std::move(parent), version(n, 0);
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

    std::vector<QueueNode> queue_storage;
    queue_storage.reserve(n);
    std::priority_queue<QueueNode, std::vector<QueueNode>, QueueCompare> que(
        QueueCompare{}, std::move(queue_storage));
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
        int v = -1;
        while (true) {
            const QueueNode &current = que.top();
            const int current_vertex = current.vertex;
            const int current_version = current.version;
            que.pop();
            v = find(current_vertex);
            if (v == current_vertex && current_version == version[v]) {
                break;
            }
        }

        const int p = find(up[v]);

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
