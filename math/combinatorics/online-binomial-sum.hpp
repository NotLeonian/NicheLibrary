#ifndef MATH_COMBINATORICS_ONLINE_BINOMIAL_SUM_HPP
#define MATH_COMBINATORICS_ONLINE_BINOMIAL_SUM_HPP

// Σ_{i=l}^{u-1} r^i binom(m,i) をオンラインで求める。
// 0 <= l <= u と 0 <= m <= max_m を仮定する。
// m のバケット境界の累積和を n のバケット境界でサンプルし、
// バケット内の二項係数を前計算する。
// T は四則演算と T() との等値比較を持つ型で、
// r != T() の場合は r で除算できることを前提とする。
// std::numeric_limits<T>::is_integer が false の場合は、
// さらに T(1) から T(max_m) で除算できることを前提とする。
// 時間計算量は前計算 O(max_m √max_m)、クエリ O(√max_m)。
// 空間計算量は O(max_m)。

#include <cassert>
#include <limits>
#include <vector>

template <class T> struct OnlineBinomialSum {
    int max_m;
    int bucket_size;
    T r;
    bool r_is_zero;
    std::vector<int> prefix_sum_offset;
    std::vector<T> prefix_sum_table;
    std::vector<T> prefix_term_table;
    std::vector<int> weighted_binomial_offset;
    std::vector<T> weighted_binomial_table;
    std::vector<T> integer_inverse;
    T r_inverse;

    explicit OnlineBinomialSum(int m, T r = T(1))
        : max_m(m), bucket_size(1), r(r), r_is_zero(r == T()) {
        assert(max_m >= 0);
        if (r_is_zero) {
            return;
        }
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

        if constexpr (!std::numeric_limits<T>::is_integer) {
            r_inverse = T(1) / r;
            integer_inverse.assign(max_m + 1, T());
            for (int i = 1; i <= max_m; ++i) {
                integer_inverse[i] = T(1) / T(i);
            }
        }

        const int bucket_count = max_m / bucket_size + 1;
        prefix_sum_offset.assign(bucket_count + 1, 0);
        for (int b = 0; b < bucket_count; ++b) {
            prefix_sum_offset[b + 1] = prefix_sum_offset[b] + b + 2;
        }

        prefix_sum_table.assign(prefix_sum_offset[bucket_count], T());
        prefix_term_table.assign(prefix_sum_offset[bucket_count], T());

        for (int b = 0; b < bucket_count; ++b) {
            const int base = b * bucket_size;
            const int offset = prefix_sum_offset[b];
            T sum = T();
            T term = T(1);
            for (int i = 0; i <= base; ++i) {
                if (i % bucket_size == 0) {
                    const int q = i / bucket_size;
                    prefix_sum_table[offset + q] = sum;
                    prefix_term_table[offset + q] = term;
                }
                if (i < base) {
                    sum += term;
                    term *= r;
                    term *= T(base - i);
                    if constexpr (std::numeric_limits<T>::is_integer) {
                        term /= T(i + 1);
                    } else {
                        term *= integer_inverse[i + 1];
                    }
                }
            }
            sum += term;
            prefix_sum_table[offset + b + 1] = sum;
            prefix_term_table[offset + b + 1] = T();
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
        if (r_is_zero) {
            return T(1);
        }

        const int bucket = m / bucket_size;
        const int base = bucket * bucket_size;
        const int d = m - base;
        const int prefix_offset = prefix_sum_offset[bucket];
        const int weight_offset = weighted_binomial_offset[d];

        int last_j = d;
        if (last_j >= n) {
            last_j = n - 1;
        }
        const int first_n = n - last_j;

        int sample_index = first_n / bucket_size;
        if (sample_index > bucket + 1) {
            sample_index = bucket + 1;
        }
        int next_sample_index = sample_index + 1;
        if (next_sample_index > bucket + 1) {
            next_sample_index = bucket + 1;
        }
        const auto restore_cost = [&](int index) -> int {
            const int sample_n = index * bucket_size;
            if (sample_n < first_n) {
                return n - sample_n;
            }
            if (sample_n > n) {
                return sample_n - first_n;
            }
            return n - first_n;
        };
        if (restore_cost(next_sample_index) < restore_cost(sample_index)) {
            sample_index = next_sample_index;
        }

        const int sample_n = sample_index * bucket_size;
        const int sample_offset = prefix_offset + sample_index;
        const T base_term = prefix_term_table[prefix_offset + bucket];

        const auto move_right = [&](int current_n, T &sum, T &term) -> void {
            sum += term;
            if (current_n < base) {
                term *= r;
                term *= T(base - current_n);
                if constexpr (std::numeric_limits<T>::is_integer) {
                    term /= T(current_n + 1);
                } else {
                    term *= integer_inverse[current_n + 1];
                }
            } else {
                term = T();
            }
        };
        const auto move_left = [&](int current_n, T &sum, T &term) -> void {
            if (current_n > base + 1) {
                term = T();
                return;
            }
            if (current_n == base + 1) {
                term = base_term;
                sum -= term;
                return;
            }
            if constexpr (std::numeric_limits<T>::is_integer) {
                term /= r;
                term *= T(current_n);
                term /= T(base - current_n + 1);
            } else {
                term *= r_inverse;
                term *= T(current_n);
                term *= integer_inverse[base - current_n + 1];
            }
            sum -= term;
        };

        T sum = prefix_sum_table[sample_offset];
        T term = prefix_term_table[sample_offset];
        T ans = T();
        if (sample_n < first_n) {
            for (int current_n = sample_n; current_n < first_n; ++current_n) {
                move_right(current_n, sum, term);
            }
            for (int current_n = first_n; current_n <= n; ++current_n) {
                const int j = n - current_n;
                ans += weighted_binomial_table[weight_offset + j] * sum;
                if (current_n < n) {
                    move_right(current_n, sum, term);
                }
            }
        } else if (sample_n > n) {
            for (int current_n = sample_n; current_n > n; --current_n) {
                move_left(current_n, sum, term);
            }
            for (int current_n = n; current_n >= first_n; --current_n) {
                const int j = n - current_n;
                ans += weighted_binomial_table[weight_offset + j] * sum;
                if (current_n > first_n) {
                    move_left(current_n, sum, term);
                }
            }
        } else {
            T right_sum = sum;
            T right_term = term;
            for (int current_n = sample_n; current_n <= n; ++current_n) {
                const int j = n - current_n;
                ans += weighted_binomial_table[weight_offset + j] * right_sum;
                if (current_n < n) {
                    move_right(current_n, right_sum, right_term);
                }
            }
            T left_sum = sum;
            T left_term = term;
            for (int current_n = sample_n; current_n > first_n; --current_n) {
                move_left(current_n, left_sum, left_term);
                const int j = n - current_n + 1;
                ans += weighted_binomial_table[weight_offset + j] * left_sum;
            }
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
