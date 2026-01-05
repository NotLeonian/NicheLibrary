#ifndef MATH_MATRIX_DETERMINANT_OF_LINEAR_MATRIX_POLYNOMIAL_HPP
#define MATH_MATRIX_DETERMINANT_OF_LINEAR_MATRIX_POLYNOMIAL_HPP

// 一次行列多項式 det(M0 + x M1) を係数列として求める。
// 体上でのみ動作する（除算が必要）。M0, M1 は N×N。
// 多項式は a[0] + a[1]x + ... + a[N]x^N の昇順で返す。
// M1 を掃き出して I にし、det(xI + A) を特性多項式に帰着する。
// M1 が特異でも列に x を掛ける操作を挟むことで次数 1 を保つ。
// 計算量 O(N^3)。

#include <cassert>
#include <utility>
#include <vector>

template <class T>
void hessenberg_reduction(std::vector<std::vector<T>> &matrix) {
    const int n = static_cast<int>(matrix.size());
    assert(n == 0 || static_cast<int>(matrix[0].size()) == n);
    for (int i = 1; i < n; ++i)
        assert(static_cast<int>(matrix[i].size()) == n);

    for (int r = 0; r < n - 2; ++r) {
        int piv = -1;
        for (int h = r + 1; h < n; ++h) {
            if (matrix[h][r] != T()) {
                piv = h;
                break;
            }
        }
        if (piv < 0)
            continue;

        if (piv != r + 1) {
            matrix[r + 1].swap(matrix[piv]);
            for (int i = 0; i < n; ++i)
                std::swap(matrix[i][r + 1], matrix[i][piv]);
        }

        const T rinv = T(1) / matrix[r + 1][r];
        for (int i = r + 2; i < n; ++i) {
            const T coef = matrix[i][r] * rinv;
            if (coef == T())
                continue;
            for (int j = 0; j < n; ++j)
                matrix[i][j] -= matrix[r + 1][j] * coef;
            for (int j = 0; j < n; ++j)
                matrix[j][r + 1] += matrix[j][i] * coef;
        }
    }
}

template <class T>
std::vector<T> characteristic_polynomial(std::vector<std::vector<T>> matrix) {
    const int n = static_cast<int>(matrix.size());
    assert(n == 0 || static_cast<int>(matrix[0].size()) == n);
    for (int i = 1; i < n; ++i)
        assert(static_cast<int>(matrix[i].size()) == n);

    hessenberg_reduction(matrix);

    // p[i] = det(x I_i - matrix[0..i-1][0..i-1])（係数は昇順）
    std::vector<std::vector<T>> p(n + 1);
    p[0] = {T(1)};
    for (int i = 0; i < n; ++i) {
        p[i + 1].assign(i + 2, T());

        for (int j = 0; j <= i; ++j)
            p[i + 1][j + 1] += p[i][j];
        for (int j = 0; j <= i; ++j)
            p[i + 1][j] -= p[i][j] * matrix[i][i];

        T betas = T(1);
        for (int j = i - 1; j >= 0; --j) {
            betas *= matrix[j + 1][j];
            const T hb = (T() - matrix[j][i]) * betas;
            for (int k = 0; k <= j; ++k)
                p[i + 1][k] += hb * p[j][k];
        }
    }
    return p[n];
}

template <class T>
std::vector<T>
determinant_of_linear_matrix_polynomial(std::vector<std::vector<T>> M0,
                                        std::vector<std::vector<T>> M1) {
    const int n = static_cast<int>(M0.size());
    assert(static_cast<int>(M1.size()) == n);
    if (n == 0)
        return {T(1)};
    for (int i = 0; i < n; ++i) {
        assert(static_cast<int>(M0[i].size()) == n);
        assert(static_cast<int>(M1[i].size()) == n);
    }

    int multiply_by_x = 0; // 「特定の列に x を掛ける」操作の回数
    T det_inv = T(1);      // 1 / (det A det B)

    for (int p = 0; p < n; ++p) {
        int pivot = -1;
        for (int row = p; row < n; ++row) {
            if (M1[row][p] != T()) {
                pivot = row;
                break;
            }
        }

        if (pivot < 0) {
            ++multiply_by_x;
            if (multiply_by_x > n)
                return std::vector<T>(n + 1, T());

            // x^2 の項を発生させないため、M1[0..p-1][p] を先に消す（列基本変形）。
            for (int row = 0; row < p; ++row) {
                const T v = M1[row][p];
                if (v == T())
                    continue;
                M1[row][p] = T();
                for (int i = 0; i < n; ++i)
                    M0[i][p] -= v * M0[i][row];
            }

            // (M0 + x M1) の p 列に x を掛ける（M1 の p 列が 0 のとき swap で実現できる）。
            for (int i = 0; i < n; ++i)
                std::swap(M0[i][p], M1[i][p]);

            --p; // 同じ列をやり直す（高々 n 回）
            continue;
        }

        if (pivot != p) {
            M0[pivot].swap(M0[p]);
            M1[pivot].swap(M1[p]);
            det_inv = T() - det_inv; // *= -1
        }

        const T v = M1[p][p];
        assert(v != T());
        det_inv *= v;
        const T vinv = T(1) / v;
        for (int col = 0; col < n; ++col) {
            M0[p][col] *= vinv;
            M1[p][col] *= vinv;
        }

        for (int row = 0; row < n; ++row) {
            if (row == p)
                continue;
            const T coef = M1[row][p];
            if (coef == T())
                continue;
            for (int col = 0; col < n; ++col) {
                M0[row][col] -= M0[p][col] * coef;
                M1[row][col] -= M1[p][col] * coef;
            }
        }
    }

    // M1 = I なので det(x I + M0) を求める（特性多項式 det(x I - (-M0))）。
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j)
            M0[i][j] = T() - M0[i][j];
    }
    std::vector<T> poly = characteristic_polynomial(M0);
    for (T &c : poly)
        c *= det_inv;

    if (multiply_by_x > 0)
        poly.erase(poly.begin(), poly.begin() + multiply_by_x);
    poly.resize(n + 1, T());
    return poly;
}

#endif