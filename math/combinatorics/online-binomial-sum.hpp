#ifndef MATH_COMBINATORICS_ONLINE_BINOMIAL_SUM_HPP
#define MATH_COMBINATORICS_ONLINE_BINOMIAL_SUM_HPP

// Σ_{i=l}^{u-1} r^i binom(m,i) をオンラインで求める。
// 0 <= l <= u と 0 <= m <= max_m を仮定する。
// n と m のバケット境界および上端の直積で累積和をサンプルし、
// クエリの点に 2 次元的に最も近いサンプル点から復元する。
// T は素数を法とする体の型である。
// std::numeric_limits<T>::is_integer が false であり、
// 法 p について max_m < p であることを仮定する。
// r = 0 では、バケットサイズを指定しない場合もその計算を行わない。
// 前計算とクエリは時間計算量、空間計算量ともに O(1)。
// r = -1 では交代二項和の閉形式を用いる。
// B をバケットサイズとして、r が 0, -1 でない場合の時間計算量は
// 前計算 O(max_m^2 / B + max_m)、クエリ O(B)。
// 空間計算量は O((max_m / B + 1)^2 + max_m)。

#include <cassert>
#include <limits>
#include <vector>

template <class T> struct OnlineBinomialSum {
    static_assert(!std::numeric_limits<T>::is_integer,
                  "std::numeric_limits<T>::is_integer must be false.");

  public:
    int max_m;
    int bucket_size;
    T r;
    T r_plus_one;
    bool r_is_zero;
    bool r_is_minus_one;
    T r_plus_one_inverse;
    std::vector<T> factorial;
    std::vector<T> inverse_factorial;
    std::vector<T> integer_inverse;
    std::vector<T> power_r;
    std::vector<int> sample_n_list;
    std::vector<int> sample_m_list;
    std::vector<T> sample_sum_table;

    explicit OnlineBinomialSum(int max_m, T r, int bucket_size)
        : max_m(max_m), bucket_size(bucket_size), r(r), r_plus_one(r + T(1)),
          r_is_zero(r == T()), r_is_minus_one(r_plus_one == T()),
          r_plus_one_inverse(T()) {
        assert(max_m >= 0);
        assert(bucket_size > 0);

        if (r_is_zero) {
            return;
        }

        factorial.assign(max_m + 1, T(1));
        for (int i = 1; i <= max_m; ++i) {
            factorial[i] = factorial[i - 1] * T(i);
        }

        inverse_factorial.assign(max_m + 1, T(1));
        inverse_factorial[max_m] = T(1) / factorial[max_m];
        for (int i = max_m; i >= 1; --i) {
            inverse_factorial[i - 1] = inverse_factorial[i] * T(i);
        }

        if (r_is_minus_one) {
            return;
        }

        r_plus_one_inverse = T(1) / r_plus_one;

        integer_inverse.assign(max_m + 1, T());
        for (int i = 1; i <= max_m; ++i) {
            integer_inverse[i] = factorial[i - 1] * inverse_factorial[i];
        }

        power_r.assign(max_m + 2, T());
        power_r[0] = T(1);
        for (int i = 0; i <= max_m; ++i) {
            power_r[i + 1] = power_r[i] * r;
        }

        sample_n_list = make_sample_list(max_m + 1);
        sample_m_list = make_sample_list(max_m);
        sample_sum_table.assign(sample_n_list.size() * sample_m_list.size(),
                                T());

        const int sample_n_count = static_cast<int>(sample_n_list.size());
        const int sample_m_count = static_cast<int>(sample_m_list.size());
        for (int sample_m_index = 0; sample_m_index < sample_m_count;
             ++sample_m_index) {
            const int sample_m = sample_m_list[sample_m_index];
            T sum = T();
            T term = T(1);
            int current_n = 0;

            for (int sample_n_index = 0; sample_n_index < sample_n_count;
                 ++sample_n_index) {
                const int sample_n = sample_n_list[sample_n_index];
                while (current_n < sample_n && current_n <= sample_m) {
                    sum += term;
                    if (current_n < sample_m) {
                        term *= r;
                        term *= T(sample_m - current_n);
                        term *= integer_inverse[current_n + 1];
                    }
                    ++current_n;
                }
                sample_sum_table[sample_m_index * sample_n_count +
                                 sample_n_index] = sum;
            }
        }
    }

    explicit OnlineBinomialSum(int max_m, T r = T(1))
        : OnlineBinomialSum(max_m, r,
                            r == T() ? 1 : default_bucket_size(max_m)) {}

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
        if (r_is_minus_one) {
            if (m == 0) {
                return T(1);
            }

            T ans = binomial(m - 1, n - 1);
            if ((n - 1) % 2 == 1) {
                ans = T() - ans;
            }
            return ans;
        }

        const int sample_n_index = nearest_sample_index(sample_n_list, n);
        const int sample_m_index = nearest_sample_index(sample_m_list, m);
        const int sample_n_count = static_cast<int>(sample_n_list.size());
        int current_n = sample_n_list[sample_n_index];
        int current_m = sample_m_list[sample_m_index];
        T sum =
            sample_sum_table[sample_m_index * sample_n_count + sample_n_index];

        while (current_n < n) {
            sum += power_r[current_n] * binomial(current_m, current_n);
            ++current_n;
        }

        while (current_n > n) {
            --current_n;
            sum -= power_r[current_n] * binomial(current_m, current_n);
        }

        while (current_m < m) {
            sum *= r_plus_one;
            sum -= power_r[current_n] * binomial(current_m, current_n - 1);
            ++current_m;
        }

        while (current_m > m) {
            --current_m;
            sum += power_r[current_n] * binomial(current_m, current_n - 1);
            sum *= r_plus_one_inverse;
        }

        return sum;
    }

    T binom_sum(int l, int u, int m) const {
        assert(l >= 0);
        assert(l <= u);

        return binom_prefix_sum(u, m) - binom_prefix_sum(l, m);
    }

  private:
    T binomial(int n, int k) const {
        if (k < 0 || k > n) {
            return T();
        }

        return factorial[n] * inverse_factorial[k] * inverse_factorial[n - k];
    }

    std::vector<int> make_sample_list(int limit) const {
        const int full_bucket_count = limit / bucket_size;
        std::vector<int> sample_list;
        sample_list.reserve(full_bucket_count + 2);
        for (int index = 0; index <= full_bucket_count; ++index) {
            sample_list.push_back(index * bucket_size);
        }
        if (sample_list.back() != limit) {
            sample_list.push_back(limit);
        }

        return sample_list;
    }

    int nearest_sample_index(const std::vector<int> &sample_list,
                             int value) const {
        int index = value / bucket_size;
        const int sample_count = static_cast<int>(sample_list.size());
        if (index >= sample_count) {
            index = sample_count - 1;
        }
        if (index + 1 >= sample_count) {
            return index;
        }
        if (value - sample_list[index] <= sample_list[index + 1] - value) {
            return index;
        }

        return index + 1;
    }

    static int default_bucket_size(int max_m) {
        assert(max_m >= 0);

        int bucket_size = 1;
        while (4LL * bucket_size * bucket_size <= max_m) {
            bucket_size *= 2;
        }

        return bucket_size;
    }
};

#endif
