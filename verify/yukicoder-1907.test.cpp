// competitive-verifier: PROBLEM https://yukicoder.me/problems/no/1907

#include <cstdint>
#include <iostream>
#include <vector>

#include "../math/matrix/determinant-of-linear-matrix-polynomial.hpp"

class modint998244353 {
  public:
    static constexpr std::uint32_t mod = 998244353;

    modint998244353() : v_(0) {}
    modint998244353(long long x) {
        long long y = x % static_cast<long long>(mod);
        if (y < 0)
            y += mod;
        v_ = static_cast<std::uint32_t>(y);
    }

    std::uint32_t val() const { return v_; }

    modint998244353 inv() const { return pow(*this, mod - 2); }

    modint998244353 &operator+=(const modint998244353 &rhs) {
        std::uint32_t x = v_ + rhs.v_;
        if (x >= mod)
            x -= mod;
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
            if (e & 1)
                r *= a;
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

    int N;
    std::cin >> N;

    std::vector<std::vector<modint998244353>> M0(
        N, std::vector<modint998244353>(N));
    std::vector<std::vector<modint998244353>> M1(
        N, std::vector<modint998244353>(N));

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            int x;
            std::cin >> x;
            M0[i][j] = modint998244353(x);
        }
    }
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            int x;
            std::cin >> x;
            M1[i][j] = modint998244353(x);
        }
    }

    const std::vector<modint998244353> ans =
        determinant_of_linear_matrix_polynomial(M0, M1);
    for (const auto &a : ans) {
        std::cout << a.val() << '\n';
    }
    return 0;
}