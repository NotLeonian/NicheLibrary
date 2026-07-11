// competitive-verifier: STANDALONE

#include <cassert>
#include <cstdint>
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

using mint = modint998244353;

mint brute_prefix_sum(const std::vector<std::vector<mint>> &binomial, int n,
                      int m, mint r) {
    mint ans;
    mint power_r(1);
    for (int i = 0; i <= m && i < n; ++i) {
        ans += power_r * binomial[m][i];
        power_r *= r;
    }
    return ans;
}

mint brute_sum(const std::vector<std::vector<mint>> &binomial, int l, int u,
               int m, mint r) {
    mint ans;
    mint power_r(1);
    for (int i = 0; i <= m; ++i) {
        if (l <= i && i < u) {
            ans += power_r * binomial[m][i];
        }
        power_r *= r;
    }
    return ans;
}

void verify(const std::vector<std::vector<mint>> &binomial,
            const OnlineBinomialSum<mint> &online_binomial_sum, int max_m,
            mint r) {
    for (int m = 0; m <= max_m; ++m) {
        for (int n = 0; n <= max_m + 10; ++n) {
            assert(online_binomial_sum.binom_prefix_sum(n, m) ==
                   brute_prefix_sum(binomial, n, m, r));
        }
        for (int l = 0; l <= max_m + 5; ++l) {
            for (int u = l; u <= max_m + 10; ++u) {
                assert(online_binomial_sum.binom_sum(l, u, m) ==
                       brute_sum(binomial, l, u, m, r));
            }
        }
    }
}

int main() {
    constexpr int max_m = 30;
    static_assert(max_m < static_cast<int>(mint::mod),
                  "max_m must be less than the modulus.");

    std::vector<std::vector<mint>> binomial(max_m + 1,
                                            std::vector<mint>(max_m + 1));
    for (int n = 0; n <= max_m; ++n) {
        binomial[n][0] = mint(1);
        binomial[n][n] = mint(1);
        for (int k = 1; k < n; ++k) {
            binomial[n][k] = binomial[n - 1][k - 1] + binomial[n - 1][k];
        }
    }

    const std::vector<int> bucket_size_list = {1, 2, 3, 4, 5, 7, 16, 31, 64};
    for (long long r_value : {-3, -2, -1, 0, 1, 2, 3}) {
        const mint r(r_value);
        OnlineBinomialSum<mint> online_binomial_sum(max_m, r);
        if (r == mint(0)) {
            assert(online_binomial_sum.bucket_size == 1);
        }
        verify(binomial, online_binomial_sum, max_m, r);

        for (int bucket_size : bucket_size_list) {
            OnlineBinomialSum<mint> online_binomial_sum_with_bucket(
                max_m, r, bucket_size);
            assert(online_binomial_sum_with_bucket.bucket_size == bucket_size);
            verify(binomial, online_binomial_sum_with_bucket, max_m, r);

            if (r == mint(0)) {
                assert(online_binomial_sum_with_bucket.factorial.empty());
                assert(
                    online_binomial_sum_with_bucket.sample_sum_table.empty());
            } else if (r == mint(-1)) {
                assert(!online_binomial_sum_with_bucket.factorial.empty());
                assert(
                    online_binomial_sum_with_bucket.sample_sum_table.empty());
            } else {
                assert(
                    !online_binomial_sum_with_bucket.sample_sum_table.empty());
            }
        }
    }

    for (long long r_value : {-1, 0, 2}) {
        OnlineBinomialSum<mint> online_binomial_sum(0, mint(r_value), 7);
        assert(online_binomial_sum.binom_prefix_sum(0, 0) == mint(0));
        assert(online_binomial_sum.binom_prefix_sum(1, 0) == mint(1));
        assert(online_binomial_sum.binom_prefix_sum(10, 0) == mint(1));
    }

    return 0;
}
