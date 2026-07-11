// competitive-verifier: STANDALONE

#include <cassert>
#include <vector>

#include "../math/matrix/determinant-of-linear-matrix-polynomial.hpp"

namespace {
struct ModInt101 {
    static constexpr int mod = 101;
    int value;

    ModInt101(long long value = 0) {
        value %= mod;
        if (value < 0) {
            value += mod;
        }
        this->value = static_cast<int>(value);
    }

    ModInt101 &operator+=(const ModInt101 &other) {
        value += other.value;
        if (value >= mod) {
            value -= mod;
        }
        return *this;
    }

    ModInt101 &operator-=(const ModInt101 &other) {
        value -= other.value;
        if (value < 0) {
            value += mod;
        }
        return *this;
    }

    ModInt101 &operator*=(const ModInt101 &other) {
        value = value * other.value % mod;
        return *this;
    }

    ModInt101 &operator/=(const ModInt101 &other) {
        assert(other != ModInt101());
        return *this *= power(other, mod - 2);
    }

    friend ModInt101 operator-(ModInt101 lhs, const ModInt101 &rhs) {
        return lhs -= rhs;
    }

    friend ModInt101 operator*(ModInt101 lhs, const ModInt101 &rhs) {
        return lhs *= rhs;
    }

    friend ModInt101 operator/(ModInt101 lhs, const ModInt101 &rhs) {
        return lhs /= rhs;
    }

    friend bool operator==(const ModInt101 &lhs, const ModInt101 &rhs) {
        return lhs.value == rhs.value;
    }

    friend bool operator!=(const ModInt101 &lhs, const ModInt101 &rhs) {
        return lhs.value != rhs.value;
    }

  private:
    static ModInt101 power(ModInt101 base, int exponent) {
        ModInt101 result(1);
        while (exponent > 0) {
            if (exponent % 2 == 1) {
                result *= base;
            }
            base *= base;
            exponent /= 2;
        }
        return result;
    }
};

using Matrix = std::vector<std::vector<ModInt101>>;

ModInt101 determinant(Matrix matrix) {
    const int n = static_cast<int>(matrix.size());
    ModInt101 result(1);
    for (int column = 0; column < n; ++column) {
        int pivot = -1;
        for (int row = column; row < n; ++row) {
            if (matrix[row][column] != ModInt101()) {
                pivot = row;
                break;
            }
        }
        if (pivot < 0) {
            return ModInt101();
        }
        if (pivot != column) {
            matrix[pivot].swap(matrix[column]);
            result = ModInt101() - result;
        }

        result *= matrix[column][column];
        const ModInt101 pivot_inverse = ModInt101(1) / matrix[column][column];
        for (int row = column + 1; row < n; ++row) {
            const ModInt101 factor = matrix[row][column] * pivot_inverse;
            for (int j = column + 1; j < n; ++j) {
                matrix[row][j] -= matrix[column][j] * factor;
            }
        }
    }
    return result;
}

ModInt101 evaluate(const std::vector<ModInt101> &polynomial, ModInt101 x) {
    ModInt101 result;
    for (int i = static_cast<int>(polynomial.size()) - 1; i >= 0; --i) {
        result *= x;
        result += polynomial[i];
    }
    return result;
}

Matrix matrix_from_mask(int n, int mask) {
    Matrix matrix(n, std::vector<ModInt101>(n));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            matrix[i][j] = ModInt101((mask >> (i * n + j)) % 2);
        }
    }
    return matrix;
}

void check_characteristic_polynomial(const Matrix &matrix) {
    const int n = static_cast<int>(matrix.size());
    const std::vector<ModInt101> polynomial = characteristic_polynomial(matrix);
    assert(static_cast<int>(polynomial.size()) == n + 1);

    Matrix hessenberg = matrix;
    hessenberg_reduction(hessenberg);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j + 1 < i; ++j) {
            assert(hessenberg[i][j] == ModInt101());
        }
    }

    for (int x = 0; x <= n; ++x) {
        Matrix shifted = matrix;
        Matrix shifted_hessenberg = hessenberg;
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                const ModInt101 diagonal = i == j ? ModInt101(x) : ModInt101();
                shifted[i][j] = diagonal - shifted[i][j];
                shifted_hessenberg[i][j] = diagonal - shifted_hessenberg[i][j];
            }
        }
        const ModInt101 expected = determinant(shifted);
        assert(evaluate(polynomial, ModInt101(x)) == expected);
        assert(determinant(shifted_hessenberg) == expected);
    }
}

void check_linear_matrix_polynomial(const Matrix &matrix_0,
                                    const Matrix &matrix_1) {
    const int n = static_cast<int>(matrix_0.size());
    const std::vector<ModInt101> polynomial =
        determinant_of_linear_matrix_polynomial(matrix_0, matrix_1);
    assert(static_cast<int>(polynomial.size()) == n + 1);

    for (int x = 0; x <= n; ++x) {
        Matrix evaluated = matrix_0;
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                evaluated[i][j] += ModInt101(x) * matrix_1[i][j];
            }
        }
        assert(evaluate(polynomial, ModInt101(x)) == determinant(evaluated));
    }
}

void self_test() {
    check_characteristic_polynomial({});
    check_linear_matrix_polynomial({}, {});

    for (int n = 1; n <= 3; ++n) {
        const int state_count = 1 << (n * n);
        for (int mask = 0; mask < state_count; ++mask) {
            check_characteristic_polynomial(matrix_from_mask(n, mask));
        }
    }

    for (int n = 1; n <= 2; ++n) {
        const int state_count = 1 << (n * n);
        for (int mask_0 = 0; mask_0 < state_count; ++mask_0) {
            for (int mask_1 = 0; mask_1 < state_count; ++mask_1) {
                check_linear_matrix_polynomial(matrix_from_mask(n, mask_0),
                                               matrix_from_mask(n, mask_1));
            }
        }
    }

    constexpr int state_count = 1 << 9;
    for (int mask = 0; mask < state_count; ++mask) {
        check_linear_matrix_polynomial(matrix_from_mask(3, mask),
                                       matrix_from_mask(3, 0));
        check_linear_matrix_polynomial(
            matrix_from_mask(3, mask),
            matrix_from_mask(3, (mask * 137 + 91) & (state_count - 1)));
    }
}
} // namespace

int main() {
    self_test();

    return 0;
}
