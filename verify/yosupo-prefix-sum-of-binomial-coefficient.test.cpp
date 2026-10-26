// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/prefix_sum_of_binomial_coefficient

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <utility>
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

    int t;
    std::cin >> t;
    std::vector<std::pair<int, int>> queries(t);
    int max_n = 0;
    for (auto &[n, k] : queries) {
        std::cin >> n >> k;
        max_n = std::max(max_n, n);
    }

    const OnlineBinomialSum<modint998244353> online_binomial_sum(max_n);
    for (const auto &[n, k] : queries) {
        std::cout << online_binomial_sum.binom_prefix_sum(k + 1, n).val()
                  << '\n';
    }

    return 0;
}
