// competitive-verifier: STANDALONE

#include <cassert>
#include <cstdint>

#include "../internal/int128.hpp"
#include "../math/number-theory/floor-sum.hpp"

long long floor_div_brute(long long x, long long m) {
    assert(m > 0);

    long long q = x / m;
    const long long r = x % m;
    if (r < 0) {
        --q;
    }
    return q;
}

long long floor_sum_brute(long long n, long long m, long long a, long long b) {
    long long result = 0;
    for (long long i = 0; i < n; ++i) {
        result += floor_div_brute(a * i + b, m);
    }
    return result;
}

int main() {
    for (long long n = 0; n <= 20; ++n) {
        for (long long m = 1; m <= 20; ++m) {
            for (long long a = -20; a <= 20; ++a) {
                for (long long b = -20; b <= 20; ++b) {
                    const long long expected = floor_sum_brute(n, m, a, b);
                    assert(floor_sum(n, m, a, b) == expected);

                    using NicheLibrary::Int128;
                    const Int128 actual =
                        floor_sum(Int128(n), Int128(m), Int128(a), Int128(b));
                    assert(actual == Int128(expected));
                }
            }
        }
    }
    for (std::uint64_t n = 0; n <= 20; ++n) {
        for (std::uint64_t m = 1; m <= 20; ++m) {
            for (std::uint64_t a = 0; a <= 20; ++a) {
                for (std::uint64_t b = 0; b <= 20; ++b) {
                    const long long expected = floor_sum_brute(n, m, a, b);
                    assert(floor_sum(n, m, a, b) ==
                           static_cast<std::uint64_t>(expected));
                }
            }
        }
    }

    return 0;
}
