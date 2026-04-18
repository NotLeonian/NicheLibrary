// competitive-verifier: PROBLEM https://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=1397

#include <cassert>
#include <iostream>
#include <string>
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

template <class T>
std::vector<std::vector<T>>
transpose_matrix(const std::vector<std::vector<T>> &a) {
    const int h = static_cast<int>(a.size());
    if (h == 0)
        return {};
    const int w = static_cast<int>(a[0].size());
    std::vector<std::vector<T>> transposed(w, std::vector<T>(h, T()));
    for (int i = 0; i < h; ++i) {
        assert(static_cast<int>(a[i].size()) == w);
        for (int j = 0; j < w; ++j)
            transposed[j][i] = a[i][j];
    }
    return transposed;
}

template <class T>
std::vector<int> find_independent_columns(std::vector<std::vector<T>> a) {
    const int h = static_cast<int>(a.size());
    if (h == 0)
        return {};
    const int w = static_cast<int>(a[0].size());
    int rank = 0;
    std::vector<int> pivot_columns;
    for (int column = 0; column < w; ++column) {
        int pivot = -1;
        for (int row = rank; row < h; ++row) {
            if (a[row][column] != T()) {
                pivot = row;
                break;
            }
        }
        if (pivot < 0)
            continue;
        if (pivot != rank)
            a[pivot].swap(a[rank]);
        const T inverse = T(1) / a[rank][column];
        for (int j = column; j < w; ++j)
            a[rank][j] *= inverse;
        for (int row = rank + 1; row < h; ++row) {
            const T factor = a[row][column];
            if (factor == T())
                continue;
            for (int j = column; j < w; ++j)
                a[row][j] -= a[rank][j] * factor;
        }
        pivot_columns.push_back(column);
        ++rank;
        if (rank == h)
            break;
    }
    return pivot_columns;
}

template <class T>
std::vector<std::vector<T>> inverse_matrix(std::vector<std::vector<T>> a) {
    const int n = static_cast<int>(a.size());
    for (int i = 0; i < n; ++i)
        assert(static_cast<int>(a[i].size()) == n);

    std::vector<std::vector<T>> inverse(n, std::vector<T>(n, T()));
    for (int i = 0; i < n; ++i)
        inverse[i][i] = T(1);

    for (int column = 0; column < n; ++column) {
        int pivot = -1;
        for (int row = column; row < n; ++row) {
            if (a[row][column] != T()) {
                pivot = row;
                break;
            }
        }
        assert(pivot >= 0);
        if (pivot != column) {
            a[pivot].swap(a[column]);
            inverse[pivot].swap(inverse[column]);
        }
        const T diagonal_inverse = T(1) / a[column][column];
        for (int j = 0; j < n; ++j) {
            a[column][j] *= diagonal_inverse;
            inverse[column][j] *= diagonal_inverse;
        }
        for (int row = 0; row < n; ++row) {
            if (row == column)
                continue;
            const T factor = a[row][column];
            if (factor == T())
                continue;
            for (int j = 0; j < n; ++j) {
                a[row][j] -= a[column][j] * factor;
                inverse[row][j] -= inverse[column][j] * factor;
            }
        }
    }
    return inverse;
}

