#ifndef MATH_NUMBER_THEORY_GENERALIZED_FLOOR_SUM_DEGREE_LE_TWO_HPP
#define MATH_NUMBER_THEORY_GENERALIZED_FLOOR_SUM_DEGREE_LE_TWO_HPP

// 一般化 floor sum（次数 2 以下）のうち、（0, 1）、（1, 1）、（0, 2）をまとめて求める。
// ans_01 = Σ floor((a i + b) / m)、ans_11 = Σ i floor((a i + b) / m)、
// ans_02 = Σ floor((a i + b) / m)^2。
// n >= 0、m > 0 を仮定する。T が符号付きの場合、a, b は負でもよい。
// T および明示的に指定する Internal は整数型であり、内部計算が内部型の範囲に収まることを仮定する。
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
    if constexpr (is_signed_v<T>) {
        if (x >= 0 && x < y) {
            return {0, x};
        }
        T q = x / y;
        T r = x - q * y;
        if (r < 0) {
            q -= T(1);
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
template <class State, class Value> Value sum_0_to_n_minus_1(State n) {
    const State half = n / 2;
    if (n == half * 2) {
        return static_cast<Value>(half) * static_cast<Value>(n - 1);
    }
    return static_cast<Value>(n) * static_cast<Value>(half);
}

// Σ_{i=0}^{n-1} i^2 = (n-1)n(2n-1)/6
template <class State, class Value> Value sum_0_to_n_minus_1_sq(State n) {
    Value a = static_cast<Value>(n - 1);
    Value b = static_cast<Value>(n);
    Value c = static_cast<Value>(2) * static_cast<Value>(n) - Value(1);
    const State n_mod_6 = n % 6;
    if (n_mod_6 == 0) {
        b /= Value(6);
    } else if (n_mod_6 == 1) {
        a /= Value(6);
    } else if (n_mod_6 == 2) {
        b /= Value(2);
        c /= Value(3);
    } else if (n_mod_6 == 3) {
        a /= Value(2);
        b /= Value(3);
    } else if (n_mod_6 == 4) {
        a /= Value(3);
        b /= Value(2);
    } else {
        a /= Value(2);
        c /= Value(3);
    }
    return a * b * c;
}

// Σ_{i=l}^{n-1} i
template <class State, class Value> Value sum_l_to_n_minus_1(State l, State n) {
    const State count = n - l;
    const Value sum = static_cast<Value>(l) + static_cast<Value>(n) - Value(1);
    const State half_count = count / 2;
    if (count == half_count * 2) {
        return static_cast<Value>(half_count) * sum;
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
    const auto [qa_state, a_mod] = floor_div_mod(a, m);
    const auto [qb_state, b_mod] = floor_div_mod(b, m);
    a = a_mod;
    b = b_mod;

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

            const Value a_value = static_cast<Value>(a);
            const Value x_max_div_a = x_max / a_value;
            const Value x_max_mod_a = x_max - x_max_div_a * a_value;
            const bool has_remainder = x_max_mod_a != Value(0);
            const State t = static_cast<State>(x_max_div_a) +
                            (has_remainder ? State(1) : State(0));
            const State b2 = has_remainder
                                 ? static_cast<State>(a_value - x_max_mod_a)
                                 : State(0);

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
            const Value tail_11 = y * sum_l_to_n_minus_1<State, Value>(t, n);
            const Value tail_02 = tail_len * y * y;

            base = {head_01 + tail_01, head_11 + tail_11, head_02 + tail_02};
        }
    }

    const Value n_value = static_cast<Value>(n);
    if (qa == Value(0)) {
        if (qb == Value(0)) {
            return base;
        }
        const Value si = sum_0_to_n_minus_1<State, Value>(n);
        return {qb * n_value + base.ans_01, qb * si + base.ans_11,
                qb * qb * n_value + Value(2) * qb * base.ans_01 + base.ans_02};
    }

    const Value si = sum_0_to_n_minus_1<State, Value>(n);
    const Value si2 = sum_0_to_n_minus_1_sq<State, Value>(n);
    if (qb == Value(0)) {
        return {qa * si + base.ans_01, qa * si2 + base.ans_11,
                qa * qa * si2 + Value(2) * qa * base.ans_11 + base.ans_02};
    }

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
    if (n == 0) {
        return {T(0), T(0), T(0)};
    }
    if (n == 1) {
        const T q = gfs_internal::floor_div_mod(b, m).first;
        const Value q_value = static_cast<Value>(q);
        return {q, T(0), static_cast<T>(q_value * q_value)};
    }

    const auto res = gfs_internal::solve<T, Value>(n, m, a, b);
    return {static_cast<T>(res.ans_01), static_cast<T>(res.ans_11),
            static_cast<T>(res.ans_02)};
}

#endif
