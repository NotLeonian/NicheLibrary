#ifndef MATH_NUMBER_THEORY_GENERALIZED_FLOOR_SUM_PQ_LE_2_HPP
#define MATH_NUMBER_THEORY_GENERALIZED_FLOOR_SUM_PQ_LE_2_HPP

// 一般化 floor sum (p+q<=2) のうち (0,1),(1,1),(0,2) をまとめて求める。
// ans_01 = Σ floor((a i + b)/m), ans_11 = Σ i*floor((a i + b)/m), ans_02 = Σ floor((a i + b)/m)^2。
// n>=0, m>0 を仮定する。a,b は負でもよい。
// 計算量 O(log m)。

#include <cassert>
#include <type_traits>

template <class T> struct GeneralizedFloorSumPQLe2Result {
    T ans_01;
    T ans_11;
    T ans_02;
};

namespace generalized_floor_sum_pq_le_2_internal {

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

// Σ_{i=0}^{n-1} i^2 = (n-1)n(2n-1)/6
template <class T> T sum_0_to_n_minus_1_sq(T n) {
    if (n == 0)
        return 0;
    T a = n - 1, b = n, c = 2 * n - 1;
    if ((a % 2) == 0)
        a /= 2;
    else if ((b % 2) == 0)
        b /= 2;
    else
        c /= 2;

    if ((a % 3) == 0)
        a /= 3;
    else if ((b % 3) == 0)
        b /= 3;
    else
        c /= 3;

    return a * b * c;
}

template <class T> T sum_range(T l, T r) {
    // Σ_{i=l}^{r} i
    if (l > r)
        return 0;
    T cnt = r - l + 1;
    T s = l + r;
    if ((s & 1) == 0)
        s /= 2;
    else
        cnt /= 2;
    return s * cnt;
}

template <class Int> struct Result {
    Int ans_01;
    Int ans_11;
    Int ans_02;
};

template <class Int> Result<Int> solve(Int n, Int m, Int a, Int b) {
    if constexpr (is_signed<Int>::value)
        assert(n >= 0);
    assert(m > 0);
    if (n == 0)
        return {0, 0, 0};

    const Int qa = floor_div(a, m);
    a = floor_mod(a, m);
    const Int qb = floor_div(b, m);
    b = floor_mod(b, m);

    if constexpr (is_signed<Int>::value) {
        assert(a >= 0);
        assert(b >= 0);
    }
    assert(a < m);
    assert(b < m);

    Result<Int> base = {0, 0, 0};
    if (a != 0) {
        const Int y_max = (a * n + b) / m;
        if (y_max != 0) {
            const Int x_max = y_max * m - b;
            const Int t = (x_max + a - 1) / a; // ceil(x_max / a)
            const Int b2 = (a - (x_max % a)) % a;

            const auto rec = solve(y_max, a, m, b2);

            const Int head_01 = rec.ans_01;
            const Int head_11 = ((2 * t - 1) * rec.ans_01 - rec.ans_02) / 2;
            const Int head_02 = (2 * y_max - 1) * rec.ans_01 - 2 * rec.ans_11;

            const Int tail_len = n - t;
            const Int tail_01 = tail_len * y_max;
            const Int tail_11 = y_max * sum_range(t, n - 1);
            const Int tail_02 = tail_len * y_max * y_max;

            base = {head_01 + tail_01, head_11 + tail_11, head_02 + tail_02};
        }
    }

    const Int si = sum_0_to_n_minus_1(n);
    const Int si2 = sum_0_to_n_minus_1_sq(n);

    Result<Int> ans;
    ans.ans_01 = qa * si + qb * n + base.ans_01;
    ans.ans_11 = qa * si2 + qb * si + base.ans_11;
    ans.ans_02 = qa * qa * si2 + 2 * qa * qb * si + 2 * qa * base.ans_11 +
                 qb * qb * n + 2 * qb * base.ans_01 + base.ans_02;
    return ans;
}

} // namespace generalized_floor_sum_pq_le_2_internal

template <class T>
GeneralizedFloorSumPQLe2Result<T> generalized_floor_sum_pq_le_2(T n, T m, T a,
                                                                T b) {
    static_assert(generalized_floor_sum_pq_le_2_internal::is_integral<T>::value,
                  "T must be integer.");
    if constexpr (generalized_floor_sum_pq_le_2_internal::is_signed<T>::value)
        assert(n >= 0);
    assert(m > 0);

#ifdef __SIZEOF_INT128__
    using I = __int128_t;
    const auto res = generalized_floor_sum_pq_le_2_internal::solve<I>(
        static_cast<I>(n), static_cast<I>(m), static_cast<I>(a),
        static_cast<I>(b));
    return {static_cast<T>(res.ans_01), static_cast<T>(res.ans_11),
            static_cast<T>(res.ans_02)};
#else
    const auto res =
        generalized_floor_sum_pq_le_2_internal::solve<T>(n, m, a, b);
    return {res.ans_01, res.ans_11, res.ans_02};
#endif
}

#endif