template <class T> int brute_rank(std::vector<std::vector<T>> matrix) {
    const int h = static_cast<int>(matrix.size());
    if (h == 0)
        return 0;
    const int w = static_cast<int>(matrix[0].size());
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
            matrix[pivot].swap(matrix[rank]);
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

struct StaticFlipRankF2 {
    int row_size = 0;
    int column_size = 0;
    int matrix_rank = 0;

    std::vector<int> basis_rows;
    std::vector<int> basis_columns;
    std::vector<int> basis_row_position;
    std::vector<int> basis_column_position;
    std::vector<std::vector<F2>> intersection_inverse_matrix;
    std::vector<std::vector<F2>> column_space_reconstructor;
    std::vector<std::vector<F2>> row_space_reconstructor;
    std::vector<bool> standard_column_in_column_space;
    std::vector<bool> standard_row_in_row_space;

    explicit StaticFlipRankF2(const std::vector<std::vector<F2>> &matrix) {
        row_size = static_cast<int>(matrix.size());
        column_size = row_size == 0 ? 0 : static_cast<int>(matrix[0].size());
        basis_columns = find_independent_columns(matrix);
        basis_rows = find_independent_columns(transpose_matrix(matrix));
        matrix_rank = static_cast<int>(basis_columns.size());

        basis_row_position.assign(row_size, -1);
        basis_column_position.assign(column_size, -1);
        for (int i = 0; i < matrix_rank; ++i) {
            basis_row_position[basis_rows[i]] = i;
            basis_column_position[basis_columns[i]] = i;
        }

        std::vector<std::vector<F2>> intersection_matrix(
            matrix_rank, std::vector<F2>(matrix_rank, F2()));
        for (int i = 0; i < matrix_rank; ++i) {
            for (int j = 0; j < matrix_rank; ++j) {
                intersection_matrix[i][j] =
                    matrix[basis_rows[i]][basis_columns[j]];
            }
        }
        intersection_inverse_matrix = inverse_matrix(intersection_matrix);

        column_space_reconstructor.assign(row_size,
                                          std::vector<F2>(matrix_rank, F2()));
        for (int i = 0; i < row_size; ++i) {
            for (int mid = 0; mid < matrix_rank; ++mid) {
                const F2 value = matrix[i][basis_columns[mid]];
                if (value == F2())
                    continue;
                for (int j = 0; j < matrix_rank; ++j) {
                    column_space_reconstructor[i][j] +=
                        value * intersection_inverse_matrix[mid][j];
                }
            }
        }

        row_space_reconstructor.assign(matrix_rank,
                                       std::vector<F2>(column_size, F2()));
        for (int i = 0; i < matrix_rank; ++i) {
            for (int mid = 0; mid < matrix_rank; ++mid) {
                const F2 value = intersection_inverse_matrix[i][mid];
                if (value == F2())
                    continue;
                for (int j = 0; j < column_size; ++j) {
                    row_space_reconstructor[i][j] +=
                        value * matrix[basis_rows[mid]][j];
                }
            }
        }

        standard_column_in_column_space.assign(row_size, false);
        for (int row = 0; row < row_size; ++row) {
            const int pos = basis_row_position[row];
            if (pos < 0)
                continue;
            bool ok = true;
            for (int i = 0; i < row_size; ++i) {
                if (column_space_reconstructor[i][pos] !=
                    (i == row ? F2(1) : F2())) {
                    ok = false;
                    break;
                }
            }
            standard_column_in_column_space[row] = ok;
        }

        standard_row_in_row_space.assign(column_size, false);
        for (int column = 0; column < column_size; ++column) {
            const int pos = basis_column_position[column];
            if (pos < 0)
                continue;
            bool ok = true;
            for (int j = 0; j < column_size; ++j) {
                if (row_space_reconstructor[pos][j] !=
                    (j == column ? F2(1) : F2())) {
                    ok = false;
                    break;
                }
            }
            standard_row_in_row_space[column] = ok;
        }
    }

    char classify(int row, int column) const {
        const bool column_inside = standard_column_in_column_space[row];
        const bool row_inside = standard_row_in_row_space[column];
        if (!column_inside && !row_inside)
            return '+';
        if (column_inside != row_inside)
            return '0';
        const int row_pos = basis_row_position[row];
        const int column_pos = basis_column_position[column];
        assert(row_pos >= 0 && column_pos >= 0);
        const F2 s = F2(1) + intersection_inverse_matrix[column_pos][row_pos];
        return s == F2() ? '-' : '0';
    }
};

std::vector<std::vector<F2>>
apply_rank_one(std::vector<std::vector<F2>> matrix,
               const std::vector<F2> &column_vector,
               const std::vector<F2> &row_vector) {
    const int h = static_cast<int>(matrix.size());
    const int w = h == 0 ? 0 : static_cast<int>(matrix[0].size());
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j)
            matrix[i][j] += column_vector[i] * row_vector[j];
    }
    return matrix;
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
                assert(solver.materialize_matrix() == matrix);

                for (int column_mask = 0; column_mask < (1 << h);
                     ++column_mask) {
                    std::vector<F2> column_vector(h, F2());
                    for (int i = 0; i < h; ++i)
                        column_vector[i] = F2((column_mask >> i) & 1);
                    for (int row_mask = 0; row_mask < (1 << w); ++row_mask) {
                        std::vector<F2> row_vector(w, F2());
                        for (int j = 0; j < w; ++j)
                            row_vector[j] = F2((row_mask >> j) & 1);
                        const auto updated =
                            apply_rank_one(matrix, column_vector, row_vector);
                        const int expected = brute_rank(updated);
                        assert(solver.rank_after_rank_one_update(
                                   column_vector, row_vector) == expected);

                        auto applied = solver;
                        assert(applied.apply_rank_one_update(
                                   column_vector, row_vector) == expected);
                        assert(applied.rank() == expected);
                        assert(applied.materialize_matrix() == updated);
                        applied.build();
                        assert(applied.rank() == expected);
                        assert(applied.materialize_matrix() == updated);

                        for (int row = 0; row < h; ++row) {
                            for (int new_row_mask = 0; new_row_mask < (1 << w);
                                 ++new_row_mask) {
                                std::vector<F2> new_row(w, F2());
                                for (int j = 0; j < w; ++j)
                                    new_row[j] = F2((new_row_mask >> j) & 1);
                                auto row_updated = updated;
                                row_updated[row] = new_row;
                                const int row_expected =
                                    brute_rank(row_updated);
                                assert(applied.rank_after_row_replacement(
                                           row, new_row) == row_expected);
                                auto row_applied = applied;
                                assert(row_applied.apply_row_replacement(
                                           row, new_row) == row_expected);
                                assert(row_applied.rank() == row_expected);
                                assert(row_applied.materialize_matrix() ==
                                       row_updated);
                            }
                        }

                        for (int column = 0; column < w; ++column) {
                            for (int new_column_mask = 0;
                                 new_column_mask < (1 << h);
                                 ++new_column_mask) {
                                std::vector<F2> new_column(h, F2());
                                for (int i = 0; i < h; ++i)
                                    new_column[i] =
                                        F2((new_column_mask >> i) & 1);
                                auto column_updated = updated;
                                for (int i = 0; i < h; ++i)
                                    column_updated[i][column] = new_column[i];
                                const int column_expected =
                                    brute_rank(column_updated);
                                assert(applied.rank_after_column_replacement(
                                           column, new_column) ==
                                       column_expected);
                                auto column_applied = applied;
                                assert(column_applied.apply_column_replacement(
                                           column, new_column) ==
                                       column_expected);
                                assert(column_applied.rank() ==
                                       column_expected);
                                assert(column_applied.materialize_matrix() ==
                                       column_updated);
                            }
                        }
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

    StaticFlipRankF2 solver(matrix);
    for (int i = 0; i < n; ++i) {
        std::string answer(m, '0');
        for (int j = 0; j < m; ++j)
            answer[j] = solver.classify(i, j);
        std::cout << answer << '\n';
    }
}
