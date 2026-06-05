// competitive-verifier: STANDALONE

#include <array>
#include <cassert>
#include <utility>
#include <vector>

#include "../graph/tree/hl-rec-dp.hpp"

struct TestSpec {
    static constexpr int K = 3;
    using State = std::array<long long, 2>;
    using Pack = std::array<State, K>;

    std::vector<long long> value;
    std::vector<int> before_vertex_order;
    std::vector<int> after_vertex_order;
    std::vector<Pack> before_pack;
    std::vector<Pack> after_pack;

    explicit TestSpec(std::vector<long long> value) : value(std::move(value)) {}

    Pack make_pack(int v, const State &in) {
        Pack pack{};
        for (int lane = 0; lane < K; ++lane) {
            pack[lane] = {in[0] + value[v] * (lane + 1) - lane,
                          in[1] - value[v] + lane * 3};
        }
        return pack;
    }

    Pack take_heavy(int v, int child, Pack &&child_pack) {
        for (int lane = 0; lane < K; ++lane) {
            child_pack[lane][0] += value[v] - 2 * value[child] + lane;
            child_pack[lane][1] = child_pack[lane][1] * 2 + v - child - lane;
        }
        return std::move(child_pack);
    }

    State take_light(int v, int child, int lane, Pack &&child_pack) {
        State res = child_pack[(lane + 1) % K];
        res[0] += child_pack[lane][1] + value[v] - value[child] + lane;
        res[1] -= child_pack[(lane + 2) % K][0] - v + child;
        return res;
    }

    void before_vertex(int v, const Pack &pack) {
        before_vertex_order.push_back(v);
        before_pack.push_back(pack);
    }

    void add_vertex(int v, Pack &pack) {
        for (int lane = 0; lane < K; ++lane) {
            pack[lane][0] += value[v] + 5 * lane;
            pack[lane][1] -= value[v] * (lane + 2) - 3;
        }
    }

    void after_vertex(int v, const Pack &pack) {
        after_vertex_order.push_back(v);
        after_pack.push_back(pack);
    }
};

std::vector<std::vector<int>>
make_children(int n, const std::vector<std::pair<int, int>> &edges, int root) {
    std::vector<std::vector<int>> graph(n), child(n);
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
            assert(parent[to] == -2);
            parent[to] = v;
            child[v].push_back(to);
            stack.push_back(to);
        }
    }
    return child;
}

void put_heavy_child_first(std::vector<std::vector<int>> &child, int root) {
    const int n = static_cast<int>(child.size());
    std::vector<int> order, stack{root};
    order.reserve(n);
    while (!stack.empty()) {
        const int v = stack.back();
        stack.pop_back();
        order.push_back(v);
        for (int to : child[v]) {
            stack.push_back(to);
        }
    }
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

struct NaiveRunner {
    using State = TestSpec::State;
    using Pack = TestSpec::Pack;

    std::vector<std::vector<int>> child;
    TestSpec &spec;

    NaiveRunner(std::vector<std::vector<int>> child, TestSpec &spec)
        : child(std::move(child)), spec(spec) {}

    Pack dfs(int v, const State &in, bool collect) {
        Pack cur{};
        if (child[v].empty()) {
            cur = spec.make_pack(v, in);
        } else {
            const int heavy = child[v][0];
            cur = spec.take_heavy(v, heavy, dfs(heavy, in, collect));
            for (int idx = 1; idx < static_cast<int>(child[v].size()); ++idx) {
                const int to = child[v][idx];
                for (int lane = 0; lane < TestSpec::K; ++lane) {
                    cur[lane] =
                        spec.take_light(v, to, lane, dfs(to, cur[lane], false));
                }
            }
        }
        if (collect) {
            spec.before_vertex(v, cur);
        }
        spec.add_vertex(v, cur);
        if (collect) {
            spec.after_vertex(v, cur);
            for (int idx = 1; idx < static_cast<int>(child[v].size()); ++idx) {
                dfs(child[v][idx], in, true);
            }
        }
        return cur;
    }
};

void check_result(const TestSpec &expected_spec,
                  const TestSpec::Pack &expected_pack,
                  const TestSpec &actual_spec,
                  const TestSpec::Pack &actual_pack) {
    assert(expected_pack == actual_pack);
    assert(expected_spec.before_vertex_order ==
           actual_spec.before_vertex_order);
    assert(expected_spec.after_vertex_order == actual_spec.after_vertex_order);
    assert(expected_spec.before_pack == actual_spec.before_pack);
    assert(expected_spec.after_pack == actual_spec.after_pack);
}

std::vector<long long> make_values(int n) {
    std::vector<long long> value(n);
    for (int i = 0; i < n; ++i) {
        value[i] = (i % 2 == 0 ? 1 : -1) * (i % 4) - 2;
    }
    return value;
}

void test_tree(int n, const std::vector<std::pair<int, int>> &edges) {
    const auto value = make_values(n);
    for (int root = 0; root < n; ++root) {
        auto child = make_children(n, edges, root);
        put_heavy_child_first(child, root);
        const TestSpec::State initial{-3 + root, 7 - 2 * root};

        TestSpec expected_spec(value);
        NaiveRunner naive(child, expected_spec);
        const auto expected_pack = naive.dfs(root, initial, true);

        TestSpec actual_spec(value);
        const auto actual_pack =
            hl_rec_dp(n, edges, root, initial, actual_spec);
        check_result(expected_spec, expected_pack, actual_spec, actual_pack);
    }
}

void enumerate_parent_trees(int n, int v,
                            std::vector<std::pair<int, int>> &edges) {
    if (v == n) {
        test_tree(n, edges);
        return;
    }
    for (int p = 0; p < v; ++p) {
        edges.push_back({p, v});
        enumerate_parent_trees(n, v + 1, edges);
        edges.pop_back();
    }
}

std::vector<std::pair<int, int>> decode_prufer(int n,
                                               const std::vector<int> &code) {
    std::vector<int> degree(n, 1);
    for (int v : code) {
        ++degree[v];
    }
    std::vector<std::pair<int, int>> edges;
    edges.reserve(n - 1);
    for (int v : code) {
        int leaf = -1;
        for (int i = 0; i < n; ++i) {
            if (degree[i] == 1) {
                leaf = i;
                break;
            }
        }
        assert(leaf != -1);
        edges.push_back({leaf, v});
        --degree[leaf];
        --degree[v];
    }
    int a = -1, b = -1;
    for (int i = 0; i < n; ++i) {
        if (degree[i] == 1) {
            (a == -1 ? a : b) = i;
        }
    }
    assert(a != -1 && b != -1);
    edges.push_back({a, b});
    return edges;
}

void enumerate_prufer(int n, int pos, std::vector<int> &code) {
    if (pos == n - 2) {
        test_tree(n, decode_prufer(n, code));
        return;
    }
    for (int v = 0; v < n; ++v) {
        code[pos] = v;
        enumerate_prufer(n, pos + 1, code);
    }
}

void self_test() {
    test_tree(1, {});
    for (int n = 2; n <= 5; ++n) {
        std::vector<int> code(n - 2);
        enumerate_prufer(n, 0, code);
    }
    for (int n = 2; n <= 7; ++n) {
        std::vector<std::pair<int, int>> edges;
        enumerate_parent_trees(n, 1, edges);
    }
}

int main() {
    self_test();
    return 0;
}
