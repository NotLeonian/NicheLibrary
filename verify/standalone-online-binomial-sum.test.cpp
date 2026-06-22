// competitive-verifier: STANDALONE

#include <cassert>
#include <vector>

#include "../math/combinatorics/online-binomial-sum.hpp"

// 小さい m で、r = -2,-1,0,1,3 と境界をまたぐ n,l,u を愚直解と比較する。
// n > m + 1、u > m + 1、空区間、l > m の範囲を検証する。

long long brute_prefix_sum(const std::vector<std::vector<long long>> &binom,
                           int n, int m, long long r) {
    long long ans = 0;
    long long pow_r = 1;
    for (int i = 0; i <= m; ++i) {
        if (i >= n) {
            break;
        }
        ans += pow_r * binom[m][i];
        pow_r *= r;
    }
    return ans;
}

long long brute_sum(const std::vector<std::vector<long long>> &binom, int l,
                    int u, int m, long long r) {
    long long ans = 0;
    long long pow_r = 1;
    for (int i = 0; i <= m; ++i) {
        if (l <= i && i < u) {
            ans += pow_r * binom[m][i];
        }
        pow_r *= r;
    }
    return ans;
}

int main() {
    constexpr int max_m = 30;
    std::vector<std::vector<long long>> binom(
        max_m + 1, std::vector<long long>(max_m + 1));
    for (int n = 0; n <= max_m; ++n) {
        binom[n][0] = binom[n][n] = 1;
        for (int k = 1; k < n; ++k) {
            binom[n][k] = binom[n - 1][k - 1] + binom[n - 1][k];
        }
    }

    for (long long r : {-2, -1, 0, 1, 3}) {
        OnlineBinomialSum<long long> online_binomial_sum(max_m, r);
        for (int m = 0; m <= max_m; ++m) {
            for (int n = 0; n <= max_m + 10; ++n) {
                assert(online_binomial_sum.binom_prefix_sum(n, m) ==
                       brute_prefix_sum(binom, n, m, r));
            }
            for (int l = 0; l <= max_m + 5; ++l) {
                for (int u = l; u <= max_m + 10; ++u) {
                    assert(online_binomial_sum.binom_sum(l, u, m) ==
                           brute_sum(binom, l, u, m, r));
                }
            }
        }
    }
    return 0;
}
