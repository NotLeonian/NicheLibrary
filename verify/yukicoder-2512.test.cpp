// competitive-verifier: PROBLEM https://yukicoder.me/problems/no/2512

#include <cstdint>
#include <iostream>
#include <vector>

#include "../math/combinatorics/online-binomial-sum.hpp"

class modint998244353 {
  public:
    static constexpr std::uint32_t mod = 998244353;

    modint998244353() : value_(0) {}

    modint998244353(long long value) {
        long long reduced = value % static_cast<long long>(mod);
        if (reduced < 0) {
            reduced += mod;
        }
        value_ = static_cast<std::uint32_t>(reduced);
    }

    std::uint32_t val() const { return value_; }

    modint998244353 inv() const { return pow(*this, mod - 2); }

    modint998244353 &operator+=(const modint998244353 &rhs) {
        std::uint32_t value = value_ + rhs.value_;
        if (value >= mod) {
            value -= mod;
        }
        value_ = value;
        return *this;
    }

    modint998244353 &operator-=(const modint998244353 &rhs) {
        const std::uint32_t value = value_ >= rhs.value_
                                        ? value_ - rhs.value_
                                        : value_ + mod - rhs.value_;
        value_ = value;
        return *this;
    }

    modint998244353 &operator*=(const modint998244353 &rhs) {
        const std::uint64_t value =
            static_cast<std::uint64_t>(value_) * rhs.value_ % mod;
        value_ = static_cast<std::uint32_t>(value);
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
        return lhs.value_ == rhs.value_;
    }

  private:
    static modint998244353 pow(modint998244353 base, long long exponent) {
        modint998244353 result(1);
        while (exponent > 0) {
            if (exponent % 2 == 1) {
                result *= base;
            }
            base *= base;
            exponent /= 2;
        }
        return result;
    }

    std::uint32_t value_;
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    constexpr int max_n = 200000;
    constexpr int max_m = 400000;
    static_assert(max_m < static_cast<int>(modint998244353::mod),
                  "max_m must be less than the modulus.");

    OnlineBinomialSum<modint998244353> online_binomial_sum(max_m,
                                                           modint998244353(-2));
    const modint998244353 minus_inv2 = modint998244353(-1) / modint998244353(2);
    std::vector<modint998244353> power_minus_inv2(max_n + 2);
    power_minus_inv2[0] = modint998244353(1);
    for (int i = 0; i <= max_n; ++i) {
        power_minus_inv2[i + 1] = power_minus_inv2[i] * minus_inv2;
    }

    int test_count;
    std::cin >> test_count;
    while (test_count--) {
        int n, m;
        std::cin >> n >> m;
        const modint998244353 sum =
            online_binomial_sum.binom_prefix_sum(n + 1, 2 * m);
        const modint998244353 ans =
            (sum - modint998244353(1)) *
            (modint998244353(0) - power_minus_inv2[n + 1]);
        std::cout << ans.val() << '\n';
    }

    return 0;
}
