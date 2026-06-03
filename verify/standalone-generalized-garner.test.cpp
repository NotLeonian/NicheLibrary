// competitive-verifier: STANDALONE
#include <cassert>
#include <vector>

#include "../math/number-theory/generalized-garner.hpp"

namespace {
struct Equation {
    long long a;
    long long b;
    long long m;
};

long long safe_mod_ll(long long x, long long m) {
    assert(m > 0);
    x %= m;
    if (x < 0) {
        x += m;
    }
    return x;
}

long long gcd_ll(long long a, long long b) {
    if (a < 0) {
        a = -a;
    }
    if (b < 0) {
        b = -b;
    }
    while (b != 0) {
        const long long c = a % b;
        a = b;
        b = c;
    }
    return a;
}

long long lcm_ll(long long a, long long b) { return a / gcd_ll(a, b) * b; }

bool satisfy(long long x, const Equation &e) {
    return safe_mod_ll(e.a * x - e.b, e.m) == 0;
}

void check(const std::vector<Equation> &eqs) {
    std::vector<long long> a;
    std::vector<long long> b;
    std::vector<long long> m;
    long long period = 1;
    for (const Equation &e : eqs) {
        a.push_back(e.a);
        b.push_back(e.b);
        m.push_back(e.m);
        period = lcm_ll(period, e.m);
    }
    const auto got = generalized_garner<long long>(a, b, m);
    std::vector<bool> good(period, false);
    bool exists = false;
    for (long long x = 0; x < period; ++x) {
        bool ok = true;
        for (const Equation &e : eqs) {
            if (!satisfy(x, e)) {
                ok = false;
            }
        }
        good[x] = ok;
        if (ok) {
            exists = true;
        }
    }
    if (!exists) {
        assert(got.first == 0);
        assert(got.second == 0);
        return;
    }
    long long want_rem = 0;
    while (!good[want_rem]) {
        ++want_rem;
    }
    long long want_mod = period;
    for (long long x = want_rem + 1; x < period; ++x) {
        if (good[x]) {
            want_mod = gcd_ll(want_mod, x - want_rem);
        }
    }
    assert(got.first == want_rem);
    assert(got.second == want_mod);
}

void self_test_small() {
    check({});
    std::vector<Equation> eqs;
    for (long long m = 1; m <= 6; ++m) {
        for (long long a = -5; a <= 5; ++a) {
            for (long long b = -5; b <= 5; ++b) {
                eqs.push_back({a, b, m});
            }
        }
    }
    for (const Equation &e0 : eqs) {
        check({e0});
    }
    for (const Equation &e0 : eqs) {
        for (const Equation &e1 : eqs) {
            check({e0, e1});
        }
    }
    for (int i = 0; i < static_cast<int>(eqs.size()); i += 37) {
        for (int j = 0; j < static_cast<int>(eqs.size()); j += 41) {
            for (int k = 0; k < static_cast<int>(eqs.size()); k += 43) {
                check({eqs[i], eqs[j], eqs[k]});
            }
        }
    }
}

#ifdef __SIZEOF_INT128__
void self_test_int128() {
    using i128 = __int128_t;
    const i128 x0 = 123456789;
    const i128 m0 = i128(1) << 70;
    const i128 m1 = i128(1) << 69;
    {
        const std::vector<i128> a = {3, 5};
        const std::vector<i128> b = {3 * x0, 5 * x0};
        const std::vector<i128> m = {m0, m1};
        const auto res = generalized_garner<i128>(a, b, m);
        assert(res.first == x0);
        assert(res.second == m0);
    }
    {
        const std::vector<i128> a = {1, 1};
        const std::vector<i128> b = {0, 1};
        const std::vector<i128> m = {m0, m1};
        const auto res = generalized_garner<i128>(a, b, m);
        assert(res.first == 0);
        assert(res.second == 0);
    }
}
#endif
} // namespace

int main() {
    self_test_small();
#ifdef __SIZEOF_INT128__
    self_test_int128();
#endif
    return 0;
}
