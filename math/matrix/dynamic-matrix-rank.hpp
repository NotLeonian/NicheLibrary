#ifndef MATH_MATRIX_DYNAMIC_MATRIX_RANK_HPP
#define MATH_MATRIX_DYNAMIC_MATRIX_RANK_HPP

// 行列の階数と、1 列ベクトルと 1 行ベクトルの外積を加えた後の階数を求める。
// 1 行差し替え・1 列差し替えはそれぞれ e_i (b-a_i)^T, (b-a_j) e_j^T に帰着する。
// 体上の非正方行列を含む一般の r×c 行列に対応する。
// 前処理 O(rc min(r, c) + k^2(r + c))、外積 1 項更新は O(k(r + c))。
// 行差し替えは O(kc)、列差し替えは O(kr)。ここで k = rank(A) である。
// T は零判定と四則演算ができることを仮定し、添字やサイズ不一致は assert する。

#include <cassert>
#include <utility>
#include <vector>

template <class T> struct DynamicMatrixRank {
    std::vector<std::vector<T>> matrix;
    int row_size = 0;
    int column_size = 0;
    int matrix_rank = 0;

    std::vector<int> basis_rows;
    std::vector<int> basis_columns;
    std::vector<int> basis_row_position;
    std::vector<int> basis_column_position;

    std::vector<std::vector<T>> intersection_inverse_matrix;
    std::vector<std::vector<T>> column_space_reconstructor;
    std::vector<std::vector<T>> row_space_reconstructor;

    std::vector<bool> standard_column_in_column_space;
    std::vector<bool> standard_row_in_row_space;

    explicit DynamicMatrixRank(const std::vector<std::vector<T>> &matrix_)
        : matrix(matrix_) {
        build();
    }

    void build() {
        row_size = static_cast<int>(matrix.size());
        column_size = row_size == 0 ? 0 : static_cast<int>(matrix[0].size());
        for (int i = 1; i < row_size; ++i)
            assert(static_cast<int>(matrix[i].size()) == column_size);

        basis_columns = find_independent_columns(matrix);
        basis_rows = find_independent_columns(transpose_matrix(matrix));
        matrix_rank = static_cast<int>(basis_columns.size());
        assert(static_cast<int>(basis_rows.size()) == matrix_rank);

        basis_row_position.assign(row_size, -1);
        basis_column_position.assign(column_size, -1);
        for (int i = 0; i < matrix_rank; ++i) {
            basis_row_position[basis_rows[i]] = i;
            basis_column_position[basis_columns[i]] = i;
        }

        std::vector<std::vector<T>> basis_minor(
            matrix_rank, std::vector<T>(matrix_rank, T()));
        for (int i = 0; i < matrix_rank; ++i) {
            for (int j = 0; j < matrix_rank; ++j) {
                basis_minor[i][j] = matrix[basis_rows[i]][basis_columns[j]];
            }
        }
        intersection_inverse_matrix = inverse_matrix(basis_minor);

        column_space_reconstructor.assign(row_size,
                                          std::vector<T>(matrix_rank, T()));
        for (int i = 0; i < row_size; ++i) {
            for (int mid = 0; mid < matrix_rank; ++mid) {
                const T value = matrix[i][basis_columns[mid]];
                if (value == T())
                    continue;
                for (int j = 0; j < matrix_rank; ++j) {
                    column_space_reconstructor[i][j] +=
                        value * intersection_inverse_matrix[mid][j];
                }
            }
        }

        row_space_reconstructor.assign(matrix_rank,
                                       std::vector<T>(column_size, T()));
        for (int i = 0; i < matrix_rank; ++i) {
            for (int mid = 0; mid < matrix_rank; ++mid) {
                const T value = intersection_inverse_matrix[i][mid];
                if (value == T())
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
                    (i == row ? T(1) : T())) {
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
                    (j == column ? T(1) : T())) {
                    ok = false;
                    break;
                }
            }
            standard_row_in_row_space[column] = ok;
        }
    }

    int rank() const { return matrix_rank; }

    int rank_after_rank_one_update(const std::vector<T> &column_vector,
                                   const std::vector<T> &row_vector) const {
        assert(static_cast<int>(column_vector.size()) == row_size);
        assert(static_cast<int>(row_vector.size()) == column_size);

        const std::vector<T> basis_row_values =
            restrict_to_basis_rows(column_vector);
        const std::vector<T> basis_column_values =
            restrict_to_basis_columns(row_vector);

        const bool column_inside =
            reconstruct_column_vector(basis_row_values) == column_vector;
        const bool row_inside =
            reconstruct_row_vector(basis_column_values) == row_vector;

        if (!column_inside && !row_inside)
            return matrix_rank + 1;
        if (column_inside != row_inside)
            return matrix_rank;

        const std::vector<T> alpha =
            multiply_inverse_from_left(basis_row_values);
        T s = T(1);
        for (int i = 0; i < matrix_rank; ++i)
            s += basis_column_values[i] * alpha[i];
        return s == T() ? matrix_rank - 1 : matrix_rank;
    }

    int rank_after_row_replacement(int row_index,
                                   const std::vector<T> &new_row) const {
        assert(0 <= row_index && row_index < row_size);
        assert(static_cast<int>(new_row.size()) == column_size);

        std::vector<T> difference(column_size, T());
        for (int j = 0; j < column_size; ++j)
            difference[j] = new_row[j] - matrix[row_index][j];

        const std::vector<T> basis_column_values =
            restrict_to_basis_columns(difference);
        const bool row_inside =
            reconstruct_row_vector(basis_column_values) == difference;
        const bool column_inside = standard_column_in_column_space[row_index];

        if (!column_inside && !row_inside)
            return matrix_rank + 1;
        if (column_inside != row_inside)
            return matrix_rank;

        const int pos = basis_row_position[row_index];
        assert(pos >= 0);
        T s = T(1);
        for (int i = 0; i < matrix_rank; ++i)
            s += basis_column_values[i] * intersection_inverse_matrix[i][pos];
        return s == T() ? matrix_rank - 1 : matrix_rank;
    }

    int rank_after_column_replacement(int column_index,
                                      const std::vector<T> &new_column) const {
        assert(0 <= column_index && column_index < column_size);
        assert(static_cast<int>(new_column.size()) == row_size);

        std::vector<T> difference(row_size, T());
        for (int i = 0; i < row_size; ++i)
            difference[i] = new_column[i] - matrix[i][column_index];

        const std::vector<T> basis_row_values =
            restrict_to_basis_rows(difference);
        const bool column_inside =
            reconstruct_column_vector(basis_row_values) == difference;
        const bool row_inside = standard_row_in_row_space[column_index];

        if (!column_inside && !row_inside)
            return matrix_rank + 1;
        if (column_inside != row_inside)
            return matrix_rank;

        const int pos = basis_column_position[column_index];
        assert(pos >= 0);
        T s = T(1);
        for (int i = 0; i < matrix_rank; ++i)
            s += intersection_inverse_matrix[pos][i] * basis_row_values[i];
        return s == T() ? matrix_rank - 1 : matrix_rank;
    }

  private:
    static std::vector<std::vector<T>>
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

    static std::vector<int>
    find_independent_columns(const std::vector<std::vector<T>> &a) {
        const int h = static_cast<int>(a.size());
        if (h == 0)
            return {};
        const int w = static_cast<int>(a[0].size());
        std::vector<std::vector<T>> b = a;
        int rank = 0;
        std::vector<int> pivot_columns;
        for (int column = 0; column < w; ++column) {
            int pivot = -1;
            for (int row = rank; row < h; ++row) {
                if (b[row][column] != T()) {
                    pivot = row;
                    break;
                }
            }
            if (pivot < 0)
                continue;
            if (pivot != rank)
                std::swap(b[pivot], b[rank]);

            const T inverse = T(1) / b[rank][column];
            for (int j = column; j < w; ++j)
                b[rank][j] *= inverse;
            for (int row = rank + 1; row < h; ++row) {
                const T factor = b[row][column];
                if (factor == T())
                    continue;
                for (int j = column; j < w; ++j)
                    b[row][j] -= b[rank][j] * factor;
            }
            pivot_columns.push_back(column);
            ++rank;
            if (rank == h)
                break;
        }
        return pivot_columns;
    }

    static std::vector<std::vector<T>>
    inverse_matrix(std::vector<std::vector<T>> a) {
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
                std::swap(a[pivot], a[column]);
                std::swap(inverse[pivot], inverse[column]);
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

    std::vector<T>
    restrict_to_basis_rows(const std::vector<T> &column_vector) const {
        std::vector<T> values(matrix_rank, T());
        for (int i = 0; i < matrix_rank; ++i)
            values[i] = column_vector[basis_rows[i]];
        return values;
    }

    std::vector<T>
    restrict_to_basis_columns(const std::vector<T> &row_vector) const {
        std::vector<T> values(matrix_rank, T());
        for (int i = 0; i < matrix_rank; ++i)
            values[i] = row_vector[basis_columns[i]];
        return values;
    }

    std::vector<T>
    reconstruct_column_vector(const std::vector<T> &basis_row_values) const {
        assert(static_cast<int>(basis_row_values.size()) == matrix_rank);
        std::vector<T> reconstructed(row_size, T());
        for (int i = 0; i < row_size; ++i) {
            for (int j = 0; j < matrix_rank; ++j) {
                reconstructed[i] +=
                    column_space_reconstructor[i][j] * basis_row_values[j];
            }
        }
        return reconstructed;
    }

    std::vector<T>
    reconstruct_row_vector(const std::vector<T> &basis_column_values) const {
        assert(static_cast<int>(basis_column_values.size()) == matrix_rank);
        std::vector<T> reconstructed(column_size, T());
        for (int i = 0; i < matrix_rank; ++i) {
            const T value = basis_column_values[i];
            if (value == T())
                continue;
            for (int j = 0; j < column_size; ++j) {
                reconstructed[j] += value * row_space_reconstructor[i][j];
            }
        }
        return reconstructed;
    }

    std::vector<T>
    multiply_inverse_from_left(const std::vector<T> &basis_row_values) const {
        assert(static_cast<int>(basis_row_values.size()) == matrix_rank);
        std::vector<T> product(matrix_rank, T());
        for (int i = 0; i < matrix_rank; ++i) {
            for (int j = 0; j < matrix_rank; ++j) {
                product[i] +=
                    intersection_inverse_matrix[i][j] * basis_row_values[j];
            }
        }
        return product;
    }
};

#endif
