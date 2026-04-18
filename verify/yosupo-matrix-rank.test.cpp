// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/matrix_rank

#include <cassert>
#include <iostream>
#include <vector>

#include "../math/matrix/dynamic-matrix-rank.hpp"

struct ModInt998244353 {
    static constexpr int mod = 998244353;
    int value;

    ModInt998244353(long long value = 0) {
        value %= mod;
        if (value < 0)
            value += mod;
        this->value = static_cast<int>(value);
    }

    ModInt998244353 &operator+=(const ModInt998244353 &other) {
        value += other.value;
        if (value >= mod)
            value -= mod;
        return *this;
    }

    ModInt998244353 &operator-=(const ModInt998244353 &other) {
        value -= other.value;
        if (value < 0)
            value += mod;
        return *this;
    }

    ModInt998244353 &operator*=(const ModInt998244353 &other) {
        value = static_cast<int>(1LL * value * other.value % mod);
        return *this;
    }

    static long long pow_mod(long long x, long long n) {
        long long result = 1;
        while (n > 0) {
            if (n & 1)
                result = result * x % mod;
            x = x * x % mod;
            n >>= 1;
        }
        return result;
    }

    ModInt998244353 inv() const {
        assert(value != 0);
        return ModInt998244353(pow_mod(value, mod - 2));
    }

    ModInt998244353 &operator/=(const ModInt998244353 &other) {
        return *this *= other.inv();
    }

    friend ModInt998244353 operator+(ModInt998244353 lhs,
                                     const ModInt998244353 &rhs) {
        return lhs += rhs;
    }

    friend ModInt998244353 operator-(ModInt998244353 lhs,
                                     const ModInt998244353 &rhs) {
        return lhs -= rhs;
    }

    friend ModInt998244353 operator*(ModInt998244353 lhs,
                                     const ModInt998244353 &rhs) {
        return lhs *= rhs;
    }

    friend ModInt998244353 operator/(ModInt998244353 lhs,
                                     const ModInt998244353 &rhs) {
        return lhs /= rhs;
    }

    friend bool operator==(const ModInt998244353 &lhs,
                           const ModInt998244353 &rhs) {
        return lhs.value == rhs.value;
    }

    friend bool operator!=(const ModInt998244353 &lhs,
                           const ModInt998244353 &rhs) {
        return lhs.value != rhs.value;
    }
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, m;
    std::cin >> n >> m;
    std::vector<std::vector<ModInt998244353>> matrix(
        n, std::vector<ModInt998244353>(m, ModInt998244353()));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            long long x;
            std::cin >> x;
            matrix[i][j] = ModInt998244353(x);
        }
    }

    DynamicMatrixRank<ModInt998244353> solver(matrix);
    std::cout << solver.rank() << '\n';
}
