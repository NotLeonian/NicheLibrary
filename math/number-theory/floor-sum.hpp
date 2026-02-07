#ifndef MATH_NUMBER_THEORY_FLOOR_SUM_HPP
#define MATH_NUMBER_THEORY_FLOOR_SUM_HPP

// 一次式の床関数の和 Σ_{i=0}^{n-1} floor((a i + b) / m) を求める。
// n は非負、m は正を仮定する。
// a, b は負でもよい（数学的な床除算で扱う）。
// T は整数型（__int128 を含む）。
// n, m が sqrt(max(T)) 程度なら a*n+b などの中間計算が安全になりやすい。
// 計算量 O(log m)。

#include <cassert>
#include <type_traits>
#include <utility>

namespace floor_sum_internal {

template <class T> struct is_integral : std::is_integral<T> {};

#ifdef __SIZEOF_INT128__
template <> struct is_integral<__int128_t> : std::true_type {};
template <> struct is_integral<__uint128_t> : std::true_type {};
#endif

template <class T> struct is_signed : std::is_signed<T> {};

#ifdef __SIZEOF_INT128__
template <> struct is_signed<__int128_t> : std::true_type {};
template <> struct is_signed<__uint128_t> : std::false_type {};
#endif

template <class T> T floor_div(T x, T y) {
    assert(y > 0);
    if constexpr (is_signed<T>::value) {
        T q = x / y;
        T r = x % y;
        if (r < 0)
            --q;
        return q;
    } else {
        return x / y;
    }
}

template <class T> T floor_mod(T x, T y) {
    assert(y > 0);
    if constexpr (is_signed<T>::value) {
        T r = x % y;
        if (r < 0)
            r += y;
        return r;
    } else {
        return x % y;
    }
}

// Σ_{i=0}^{n-1} i = n(n-1)/2
template <class T> T sum_0_to_n_minus_1(T n) {
    if (n == 0)
        return 0;
    if ((n & 1) == 0)
        return (n / 2) * (n - 1);
    return n * ((n - 1) / 2);
}

} // namespace floor_sum_internal

template <class T> T floor_sum(T n, T m, T a, T b) {
    static_assert(floor_sum_internal::is_integral<T>::value,
                  "T must be integer.");
    if constexpr (floor_sum_internal::is_signed<T>::value)
        assert(n >= 0);
    assert(m > 0);
    if (n == 0)
        return 0;

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
        if (y_max < m)
            break;

        n = y_max / m;
        b = y_max % m;
        std::swap(m, a);
    }
    return ans;
}

#endif
