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
template <class T> T floor_div(T x, T y) {
    assert(y > 0);
    if constexpr (std::numeric_limits<T>::is_signed) {
        T q = x / y;
        T r = x % y;
        if (r < 0) {
            q -= 1;
        }
        return q;
    } else {
        return x / y;
    }
}

template <class T> T floor_mod(T x, T y) {
    assert(y > 0);
    if constexpr (std::numeric_limits<T>::is_signed) {
        T r = x % y;
        if (r < 0) {
            r += y;
        }
        return r;
    } else {
        return x % y;
    }
}

// Σ_{i=0}^{n-1} i = n(n-1)/2
template <class T> T sum_0_to_n_minus_1(T n) {
    if (n == 0) {
        return 0;
    }
    if (n % 2 == 0) {
        return (n / 2) * (n - 1);
    }
    return n * ((n - 1) / 2);
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

    T ans = 0;

    {
        const T q = floor_sum_internal::floor_div(a, m);
        a = floor_sum_internal::floor_mod(a, m);
        ans += q * floor_sum_internal::sum_0_to_n_minus_1(n);
    }
    {
        const T q = floor_sum_internal::floor_div(b, m);
        b = floor_sum_internal::floor_mod(b, m);
        ans += q * n;
    }

    while (true) {
        if (a >= m) {
            const T q = a / m;
            ans += q * floor_sum_internal::sum_0_to_n_minus_1(n);
            a %= m;
        }
        if (b >= m) {
            const T q = b / m;
            ans += q * n;
            b %= m;
        }

        const T y_max = a * n + b;
        if (y_max < m) {
            break;
        }

        n = y_max / m;
        b = y_max % m;
        std::swap(m, a);
    }
    return ans;
}

#endif
