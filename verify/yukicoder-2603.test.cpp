// competitive-verifier: PROBLEM https://yukicoder.me/problems/no/2603

#include <cassert>
#include <iostream>
#include <queue>
#include <vector>

#include "../other/minimum-mod-range-increment-decrement-operations.hpp"

namespace {

int encode(const std::vector<int> &values, int mod) {
    int state = 0;
    int base = 1;
    for (int x : values) {
        state += x * base;
        base *= mod;
    }
    return state;
}

std::vector<int> decode(int state, int n, int mod) {
    std::vector<int> values(n);
    for (int i = 0; i < n; ++i) {
        values[i] = state % mod;
        state /= mod;
    }
    return values;
}

std::vector<int> all_distances(int n, int mod) {
    int total = 1;
    for (int i = 0; i < n; ++i) {
        total *= mod;
    }

    std::vector<int> dist(total, -1);
    std::queue<int> q;
    dist[0] = 0;
    q.push(0);

    while (!q.empty()) {
        const int state = q.front();
        q.pop();

        const auto values = decode(state, n, mod);
        for (int l = 0; l < n; ++l) {
            for (int r = l; r < n; ++r) {
                for (int delta : {-1, 1}) {
                    auto next = values;
                    for (int i = l; i <= r; ++i) {
                        next[i] += delta;
                        if (next[i] == mod) {
                            next[i] = 0;
                        }
                        if (next[i] < 0) {
                            next[i] += mod;
                        }
                    }
                    const int next_state = encode(next, mod);
                    if (dist[next_state] != -1) {
                        continue;
                    }
                    dist[next_state] = dist[state] + 1;
                    q.push(next_state);
                }
            }
        }
    }

    return dist;
}

long long mod_difference_ll(long long from, long long to, long long mod) {
    return from <= to ? to - from : to + mod - from;
}

void self_test() {
    for (int n = 1; n <= 4; ++n) {
        for (int mod = 1; mod <= 3; ++mod) {
            int total = 1;
            for (int i = 0; i < n; ++i) {
                total *= mod;
            }

            const auto dist = all_distances(n, mod);
            for (int a_state = 0; a_state < total; ++a_state) {
                const auto a_values = decode(a_state, n, mod);
                std::vector<long long> a(n);
                for (int i = 0; i < n; ++i) {
                    a[i] = a_values[i];
                }

                for (int b_state = 0; b_state < total; ++b_state) {
                    const auto b_values = decode(b_state, n, mod);
                    std::vector<long long> b(n);
                    std::vector<int> delta_values(n);
                    for (int i = 0; i < n; ++i) {
                        b[i] = b_values[i];
                        delta_values[i] = static_cast<int>(
                            mod_difference_ll(a[i], b[i], mod));
                    }
                    assert(minimum_mod_range_increment_decrement_operations(
                               a, b, static_cast<long long>(mod)) ==
                           dist[encode(delta_values, mod)]);
                }
            }
        }
    }

    for (int mod = 1; mod <= 4; ++mod) {
        const int n = 5;
        int total = 1;
        for (int i = 0; i < n; ++i) {
            total *= mod;
        }

        const auto dist = all_distances(n, mod);
        std::vector<long long> a(n, 0);
        for (int b_state = 0; b_state < total; ++b_state) {
            const auto b_values = decode(b_state, n, mod);
            std::vector<long long> b(n);
            for (int i = 0; i < n; ++i) {
                b[i] = b_values[i];
            }
            assert(minimum_mod_range_increment_decrement_operations(
                       a, b, static_cast<long long>(mod)) == dist[b_state]);
        }
    }
}

} // namespace

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

#ifndef ONLINE_JUDGE
    self_test();
#endif

    int n;
    long long m;
    std::cin >> n >> m;

    std::vector<long long> a(n), b(n);
    for (int i = 0; i < n; ++i) {
        std::cin >> a[i];
    }
    for (int i = 0; i < n; ++i) {
        std::cin >> b[i];
    }

    std::cout << minimum_mod_range_increment_decrement_operations(a, b, m)
              << '\n';
    return 0;
}
