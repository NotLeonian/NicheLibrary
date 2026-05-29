// competitive-verifier: STANDALONE

#include <cassert>

#include "../math/number-theory/generalized-floor-sum-pq-le-2.hpp"

namespace {
long long floor_div_ll(long long x, long long m) {
    assert(m > 0);
    long long q = x / m;
    long long r = x % m;
    if (r < 0) {
        --q;
    }
    return q;
}

void self_test() {
    for (long long n = 0; n <= 10; ++n) {
        for (long long m = 1; m <= 10; ++m) {
            for (long long a = -10; a <= 10; ++a) {
                for (long long b = -10; b <= 10; ++b) {
                    const auto res =
                        generalized_floor_sum_pq_le_2<long long>(n, m, a, b);

                    long long s01 = 0, s11 = 0, s02 = 0;
                    for (long long i = 0; i < n; ++i) {
                        const long long y = floor_div_ll(a * i + b, m);
                        s01 += y;
                        s11 += i * y;
                        s02 += y * y;
                    }
                    assert(res.ans_01 == s01);
                    assert(res.ans_11 == s11);
                    assert(res.ans_02 == s02);
                }
            }
        }
    }
}
} // namespace

int main() {
    self_test();
    return 0;
}
