// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/sum_of_floor_of_linear

#include <cassert>
#include <cstdint>
#include <iostream>

#include "../math/number-theory/floor-sum.hpp"

#ifdef __SIZEOF_INT128__
static __int128 floor_div_i128(__int128 x, __int128 m) {
    assert(m > 0);
    __int128 q = x / m;
    __int128 r = x % m;
    if (r < 0)
        --q;
    return q;
}

static __int128 brute_floor_sum_i128(long long n, long long m, long long a,
                                     long long b) {
    assert(n >= 0);
    assert(m > 0);
    __int128 res = 0;
    for (long long i = 0; i < n; ++i) {
        res += floor_div_i128(static_cast<__int128>(a) * i + b, m);
    }
    return res;
}

static std::uint64_t xorshift64(std::uint64_t &x) {
    x ^= x << 7;
    x ^= x >> 9;
    return x;
}

static void self_check_negative() {
    std::uint64_t rng = 88172645463325252ull;

    for (int iter = 0; iter < 5000; ++iter) {
        const long long n = static_cast<long long>(xorshift64(rng) % 25);
        const long long m = static_cast<long long>(xorshift64(rng) % 25) + 1;
        const long long a = static_cast<long long>(xorshift64(rng) % 81) - 40;
        const long long b = static_cast<long long>(xorshift64(rng) % 81) - 40;

        const __int128 want = brute_floor_sum_i128(n, m, a, b);
        const __int128 got1 = floor_sum<__int128>(
            static_cast<__int128>(n), static_cast<__int128>(m),
            static_cast<__int128>(a), static_cast<__int128>(b));
        assert(want == got1);

        const long long got2 = floor_sum<long long>(n, m, a, b);
        assert(want == static_cast<__int128>(got2));
    }

    {
        const long long n = 0, m = 7, a = -3, b = -5;
        assert(floor_sum<long long>(n, m, a, b) == 0);
    }
    {
        const long long n = 10, m = 1, a = -7, b = 3;
        const __int128 want = brute_floor_sum_i128(n, m, a, b);
        const long long got = floor_sum<long long>(n, m, a, b);
        assert(want == static_cast<__int128>(got));
    }
}
#endif

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

#ifdef __SIZEOF_INT128__
    self_check_negative();
#endif

    int T;
    std::cin >> T;
    while (T--) {
        long long n, m, a, b;
        std::cin >> n >> m >> a >> b;
        std::cout << floor_sum<long long>(n, m, a, b) << '\n';
    }
    return 0;
}
