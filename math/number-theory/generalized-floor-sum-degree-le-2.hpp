#ifndef MATH_NUMBER_THEORY_GENERALIZED_FLOOR_SUM_DEGREE_LE_2_HPP
#define MATH_NUMBER_THEORY_GENERALIZED_FLOOR_SUM_DEGREE_LE_2_HPP

// 一般化 floor sum（次数 2 以下）のうち、(0,1),(1,1),(0,2) をまとめて求める。
// ans_01 = Σ floor((a i + b)/m)、ans_11 = Σ i*floor((a i + b)/m)、ans_02 = Σ floor((a i + b)/m)^2。
// n >= 0、m > 0 を仮定する。a, b は負でもよい。
// 計算量 O(log m)。

#include <cassert>
#include <limits>
#include <type_traits>
#include <utility>

#include "../../internal/int128.hpp"

template <class T> struct GeneralizedFloorSumDegreeLe2Result {
    T ans_01;
    T ans_11;
    T ans_02;
};

namespace generalized_floor_sum_degree_le_2_internal {
template <class T>
constexpr bool is_integer_v =
    std::numeric_limits<std::remove_cv_t<T>>::is_integer;

template <class T>
constexpr bool is_signed_v =
    std::numeric_limits<std::remove_cv_t<T>>::is_signed;

template <class T>
constexpr bool use_int128_by_default_v =
    std::is_integral_v<std::remove_cv_t<T>> &&
    !std::is_same_v<std::remove_cv_t<T>, bool> &&
    std::numeric_limits<std::remove_cv_t<T>>::digits <= 64;

template <class T>
using default_internal_t =
    std::conditional_t<use_int128_by_default_v<T>, NicheLibrary::Int128, T>;

template <class T> std::pair<T, T> floor_div_mod(T x, T y) {
    assert(y > 0);
    if constexpr (is_signed_v<T>) {
        T q = x / y;
        T r = x % y;
        if (r < 0) {
            q -= T(1);
            r += y;
        }
        return {q, r};
    } else {
        return {x / y, x % y};
    }
}

// Σ_{i=0}^{n-1} i = n(n-1)/2
template <class State, class Value> Value sum_0_to_n_minus_1(State n) {
    if (n == 0) {
        return 0;
    }
    if (n % 2 == 0) {
        return static_cast<Value>(n / 2) * static_cast<Value>(n - 1);
    }
    return static_cast<Value>(n) * static_cast<Value>((n - 1) / 2);
}

// Σ_{i=0}^{n-1} i^2 = (n-1)n(2n-1)/6
template <class State, class Value> Value sum_0_to_n_minus_1_sq(State n) {
    if (n == 0) {
        return 0;
    }
    Value a = static_cast<Value>(n - 1);
    Value b = static_cast<Value>(n);
    Value c = static_cast<Value>(2) * static_cast<Value>(n) - Value(1);
    if (a % Value(2) == 0) {
        a /= Value(2);
    } else if (b % Value(2) == 0) {
        b /= Value(2);
    } else {
        c /= Value(2);
    }
    if (a % Value(3) == 0) {
        a /= Value(3);
    } else if (b % Value(3) == 0) {
        b /= Value(3);
    } else {
        c /= Value(3);
    }
    return a * b * c;
}

template <class State, class Value> Value sum_range(State l, State r) {
    // Σ_{i=l}^{r} i
    if (l > r) {
        return 0;
    }
    const State count = r - l + 1;
    const Value sum = static_cast<Value>(l) + static_cast<Value>(r);
    if (count % 2 == 0) {
        return static_cast<Value>(count / 2) * sum;
    }
    return static_cast<Value>(count) * (sum / Value(2));
}

template <class Value> struct Result {
    Value ans_01;
    Value ans_11;
    Value ans_02;
};

template <class State, class Value>
Result<Value> solve(State n, State m, State a, State b) {
    if constexpr (is_signed_v<State>) {
        assert(n >= 0);
    }
    assert(m > 0);
    if (n == 0) {
        return {0, 0, 0};
    }

    const auto [qa_state, a_mod] = floor_div_mod(a, m);
    const auto [qb_state, b_mod] = floor_div_mod(b, m);
    a = a_mod;
    b = b_mod;

    if constexpr (is_signed_v<State>) {
        assert(a >= 0);
        assert(b >= 0);
    }
    assert(a < m);
    assert(b < m);

    const Value qa = static_cast<Value>(qa_state);
    const Value qb = static_cast<Value>(qb_state);

    Result<Value> base = {0, 0, 0};

    if (a != 0) {
        const Value y_max_value =
            (static_cast<Value>(a) * static_cast<Value>(n) +
             static_cast<Value>(b)) /
            static_cast<Value>(m);

        const State y_max = static_cast<State>(y_max_value);

        if (y_max != 0) {
            const Value x_max =
                y_max_value * static_cast<Value>(m) - static_cast<Value>(b);

            const State t =
                static_cast<State>((x_max + static_cast<Value>(a) - Value(1)) /
                                   static_cast<Value>(a));

            const State b2 = static_cast<State>(
                (static_cast<Value>(a) - (x_max % static_cast<Value>(a))) %
                static_cast<Value>(a));

            const auto rec = solve<State, Value>(y_max, a, m, b2);

            const Value head_01 = rec.ans_01;
            const Value head_11 =
                ((static_cast<Value>(2) * static_cast<Value>(t) - Value(1)) *
                     rec.ans_01 -
                 rec.ans_02) /
                Value(2);
            const Value head_02 =
                (static_cast<Value>(2) * static_cast<Value>(y_max) - Value(1)) *
                    rec.ans_01 -
                static_cast<Value>(2) * rec.ans_11;

            const State tail_len_state = n - t;
            const Value tail_len = static_cast<Value>(tail_len_state);
            const Value y = static_cast<Value>(y_max);

            const Value tail_01 = tail_len * y;
            const Value tail_11 = y * sum_range<State, Value>(t, n - 1);
            const Value tail_02 = tail_len * y * y;

            base = {head_01 + tail_01, head_11 + tail_11, head_02 + tail_02};
        }
    }

    const Value n_value = static_cast<Value>(n);
    const Value si = sum_0_to_n_minus_1<State, Value>(n);
    const Value si2 = sum_0_to_n_minus_1_sq<State, Value>(n);

    Result<Value> ans;
    ans.ans_01 = qa * si + qb * n_value + base.ans_01;
    ans.ans_11 = qa * si2 + qb * si + base.ans_11;
    ans.ans_02 = qa * qa * si2 + Value(2) * qa * qb * si +
                 Value(2) * qa * base.ans_11 + qb * qb * n_value +
                 Value(2) * qb * base.ans_01 + base.ans_02;
    return ans;
}
} // namespace generalized_floor_sum_degree_le_2_internal

