#ifndef MATH_NUMBER_THEORY_FLOOR_SUM_HPP
#define MATH_NUMBER_THEORY_FLOOR_SUM_HPP

// 一次式の床関数の和 Σ_{i=0}^{n-1} floor((a i + b) / m) を求める。
// n は非負、m は正を仮定する。
// T は std::numeric_limits<T>::is_integer が true である型。
// T が符号付きの場合、a, b は負でもよい（数学的な床除算で扱う）。
// 中間計算と戻り値が T の範囲に収まることを仮定する。
// 計算量 O(log m)。

#include <cassert>
#include <limits>
#include <utility>

namespace floor_sum_internal {
template <class T> std::pair<T, T> floor_div_mod(T x, T y) {
    if constexpr (std::numeric_limits<T>::is_signed) {
        if (x >= 0 && x < y) {
            return {0, x};
        }
        T q = x / y;
        T r = x - q * y;
        if (r < 0) {
            q -= 1;
            r += y;
        }
        return {q, r};
    } else {
        if (x < y) {
            return {0, x};
        }
        const T q = x / y;
        return {q, x - q * y};
    }
}

// Σ_{i=0}^{n-1} i = n(n-1)/2
template <class T> T sum_0_to_n_minus_1(T n) {
    const T half = n / 2;
    if (n == half * 2) {
        return half * (n - 1);
    }
    return n * half;
}
} // namespace floor_sum_internal

template <class T> T floor_sum(T n, T m, T a, T b) {
    static_assert(std::numeric_limits<T>::is_integer, "T must be integer.");
    if constexpr (std::numeric_limits<T>::is_signed) {
        assert(n >= 0);
    }
    assert(m > 0);
    if (n == 0) {
        return 0;
    }
    if (n == 1) {
        return floor_sum_internal::floor_div_mod(b, m).first;
    }

    T ans = 0;

    {
        const auto [q, r] = floor_sum_internal::floor_div_mod(a, m);
        a = r;
        if (q != 0) {
            ans += q * floor_sum_internal::sum_0_to_n_minus_1(n);
        }
    }
    {
        const auto [q, r] = floor_sum_internal::floor_div_mod(b, m);
        b = r;
        if (q != 0) {
            ans += q * n;
        }
    }
    if (a == 0) {
        return ans;
    }

    while (true) {
        const T y_max = a * n + b;
        if (y_max < m) {
            break;
        }

        const T new_n = y_max / m;
        b = y_max - new_n * m;
        n = new_n;
        std::swap(m, a);

        const T qa = a / m;
        ans += qa * floor_sum_internal::sum_0_to_n_minus_1(n);
        a -= qa * m;
        if (b >= m) {
            const T qb = b / m;
            ans += qb * n;
            b -= qb * m;
        }
    }
    return ans;
}

#endif
