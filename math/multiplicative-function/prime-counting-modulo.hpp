#ifndef MATH_MULTIPLICATIVE_FUNCTION_PRIME_COUNTING_MODULO_HPP
#define MATH_MULTIPLICATIVE_FUNCTION_PRIME_COUNTING_MODULO_HPP

// N 以下の素数を、m で割った余りごとに数える。
// Lucy DP のテーブルを余りごとに持ち、素数 x によるふるいを同時に行う。
// N は非負、m は正を仮定する。
// 戻り値のテーブルの 1 つ目の添字は m で割った余りである。
// 計算量 O(m N^{3/4} / log N)、空間 O(m sqrt(N))。

#include <cassert>
#include <utility>
#include <vector>

namespace prime_counting_modulo_internal {

long long integer_sqrt(long long n) {
    assert(n >= 0);
    long long ok = 0, ng = 1;
    while (ng <= n / ng)
        ng <<= 1;
    while (ng - ok > 1) {
        const long long mid = ok + (ng - ok) / 2;
        if (mid <= n / mid) {
            ok = mid;
        } else {
            ng = mid;
        }
    }
    return ok;
}

long long count_residue_2_to_n(long long n, long long m, long long r) {
    assert(n >= 0);
    assert(m > 0);
    assert(0 <= r && r < m);
    long long res = 0;
    if (r <= n)
        res = (n - r) / m + 1;
    if (r == 0)
        --res;
    if (n >= 1 && r == 1 % m)
        --res;
    return res;
}

void add_mod(long long &x, long long a, long long m) {
    assert(0 <= x && x < m);
    assert(0 <= a && a < m);
    if (a == 0)
        return;
    if (x >= m - a) {
        x -= m - a;
    } else {
        x += a;
    }
}

} // namespace prime_counting_modulo_internal

std::pair<std::vector<long long>, std::vector<std::vector<long long>>>
prime_counting_modulo_table(long long N, long long m) {
    assert(N >= 0);
    assert(m > 0);
    using i64 = long long;
    std::vector<i64> ns{0};
    for (i64 i = N; i > 0;) {
        ns.push_back(i);
        const i64 q = N / i;
        if (q == N)
            break;
        i = N / (q + 1);
    }
    const i64 sq = prime_counting_modulo_internal::integer_sqrt(N);
    const i64 nsz = static_cast<i64>(ns.size());
    std::vector<std::vector<i64>> h(m, std::vector<i64>(nsz));
    for (i64 r = 0; r < m; ++r) {
        for (i64 i = 0; i < nsz; ++i) {
            h[r][i] = prime_counting_modulo_internal::count_residue_2_to_n(
                ns[i], m, r);
        }
    }
    for (i64 x = 2; x <= sq; ++x) {
        const i64 x_mod = x % m;
        const i64 x_idx = nsz - x;
        const i64 prev_idx = nsz - x + 1;
        if (h[x_mod][x_idx] == h[x_mod][prev_idx])
            continue;
        const i64 x2 = x * x;
        for (i64 i = 1; i < nsz && ns[i] >= x2; ++i) {
            const i64 n = ns[i];
            const i64 q = n / x;
            const i64 q_idx = i <= sq / x ? i * x : nsz - q;
            i64 to = 0;
            for (i64 r = 0; r < m; ++r) {
                h[to][i] -= h[r][q_idx] - h[r][prev_idx];
                prime_counting_modulo_internal::add_mod(to, x_mod, m);
            }
        }
    }
    return {ns, h};
}

std::vector<long long> prime_counting_modulo(long long N, long long m) {
    assert(N >= 0);
    assert(m > 0);
    std::vector<long long> res(m);
    if (N == 0)
        return res;
    const auto table = prime_counting_modulo_table(N, m).second;
    for (long long r = 0; r < m; ++r)
        res[r] = table[r][1];
    return res;
}

template <class T>
std::vector<std::vector<T>>
prime_counting_modulo_mf_prefix_sum_table(long long N, long long m) {
    assert(N >= 0);
    assert(m > 0);
    std::vector<std::vector<T>> res(m);
    if (N == 0)
        return res;
    const auto table = prime_counting_modulo_table(N, m).second;
    for (long long r = 0; r < m; ++r) {
        res[r].resize(table[r].size());
        for (long long i = 0; i < static_cast<long long>(table[r].size());
             ++i) {
            res[r][i] = static_cast<T>(table[r][i]);
        }
    }
    return res;
}

#endif
