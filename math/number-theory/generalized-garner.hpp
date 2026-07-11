#ifndef MATH_NUMBER_THEORY_GENERALIZED_GARNER_HPP
#define MATH_NUMBER_THEORY_GENERALIZED_GARNER_HPP

// A_i x ≡ B_i (mod M_i) の形の連立合同方程式を解く。
// 各式を x ≡ r_i (mod m_i) に直してから一般の中国剰余定理で合成する。
// M_i > 0、a, b, m のサイズ一致、返り値が R1, R2 に収まることを仮定する。
// 係数や法は互いに素でなくてよい。解がなければ (0, 0) を返す。
// 標準の 64 bit 以下の整数型では剰余乗算に 128 bit 整数型を用いる。
// それ以外の整数型では加算と 2 倍による剰余乗算を用いる。
// 式の個数を N、全ての法と 2 の最大値を V とすると、計算量 O(N log V)。
// ただし、通常の乗算を使えない型では O(N log^2 V)。

#include <cassert>
#include <cstddef>
#include <limits>
#include <type_traits>
#include <utility>
#include <vector>

#include "../../internal/int128.hpp"

namespace generalized_garner_internal {
template <class T>
constexpr bool is_integer_v =
    std::numeric_limits<std::remove_cv_t<T>>::is_integer;

template <class T>
constexpr bool is_signed_v =
    std::numeric_limits<std::remove_cv_t<T>>::is_signed;

template <class T>
constexpr bool is_standard_integer_v =
    std::is_integral_v<std::remove_cv_t<T>> &&
    !std::is_same_v<std::remove_cv_t<T>, bool>;

template <class T>
constexpr bool use_uint128_mul_v =
    is_standard_integer_v<T> &&
    std::numeric_limits<std::remove_cv_t<T>>::digits <= 64;

template <class T> T safe_mod(T x, T m) {
    if constexpr (is_signed_v<T>) {
        if (x >= 0 && x < m) {
            return x;
        }
    } else if (x < m) {
        return x;
    }
    x %= m;
    if constexpr (is_signed_v<T>) {
        if (x < 0) {
            x += m;
        }
    }
    return x;
}

template <class T> T gcd(T a, T b) {
    while (b != 0) {
        const T c = a % b;
        a = b;
        b = c;
    }
    return a;
}

template <class T> T add_mod(T a, T b, T m) {
    const T gap = m - b;
    if (a >= gap) {
        return a - gap;
    }
    return a + b;
}

template <class T> T mul_mod_normalized(T a, T b, T m) {
    if (a == 0 || b == 0) {
        return 0;
    }
    if (a == 1) {
        return b;
    }
    if (b == 1) {
        return a;
    }
    if constexpr (use_uint128_mul_v<T>) {
        using U = NicheLibrary::UInt128;
        return static_cast<T>((U(a) * U(b)) % U(m));
    } else {
        if (a < b) {
            std::swap(a, b);
        }
        T res = 0;
        while (b != 0) {
            const T half_b = b / 2;
            if (b != half_b * 2) {
                res = add_mod(res, a, m);
            }
            b = half_b;
            if (b != 0) {
                a = add_mod(a, a, m);
            }
        }
        return res;
    }
}

template <class T> T inv_mod(T a, T m) {
    if (a == 1) {
        return 1;
    }
    T b = a;
    a = m;
    T x = 0;
    T y = 1;
    while (b != 1) {
        const T q = a / b;
        const T c = a - q * b;
        a = b;
        b = c;
        // 初期値と呼び出し元が互いに素を保証することから 0 < q < m。
        const T qy = mul_mod_normalized(q, y, m);
        const T z = x >= qy ? x - qy : x + (m - qy);
        x = y;
        y = z;
    }
    return y;
}

template <class T> std::pair<T, T> merge_congruence(T r0, T m0, T r1, T m1) {
    if (m0 == 1) {
        return {r1, m1};
    }
    if (m1 == 1) {
        return {r0, m0};
    }
    const T g = gcd(m0, m1);
    const T diff = r1 - r0;
    if (diff == 0) {
        return {r0, m0 * (m1 / g)};
    }
    const T diff_div_g = diff / g;
    if (diff_div_g * g != diff) {
        return {0, 0};
    }
    const T u1 = m1 / g;
    if (u1 == 1) {
        return {r0, m0};
    }
    const T factor = safe_mod(diff_div_g, u1);
    if (factor != 0) {
        const T t =
            mul_mod_normalized(factor, inv_mod(safe_mod(m0 / g, u1), u1), u1);
        r0 += t * m0;
    }
    m0 *= u1;
    return {r0, m0};
}

template <class T> std::pair<T, T> solve_linear_congruence(T a, T b, T m) {
    if (m == 1) {
        return {0, 1};
    }
    a = safe_mod(a, m);
    b = safe_mod(b, m);
    if (a == 0) {
        return b == 0 ? std::pair<T, T>{0, 1} : std::pair<T, T>{0, 0};
    }
    if (a == 1) {
        return {b, m};
    }
    // 0 < a < m なので、引数をこの順序にすると最初の剰余の計算を省ける。
    const T g = gcd(m, a);
    const T b_div_g = b / g;
    if (b_div_g * g != b) {
        return {0, 0};
    }
    const T mod = m / g;
    if (mod == 1) {
        return {0, 1};
    }
    if (b_div_g == 0) {
        return {0, mod};
    }
    const T rem = mul_mod_normalized(b_div_g, inv_mod(a / g, mod), mod);
    return {rem, mod};
}
} // namespace generalized_garner_internal

template <class R1 = long long, class R2 = R1, class T1, class T2, class M>
std::pair<R1, R2> generalized_garner(const std::vector<T1> &a,
                                     const std::vector<T2> &b,
                                     const std::vector<M> &m) {
    namespace gg_internal = generalized_garner_internal;
    static_assert(gg_internal::is_integer_v<R1>, "R1 must be integer.");
    static_assert(gg_internal::is_integer_v<R2>, "R2 must be integer.");
    static_assert(gg_internal::is_signed_v<R1>, "R1 must be signed.");
    static_assert(gg_internal::is_signed_v<R2>, "R2 must be signed.");
    static_assert(gg_internal::is_integer_v<T1>, "T1 must be integer.");
    static_assert(gg_internal::is_integer_v<T2>, "T2 must be integer.");
    static_assert(gg_internal::is_integer_v<M>, "M must be integer.");

    using R = std::common_type_t<R1, R2>;
    static_assert(gg_internal::is_integer_v<R>, "R must be integer.");
    static_assert(gg_internal::is_signed_v<R>, "R must be signed.");

    assert(a.size() == b.size());
    assert(a.size() == m.size());
    R r0 = 0;
    R m0 = 1;
    const std::size_t n = a.size();
    for (std::size_t i = 0; i < n; ++i) {
        const R mi = static_cast<R>(m[i]);
        assert(mi > 0);
        const auto [r1, m1] = gg_internal::solve_linear_congruence(
            static_cast<R>(a[i]), static_cast<R>(b[i]), mi);
        if (m1 == 0) {
            return {0, 0};
        }
        const auto [nr, nm] = gg_internal::merge_congruence(r0, m0, r1, m1);
        if (nm == 0) {
            return {0, 0};
        }
        r0 = nr;
        m0 = nm;
    }
    return {static_cast<R1>(r0), static_cast<R2>(m0)};
}

#endif
