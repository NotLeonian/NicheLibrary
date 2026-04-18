// competitive-verifier: PROBLEM https://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=1397

#include <cassert>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "../math/matrix/dynamic-matrix-rank.hpp"

struct F2 {
    int value;

    F2(int value = 0) : value(value & 1) {}

    F2 &operator+=(const F2 &other) {
        value ^= other.value;
        return *this;
    }

    F2 &operator-=(const F2 &other) {
        value ^= other.value;
        return *this;
    }

    F2 &operator*=(const F2 &other) {
        value &= other.value;
        return *this;
    }

    F2 &operator/=(const F2 &other) {
        assert(other.value == 1);
        return *this;
    }

    friend F2 operator+(F2 lhs, const F2 &rhs) { return lhs += rhs; }

    friend F2 operator-(F2 lhs, const F2 &rhs) { return lhs -= rhs; }

    friend F2 operator*(F2 lhs, const F2 &rhs) { return lhs *= rhs; }

    friend F2 operator/(F2 lhs, const F2 &rhs) { return lhs /= rhs; }

    friend bool operator==(const F2 &lhs, const F2 &rhs) {
        return lhs.value == rhs.value;
    }

    friend bool operator!=(const F2 &lhs, const F2 &rhs) {
        return lhs.value != rhs.value;
    }
};

template <class T> int brute_rank(std::vector<std::vector<T>> matrix) {
    const int h = static_cast<int>(matrix.size());
    if (h == 0)
        return 0;
    const int w = static_cast<int>(matrix[0].size());
    for (int i = 1; i < h; ++i)
        assert(static_cast<int>(matrix[i].size()) == w);

    int rank = 0;
    for (int column = 0; column < w; ++column) {
        int pivot = -1;
        for (int row = rank; row < h; ++row) {
            if (matrix[row][column] != T()) {
                pivot = row;
                break;
            }
        }
        if (pivot < 0)
            continue;
        if (pivot != rank)
            std::swap(matrix[pivot], matrix[rank]);

        const T inverse = T(1) / matrix[rank][column];
        for (int j = column; j < w; ++j)
            matrix[rank][j] *= inverse;
        for (int row = rank + 1; row < h; ++row) {
            const T factor = matrix[row][column];
            if (factor == T())
                continue;
            for (int j = column; j < w; ++j)
                matrix[row][j] -= matrix[rank][j] * factor;
        }
        ++rank;
        if (rank == h)
            break;
    }
    return rank;
}

void self_check() {
    for (int h = 1; h <= 2; ++h) {
        for (int w = 1; w <= 2; ++w) {
            const int states = 1 << (h * w);
            for (int mask = 0; mask < states; ++mask) {
                std::vector<std::vector<F2>> matrix(h,
                                                    std::vector<F2>(w, F2()));
                for (int i = 0; i < h; ++i) {
                    for (int j = 0; j < w; ++j) {
                        matrix[i][j] = F2((mask >> (i * w + j)) & 1);
                    }
                }
                DynamicMatrixRank<F2> solver(matrix);
                assert(solver.rank() == brute_rank(matrix));

                for (int column_mask = 0; column_mask < (1 << h);
                     ++column_mask) {
                    std::vector<F2> column_vector(h, F2());
                    for (int i = 0; i < h; ++i)
                        column_vector[i] = F2((column_mask >> i) & 1);
                    for (int row_mask = 0; row_mask < (1 << w); ++row_mask) {
                        std::vector<F2> row_vector(w, F2());
                        for (int j = 0; j < w; ++j)
                            row_vector[j] = F2((row_mask >> j) & 1);
                        auto updated = matrix;
                        for (int i = 0; i < h; ++i) {
                            for (int j = 0; j < w; ++j)
                                updated[i][j] +=
                                    column_vector[i] * row_vector[j];
                        }
                        assert(solver.rank_after_rank_one_update(column_vector,
                                                                 row_vector) ==
                               brute_rank(updated));
                    }
                }

                for (int row = 0; row < h; ++row) {
                    for (int row_mask = 0; row_mask < (1 << w); ++row_mask) {
                        std::vector<F2> new_row(w, F2());
                        for (int j = 0; j < w; ++j)
                            new_row[j] = F2((row_mask >> j) & 1);
                        auto updated = matrix;
                        updated[row] = new_row;
                        assert(solver.rank_after_row_replacement(
                                   row, new_row) == brute_rank(updated));
                    }
                }

                for (int column = 0; column < w; ++column) {
                    for (int column_mask = 0; column_mask < (1 << h);
                         ++column_mask) {
                        std::vector<F2> new_column(h, F2());
                        for (int i = 0; i < h; ++i)
                            new_column[i] = F2((column_mask >> i) & 1);
                        auto updated = matrix;
                        for (int i = 0; i < h; ++i)
                            updated[i][column] = new_column[i];
                        assert(solver.rank_after_column_replacement(
                                   column, new_column) == brute_rank(updated));
                    }
                }
            }
        }
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    self_check();

    int n, m;
    std::cin >> n >> m;
    std::vector<std::vector<F2>> matrix(n, std::vector<F2>(m, F2()));
    for (int i = 0; i < n; ++i) {
        std::string s;
        std::cin >> s;
        for (int j = 0; j < m; ++j)
            matrix[i][j] = F2(s[j] - '0');
    }

    DynamicMatrixRank<F2> solver(matrix);
    for (int i = 0; i < n; ++i) {
        std::string answer(m, '0');
        for (int j = 0; j < m; ++j) {
            const bool column_inside =
                solver.standard_column_in_column_space[i];
            const bool row_inside = solver.standard_row_in_row_space[j];
            if (!column_inside && !row_inside) {
                answer[j] = '+';
            } else if (column_inside != row_inside) {
                answer[j] = '0';
            } else {
                const int row_pos = solver.basis_row_position[i];
                const int column_pos = solver.basis_column_position[j];
                assert(row_pos >= 0 && column_pos >= 0);
                const F2 s =
                    F2(1) +
                    solver.intersection_inverse_matrix[column_pos][row_pos];
                answer[j] = s == F2() ? '-' : '0';
            }
        }
        std::cout << answer << '\n';
    }
}