template <class T, class Internal = void>
GeneralizedFloorSumDegreeLe2Result<T>
generalized_floor_sum_degree_le_2(T n, T m, T a, T b) {
    namespace gfs_internal = generalized_floor_sum_degree_le_2_internal;
    using Value =
        std::conditional_t<std::is_void_v<Internal>,
                           gfs_internal::default_internal_t<T>, Internal>;

    static_assert(gfs_internal::is_integer_v<T>, "T must be integer.");
    static_assert(gfs_internal::is_integer_v<Value>,
                  "Internal must be integer.");
    static_assert(!std::is_same_v<std::remove_cv_t<T>, bool>,
                  "T must not be bool.");
    static_assert(!std::is_same_v<std::remove_cv_t<Value>, bool>,
                  "Internal must not be bool.");
    static_assert(!gfs_internal::is_signed_v<T> ||
                      gfs_internal::is_signed_v<Value>,
                  "Internal must be signed when T is signed.");
    static_assert(std::numeric_limits<std::remove_cv_t<T>>::digits <=
                      std::numeric_limits<std::remove_cv_t<Value>>::digits,
                  "Internal must be able to represent values of T.");

    if constexpr (gfs_internal::is_signed_v<T>) {
        assert(n >= 0);
    }
    assert(m > 0);

    const auto res = gfs_internal::solve<T, Value>(n, m, a, b);
    return {static_cast<T>(res.ans_01), static_cast<T>(res.ans_11),
            static_cast<T>(res.ans_02)};
}

#endif
