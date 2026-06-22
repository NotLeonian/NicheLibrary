#ifndef MATH_NUMBER_THEORY_GENERALIZED_GARNER_HPP
#define MATH_NUMBER_THEORY_GENERALIZED_GARNER_HPP

// A_i x ≡ B_i (mod M_i) の形の連立合同方程式を解く。
// 各式を x ≡ r_i (mod m_i) に直してから一般の中国剰余定理で合成する。
// M_i > 0、a, b, m のサイズ一致、返り値が R1, R2 に収まることを仮定する。
// 係数や法は互いに素でなくてよい。解なしなら (0, 0) を返す。
// 標準 64 ビット整数型では計算量 O(N log max M_i)。
// __int128 では剰余乗算をダブリングで行う。

#include <cassert>
#include <type_traits>
#include <utility>
#include <vector>

namespace generalized_garner_internal {
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

template <class T> T safe_mod(T x, T m) {
    assert(m > 0);
    x %= m;
    if constexpr (is_signed<T>::value) {
        if (x < 0) {
            x += m;
        }
    }
    return x;
}

template <class T> T gcd(T a, T b) {
    assert(a >= 0);
    assert(b >= 0);
    while (b != 0) {
        const T c = a % b;
        a = b;
        b = c;
    }
    return a;
}

template <class T> T add_mod(T a, T b, T m) {
    assert(0 <= a && a < m);
    assert(0 <= b && b < m);
    if (a >= m - b) {
        return a - (m - b);
    }
    return a + b;
}

template <class T> T mul_mod(T a, T b, T m) {
    assert(m > 0);
    a = safe_mod(a, m);
    b = safe_mod(b, m);
#ifdef __SIZEOF_INT128__
    if constexpr (sizeof(T) <= 8) {
        return static_cast<T>(
            (static_cast<__uint128_t>(a) * static_cast<__uint128_t>(b)) %
            static_cast<__uint128_t>(m));
    } else {
#endif
        T res = 0;
        while (b != 0) {
            if ((b & 1) != 0) {
                res = add_mod(res, a, m);
            }
            b >>= 1;
            if (b != 0) {
                a = add_mod(a, a, m);
            }
        }
        return res;
#ifdef __SIZEOF_INT128__
    }
#endif
}

template <class T> T inv_mod(T a, T m) {
    assert(m > 1);
    a = safe_mod(a, m);
    assert(gcd(a, m) == 1);
    T b = m;
    T x = 1;
    T y = 0;
    while (b != 0) {
        const T q = a / b;
        const T c = a - q * b;
        a = b;
        b = c;
        const T z = safe_mod(x - mul_mod(q, y, m), m);
        x = y;
        y = z;
    }
    assert(a == 1);
    return x;
}

template <class T> std::pair<T, T> merge_congruence(T r0, T m0, T r1, T m1) {
    assert(0 <= r0 && r0 < m0);
    assert(0 <= r1 && r1 < m1);
    const T g = gcd(m0, m1);
    const T diff = r1 - r0;
    if (diff % g != 0) {
        return {0, 0};
    }
    const T u1 = m1 / g;
    if (u1 == 1) {
        return {r0, m0};
    }
    const T t = mul_mod(diff / g, inv_mod(m0 / g, u1), u1);
    r0 += t * m0;
    m0 *= u1;
    return {r0, m0};
}

template <class T> std::pair<T, T> solve_linear_congruence(T a, T b, T m) {
    assert(m > 0);
    a = safe_mod(a, m);
    b = safe_mod(b, m);
    const T g = gcd(a, m);
    if (b % g != 0) {
        return {0, 0};
    }
    const T mod = m / g;
    if (mod == 1) {
        return {0, 1};
    }
    const T rem = mul_mod(b / g, inv_mod(a / g, mod), mod);
    return {rem, mod};
}
} // namespace generalized_garner_internal

template <class R1 = long long, class R2 = R1, class T1, class T2, class M>
std::pair<R1, R2> generalized_garner(const std::vector<T1> &a,
                                     const std::vector<T2> &b,
                                     const std::vector<M> &m) {
    static_assert(generalized_garner_internal::is_integral<R1>::value);
    static_assert(generalized_garner_internal::is_integral<R2>::value);
    static_assert(generalized_garner_internal::is_signed<R1>::value);
    static_assert(generalized_garner_internal::is_signed<R2>::value);
    static_assert(generalized_garner_internal::is_integral<T1>::value);
    static_assert(generalized_garner_internal::is_integral<T2>::value);
    static_assert(generalized_garner_internal::is_integral<M>::value);
    using R = std::common_type_t<R1, R2>;
    static_assert(generalized_garner_internal::is_integral<R>::value);
    static_assert(generalized_garner_internal::is_signed<R>::value);
    assert(a.size() == b.size());
    assert(a.size() == m.size());
    R r0 = 0;
    R m0 = 1;
    const int n = static_cast<int>(a.size());
    for (int i = 0; i < n; ++i) {
        const R mi = static_cast<R>(m[i]);
        assert(mi > 0);
        const auto [r1, m1] =
            generalized_garner_internal::solve_linear_congruence(
                static_cast<R>(a[i]), static_cast<R>(b[i]), mi);
        if (m1 == 0) {
            return {0, 0};
        }
        const auto [nr, nm] =
            generalized_garner_internal::merge_congruence(r0, m0, r1, m1);
        if (nm == 0) {
            return {0, 0};
        }
        r0 = nr;
        m0 = nm;
    }
    return {static_cast<R1>(r0), static_cast<R2>(m0)};
}

#endif
