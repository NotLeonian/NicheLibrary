// competitive-verifier: STANDALONE

#include <cassert>
#include <cstdint>

#include "../math/number-theory/generalized-floor-sum-degree-le-2.hpp"

long long floor_div_brute(long long x, long long y) {
    assert(y > 0);

    long long q = x / y;
    long long r = x % y;
    if (r < 0) {
        --q;
    }
    return q;
}

int main() {
    for (long long n = 0; n <= 20; ++n) {
        for (long long m = 1; m <= 20; ++m) {
            for (long long a = -20; a <= 20; ++a) {
                for (long long b = -20; b <= 20; ++b) {
                    const auto res =
                        generalized_floor_sum_degree_le_2<long long>(n, m, a,
                                                                     b);

                    long long s_01 = 0, s_11 = 0, s_02 = 0;
                    for (long long i = 0; i < n; ++i) {
                        const long long value = floor_div_brute(a * i + b, m);
                        s_01 += value;
                        s_11 += i * value;
                        s_02 += value * value;
                    }

                    assert(res.ans_01 == s_01);
                    assert(res.ans_11 == s_11);
                    assert(res.ans_02 == s_02);
                }
            }
        }
    }

    {
        using T = std::uint64_t;

        const T n = 900931385;
        const T m = 333006410;
        const T x = 263208878;
        const T y = 243209245;

        const auto r0 = generalized_floor_sum_degree_le_2<T>(n, m, x, T(0));
        const auto r1 = generalized_floor_sum_degree_le_2<T>(n, m, x, y);

        T ans = 0;
        ans -= r0.ans_01 * n - r0.ans_11;
        ans -= r1.ans_01 * (n - 1) - 2 * r1.ans_11;

        assert(ans == 202919340569980512ULL);
    }

    return 0;
}
