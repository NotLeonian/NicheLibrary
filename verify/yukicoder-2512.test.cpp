// competitive-verifier: PROBLEM https://yukicoder.me/problems/no/2512

#include <cstdint>
#include <iostream>
#include <vector>

#include "../math/combinatorics/online-binomial-sum.hpp"

class modint998244353 {
  public:
    static constexpr std::uint32_t mod = 998244353;

    modint998244353() : v_(0) {}
    modint998244353(long long x) {
        long long y = x % static_cast<long long>(mod);
        if (y < 0) {
            y += mod;
        }
        v_ = static_cast<std::uint32_t>(y);
    }

    std::uint32_t val() const { return v_; }
    modint998244353 inv() const { return pow(*this, mod - 2); }
    modint998244353 &operator+=(const modint998244353 &rhs) {
        std::uint32_t x = v_ + rhs.v_;
        if (x >= mod) {
            x -= mod;
        }
        v_ = x;
        return *this;
    }
    modint998244353 &operator-=(const modint998244353 &rhs) {
        std::uint32_t x = (v_ >= rhs.v_) ? (v_ - rhs.v_) : (v_ + mod - rhs.v_);
        v_ = x;
        return *this;
    }
    modint998244353 &operator*=(const modint998244353 &rhs) {
        std::uint64_t x = static_cast<std::uint64_t>(v_) * rhs.v_ % mod;
        v_ = static_cast<std::uint32_t>(x);
        return *this;
    }
    modint998244353 &operator/=(const modint998244353 &rhs) {
        return *this *= rhs.inv();
    }
    friend modint998244353 operator+(modint998244353 lhs,
                                     const modint998244353 &rhs) {
        return lhs += rhs;
    }
    friend modint998244353 operator-(modint998244353 lhs,
                                     const modint998244353 &rhs) {
        return lhs -= rhs;
    }
    friend modint998244353 operator*(modint998244353 lhs,
                                     const modint998244353 &rhs) {
        return lhs *= rhs;
    }
    friend modint998244353 operator/(modint998244353 lhs,
                                     const modint998244353 &rhs) {
        return lhs /= rhs;
    }
    friend bool operator==(const modint998244353 &lhs,
                           const modint998244353 &rhs) {
        return lhs.v_ == rhs.v_;
    }
    friend bool operator!=(const modint998244353 &lhs,
                           const modint998244353 &rhs) {
        return lhs.v_ != rhs.v_;
    }

  private:
    static modint998244353 pow(modint998244353 a, long long e) {
        modint998244353 r(1);
        while (e > 0) {
            if (e & 1) {
                r *= a;
            }
            a *= a;
            e >>= 1;
        }
        return r;
    }

    std::uint32_t v_;
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    constexpr int max_n = 200000;
    constexpr int max_m = 400000;
    constexpr int bucket_size = 256;
    OnlineBinomialSum<modint998244353> binom_sum(max_m, modint998244353(-2),
                                                 bucket_size);

    const modint998244353 minus_inv2 = modint998244353(-1) / modint998244353(2);
    std::vector<modint998244353> pow_minus_inv2(max_n + 2);
    pow_minus_inv2[0] = modint998244353(1);
    for (int i = 0; i <= max_n; ++i) {
        pow_minus_inv2[i + 1] = pow_minus_inv2[i] * minus_inv2;
    }

    int t;
    std::cin >> t;
    while (t--) {
        int n, m;
        std::cin >> n >> m;
        const modint998244353 s = binom_sum.binom_prefix_sum(n + 1, 2 * m);
        const modint998244353 ans =
            (s - modint998244353(1)) *
            (modint998244353(0) - pow_minus_inv2[n + 1]);
        std::cout << ans.val() << '\n';
    }

    return 0;
}
