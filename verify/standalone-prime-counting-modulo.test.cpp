// competitive-verifier: STANDALONE

#include <cassert>
#include <vector>

#include "../math/multiplicative-function/prime-counting-modulo.hpp"

namespace {
std::vector<int> prime_table(int N) {
    std::vector<int> is_prime(N + 1, 1);
    if (N >= 0) {
        is_prime[0] = 0;
    }
    if (N >= 1) {
        is_prime[1] = 0;
    }
    for (int p = 2; p <= N / p; ++p) {
        if (!is_prime[p]) {
            continue;
        }
        for (int q = p * p; q <= N; q += p) {
            is_prime[q] = 0;
        }
    }
    return is_prime;
}

void self_test() {
    for (long long N = 0; N <= 500; ++N) {
        const auto is_prime = prime_table(static_cast<int>(N));
        for (long long m = 1; m <= 30; ++m) {
            const auto table = prime_counting_modulo_table(N, m);
            const auto &ns = table.first;
            const auto &h = table.second;
            assert(static_cast<long long>(h.size()) == m);
            for (long long r = 0; r < m; ++r) {
                assert(h[r].size() == ns.size());
                for (long long i = 0; i < static_cast<long long>(ns.size());
                     ++i) {
                    long long naive = 0;
                    for (long long p = 2; p <= ns[i]; ++p) {
                        if (is_prime[p] && p % m == r) {
                            ++naive;
                        }
                    }
                    assert(h[r][i] == naive);
                }
            }

            const auto res = prime_counting_modulo(N, m);
            for (long long r = 0; r < m; ++r) {
                long long naive = 0;
                for (long long p = 2; p <= N; ++p) {
                    if (is_prime[p] && p % m == r) {
                        ++naive;
                    }
                }
                assert(res[r] == naive);
            }

            const auto mf =
                prime_counting_modulo_mf_prefix_sum_table<long long>(N, m);
            assert(static_cast<long long>(mf.size()) == m);
            for (long long r = 0; r < m; ++r) {
                if (N == 0) {
                    assert(mf[r].empty());
                } else {
                    assert(mf[r].size() == h[r].size());
                    for (long long i = 0;
                         i < static_cast<long long>(h[r].size()); ++i) {
                        assert(mf[r][i] == h[r][i]);
                    }
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
