#ifndef MATH_COMBINATORICS_ONLINE_BINOMIAL_SUM_HPP
#define MATH_COMBINATORICS_ONLINE_BINOMIAL_SUM_HPP

// Σ_{i=l}^{u-1} r^i binom(m,i) をオンラインで求める。
// 0 <= l <= u と 0 <= m <= max_m を仮定する。
// m のバケット境界の累積和と、バケット内の二項係数を前計算する。
// T は四則演算を持つ型で、std::numeric_limits<T>::is_integer が
// false の場合は T(1) から T(max_m) で除算できる。
// 計算量は前計算 O(max_m √max_m)、クエリ O(√max_m)。

#include <cassert>
#include <limits>
#include <vector>

template <class T> struct OnlineBinomialSum {
    int max_m;
    int bucket_size;
    T r;
    std::vector<int> prefix_sum_offset;
    std::vector<T> prefix_sum_table;
    std::vector<int> weighted_binomial_offset;
    std::vector<T> weighted_binomial_table;

    explicit OnlineBinomialSum(int m, T r = T(1)) : max_m(m), r(r) {
        assert(max_m >= 0);
        bucket_size = 1;
        while (1LL * bucket_size * bucket_size < max_m + 1) {
            bucket_size <<= 1;
        }

        weighted_binomial_offset.assign(bucket_size + 1, 0);
        for (int d = 0; d < bucket_size; ++d) {
            weighted_binomial_offset[d + 1] =
                weighted_binomial_offset[d] + d + 1;
        }
        weighted_binomial_table.assign(weighted_binomial_offset[bucket_size],
                                       T());
        weighted_binomial_table[0] = T(1);
        for (int d = 1; d < bucket_size; ++d) {
            weighted_binomial_table[weighted_binomial_offset[d]] = T(1);
            for (int j = 1; j < d; ++j) {
                weighted_binomial_table[weighted_binomial_offset[d] + j] =
                    weighted_binomial_table[weighted_binomial_offset[d - 1] +
                                            j] +
                    r * weighted_binomial_table
                            [weighted_binomial_offset[d - 1] + j - 1];
            }
            weighted_binomial_table[weighted_binomial_offset[d] + d] =
                r * weighted_binomial_table[weighted_binomial_offset[d - 1] +
                                            d - 1];
        }

        const int bucket_count = max_m / bucket_size + 1;
        prefix_sum_offset.assign(bucket_count + 1, 0);
        for (int b = 0; b < bucket_count; ++b) {
            const int base = b * bucket_size;
            int last = base + bucket_size;
            if (last > max_m + 1) {
                last = max_m + 1;
            }
            prefix_sum_offset[b + 1] = prefix_sum_offset[b] + last + 1;
        }
        prefix_sum_table.assign(prefix_sum_offset[bucket_count], T());

        std::vector<T> integer_inverse;
        if constexpr (!std::numeric_limits<T>::is_integer) {
            integer_inverse.assign(max_m + 1, T());
            for (int i = 1; i <= max_m; ++i) {
                integer_inverse[i] = T(1) / T(i);
            }
        }

        for (int b = 0; b < bucket_count; ++b) {
            const int base = b * bucket_size;
            const int offset = prefix_sum_offset[b];
            const int row_size = prefix_sum_offset[b + 1] - offset;
            T sum = T();
            T term = T(1);
            prefix_sum_table[offset] = T();
            for (int i = 0; i <= base; ++i) {
                sum += term;
                prefix_sum_table[offset + i + 1] = sum;
                if (i < base) {
                    term *= r;
                    term *= T(base - i);
                    if constexpr (std::numeric_limits<T>::is_integer) {
                        term /= T(i + 1);
                    } else {
                        term *= integer_inverse[i + 1];
                    }
                }
            }
            for (int i = base + 2; i < row_size; ++i) {
                prefix_sum_table[offset + i] = sum;
            }
        }
    }

    T binom_prefix_sum(int n, int m) const {
        assert(n >= 0);
        assert(m >= 0);
        assert(m <= max_m);
        if (n == 0) {
            return T();
        }
        if (n > m) {
            n = m + 1;
        }
        const int bucket = m / bucket_size;
        const int base = bucket * bucket_size;
        const int d = m - base;
        const int prefix_offset = prefix_sum_offset[bucket];
        const int weight_offset = weighted_binomial_offset[d];
        T ans = T();
        for (int j = 0; j <= d && j < n; ++j) {
            ans += weighted_binomial_table[weight_offset + j] *
                   prefix_sum_table[prefix_offset + n - j];
        }
        return ans;
    }

    T binom_sum(int l, int u, int m) const {
        assert(l >= 0);
        assert(l <= u);
        return binom_prefix_sum(u, m) - binom_prefix_sum(l, m);
    }
};

#endif
