#ifndef MATH_MATRIX_DYNAMIC_MATRIX_RANK_HPP
#define MATH_MATRIX_DYNAMIC_MATRIX_RANK_HPP

// 行列の階数を求め、外積 1 項更新後の階数を判定する。
// さらに、内部状態を O((k + 1)(r + c)) で更新しつつ変更後の階数を返せる。
// 現在の行列は左右の階数分解と片側逆元で保持する。
// 1 行差し替え、1 列差し替えはそれぞれ e_i (b-a_i)^T, (b-a_j) e_j^T に帰着する。
// 前処理は O(r + rc min(r, c) + k^2(r + c))、更新や判定は
// O((k + 1)(r + c)) である。
// T は体をなし、零判定と四則演算ができることを仮定する。

#include <cassert>
#include <vector>

template <class T> struct DynamicMatrixRank {
    int row_size = 0;
    int column_size = 0;
    int matrix_rank = 0;

    std::vector<std::vector<T>> column_space_basis;
    std::vector<std::vector<T>> row_space_basis;
    std::vector<std::vector<T>> column_space_left_inverse;
    std::vector<std::vector<T>> row_space_right_inverse;

    DynamicMatrixRank() = default;

    explicit DynamicMatrixRank(const std::vector<std::vector<T>> &matrix) {
        build(matrix);
    }

    void build(const std::vector<std::vector<T>> &matrix) {
        assert(is_rectangular(matrix));

        build_unchecked(matrix);
    }

  private:
    void build_unchecked(const std::vector<std::vector<T>> &matrix) {
        row_size = static_cast<int>(matrix.size());
        column_size = row_size == 0 ? 0 : static_cast<int>(matrix[0].size());
        const IndependentSubmatrix independent_submatrix =
            find_independent_submatrix(matrix);
        const std::vector<int> &basis_rows = independent_submatrix.rows;
        const std::vector<int> &basis_columns = independent_submatrix.columns;
        matrix_rank = static_cast<int>(basis_columns.size());

        std::vector<std::vector<T>> intersection_matrix(
            matrix_rank, std::vector<T>(matrix_rank, T()));
        for (int i = 0; i < matrix_rank; ++i) {
            for (int j = 0; j < matrix_rank; ++j) {
                intersection_matrix[i][j] =
                    matrix[basis_rows[i]][basis_columns[j]];
            }
        }
        const std::vector<std::vector<T>> intersection_inverse =
            inverse_matrix(intersection_matrix);

        column_space_basis.assign(row_size, std::vector<T>(matrix_rank, T()));
        for (int i = 0; i < row_size; ++i) {
            for (int j = 0; j < matrix_rank; ++j) {
                column_space_basis[i][j] = matrix[i][basis_columns[j]];
            }
        }

        row_space_basis.assign(matrix_rank, std::vector<T>(column_size, T()));
        for (int i = 0; i < matrix_rank; ++i) {
            for (int mid = 0; mid < matrix_rank; ++mid) {
                const T value = intersection_inverse[i][mid];
                if (value == T()) {
                    continue;
                }
                for (int j = 0; j < column_size; ++j) {
                    row_space_basis[i][j] += value * matrix[basis_rows[mid]][j];
                }
            }
        }

        column_space_left_inverse.assign(matrix_rank,
                                         std::vector<T>(row_size, T()));
        for (int i = 0; i < matrix_rank; ++i) {
            for (int j = 0; j < matrix_rank; ++j) {
                column_space_left_inverse[i][basis_rows[j]] =
                    intersection_inverse[i][j];
            }
        }

        row_space_right_inverse.assign(column_size,
                                       std::vector<T>(matrix_rank, T()));
        for (int i = 0; i < matrix_rank; ++i) {
            row_space_right_inverse[basis_columns[i]][i] = T(1);
        }
    }

  public:
    void build() { build_unchecked(materialize_matrix()); }

    int rank() const { return matrix_rank; }

    std::vector<T> get_row(int row_index) const {
        assert(0 <= row_index && row_index < row_size);
        std::vector<T> row(column_size, T());
        for (int i = 0; i < matrix_rank; ++i) {
            const T value = column_space_basis[row_index][i];
            if (value == T()) {
                continue;
            }
            for (int j = 0; j < column_size; ++j) {
                row[j] += value * row_space_basis[i][j];
            }
        }
        return row;
    }

    std::vector<T> get_column(int column_index) const {
        assert(0 <= column_index && column_index < column_size);
        std::vector<T> column(row_size, T());
        for (int j = 0; j < matrix_rank; ++j) {
            const T value = row_space_basis[j][column_index];
            if (value == T()) {
                continue;
            }
            for (int i = 0; i < row_size; ++i) {
                column[i] += column_space_basis[i][j] * value;
            }
        }
        return column;
    }

    std::vector<std::vector<T>> materialize_matrix() const {
        std::vector<std::vector<T>> matrix(row_size,
                                           std::vector<T>(column_size, T()));
        for (int i = 0; i < row_size; ++i) {
            for (int mid = 0; mid < matrix_rank; ++mid) {
                const T value = column_space_basis[i][mid];
                if (value == T()) {
                    continue;
                }
                for (int j = 0; j < column_size; ++j) {
                    matrix[i][j] += value * row_space_basis[mid][j];
                }
            }
        }
        return matrix;
    }

    int rank_after_rank_one_update(const std::vector<T> &column_vector,
                                   const std::vector<T> &row_vector) const {
        assert(static_cast<int>(column_vector.size()) == row_size);
        assert(static_cast<int>(row_vector.size()) == column_size);

        return rank_after_rank_one_update_unchecked(column_vector, row_vector);
    }

    int rank_after_row_replacement(int row_index,
                                   const std::vector<T> &new_row) const {
        assert(0 <= row_index && row_index < row_size);
        assert(static_cast<int>(new_row.size()) == column_size);
        const std::vector<T> difference =
            row_replacement_difference(row_index, new_row);
        std::vector<T> column_vector(row_size, T());
        column_vector[row_index] = T(1);
        return rank_after_rank_one_update_unchecked(column_vector, difference);
    }

    int rank_after_column_replacement(int column_index,
                                      const std::vector<T> &new_column) const {
        assert(0 <= column_index && column_index < column_size);
        assert(static_cast<int>(new_column.size()) == row_size);
        const std::vector<T> difference =
            column_replacement_difference(column_index, new_column);
        std::vector<T> row_vector(column_size, T());
        row_vector[column_index] = T(1);
        return rank_after_rank_one_update_unchecked(difference, row_vector);
    }

    int apply_rank_one_update(const std::vector<T> &column_vector,
                              const std::vector<T> &row_vector) {
        assert(static_cast<int>(column_vector.size()) == row_size);
        assert(static_cast<int>(row_vector.size()) == column_size);

        return apply_rank_one_update_unchecked(column_vector, row_vector);
    }

  private:
    int apply_rank_one_update_unchecked(const std::vector<T> &column_vector,
                                        const std::vector<T> &row_vector) {
        RankOneUpdateInfo info =
            analyze_rank_one_update(column_vector, row_vector);

        if (info.next_rank == matrix_rank + 1) {
            const int pivot_row = first_nonzero(info.column_residual);
            const int pivot_column = first_nonzero(info.row_residual);
            const std::vector<T> lambda = normalized_left_annihilator(
                pivot_row, info.column_residual[pivot_row]);
            const std::vector<T> rho = normalized_right_annihilator(
                pivot_column, info.row_residual[pivot_column]);

            append_column(column_space_basis, info.column_residual);
            column_space_left_inverse.push_back(lambda);

            row_space_basis.push_back(row_vector);
            for (int i = 0; i < matrix_rank; ++i) {
                if (info.alpha[i] == T()) {
                    continue;
                }
                for (int j = 0; j < column_size; ++j) {
                    row_space_basis[i][j] += info.alpha[i] * row_vector[j];
                }
            }

            const std::vector<T> right_alpha =
                multiply_row_space_right_inverse(info.alpha);

            for (int i = 0; i < column_size; ++i) {
                row_space_right_inverse[i].resize(matrix_rank + 1, T());
                if (rho[i] == T()) {
                    row_space_right_inverse[i][matrix_rank] =
                        T() - right_alpha[i];
                    continue;
                }
                for (int j = 0; j < matrix_rank; ++j) {
                    row_space_right_inverse[i][j] -= rho[i] * info.beta[j];
                }
                row_space_right_inverse[i][matrix_rank] =
                    T() - right_alpha[i] + rho[i] * info.schur;
            }
            ++matrix_rank;
            return matrix_rank;
        }

        if (!info.column_inside && info.row_inside) {
            const int pivot_row = first_nonzero(info.column_residual);
            const std::vector<T> lambda = normalized_left_annihilator(
                pivot_row, info.column_residual[pivot_row]);
            for (int i = 0; i < row_size; ++i) {
                if (column_vector[i] == T()) {
                    continue;
                }
                for (int j = 0; j < matrix_rank; ++j) {
                    column_space_basis[i][j] += column_vector[i] * info.beta[j];
                }
            }
            for (int i = 0; i < matrix_rank; ++i) {
                if (info.alpha[i] == T()) {
                    continue;
                }
                for (int j = 0; j < row_size; ++j) {
                    column_space_left_inverse[i][j] -=
                        info.alpha[i] * lambda[j];
                }
            }
            return matrix_rank;
        }

        if (info.column_inside && !info.row_inside) {
            const int pivot_column = first_nonzero(info.row_residual);
            const std::vector<T> rho = normalized_right_annihilator(
                pivot_column, info.row_residual[pivot_column]);
            for (int i = 0; i < matrix_rank; ++i) {
                if (info.alpha[i] == T()) {
                    continue;
                }
                for (int j = 0; j < column_size; ++j) {
                    row_space_basis[i][j] += info.alpha[i] * row_vector[j];
                }
            }
            for (int i = 0; i < column_size; ++i) {
                if (rho[i] == T()) {
                    continue;
                }
                for (int j = 0; j < matrix_rank; ++j) {
                    row_space_right_inverse[i][j] -= rho[i] * info.beta[j];
                }
            }
            return matrix_rank;
        }

        if (info.schur != T()) {
            const T schur_inverse = T(1) / info.schur;
            for (T &value : info.beta) {
                value *= schur_inverse;
            }
            const std::vector<T> right_alpha =
                multiply_row_space_right_inverse(info.alpha);
            for (int i = 0; i < matrix_rank; ++i) {
                if (info.alpha[i] == T()) {
                    continue;
                }
                for (int j = 0; j < column_size; ++j) {
                    row_space_basis[i][j] += info.alpha[i] * row_vector[j];
                }
            }
            for (int i = 0; i < column_size; ++i) {
                if (right_alpha[i] == T()) {
                    continue;
                }
                for (int j = 0; j < matrix_rank; ++j) {
                    row_space_right_inverse[i][j] -=
                        right_alpha[i] * info.beta[j];
                }
            }
            return matrix_rank;
        }

        const int removed = first_nonzero(info.alpha);
        const T removed_inverse = T(1) / info.alpha[removed];

        const std::vector<T> right_alpha =
            multiply_row_space_right_inverse(info.alpha);

        std::vector<std::vector<T>> new_column_space_basis(
            row_size, std::vector<T>(matrix_rank - 1, T()));
        std::vector<std::vector<T>> new_row_space_basis(
            matrix_rank - 1, std::vector<T>(column_size, T()));
        std::vector<std::vector<T>> new_column_space_left_inverse(
            matrix_rank - 1, std::vector<T>(row_size, T()));
        std::vector<std::vector<T>> new_row_space_right_inverse(
            column_size, std::vector<T>(matrix_rank - 1, T()));

        int new_index = 0;
        for (int old = 0; old < matrix_rank; ++old) {
            if (old == removed) {
                continue;
            }
            for (int i = 0; i < row_size; ++i) {
                new_column_space_basis[i][new_index] =
                    column_space_basis[i][old] +
                    column_vector[i] * info.beta[old];
            }
            const T factor = info.alpha[old] * removed_inverse;
            for (int j = 0; j < column_size; ++j) {
                new_row_space_basis[new_index][j] =
                    row_space_basis[old][j] -
                    factor * row_space_basis[removed][j];
            }
            for (int j = 0; j < row_size; ++j) {
                new_column_space_left_inverse[new_index][j] =
                    column_space_left_inverse[old][j] -
                    factor * column_space_left_inverse[removed][j];
            }
            for (int i = 0; i < column_size; ++i) {
                new_row_space_right_inverse[i][new_index] =
                    row_space_right_inverse[i][old] +
                    right_alpha[i] * info.beta[old];
            }
            ++new_index;
        }

        column_space_basis.swap(new_column_space_basis);
        row_space_basis.swap(new_row_space_basis);
        column_space_left_inverse.swap(new_column_space_left_inverse);
        row_space_right_inverse.swap(new_row_space_right_inverse);
        --matrix_rank;
        return matrix_rank;
    }

  public:
    int apply_row_replacement(int row_index, const std::vector<T> &new_row) {
        assert(0 <= row_index && row_index < row_size);
        assert(static_cast<int>(new_row.size()) == column_size);
        const std::vector<T> difference =
            row_replacement_difference(row_index, new_row);
        std::vector<T> column_vector(row_size, T());
        column_vector[row_index] = T(1);
        return apply_rank_one_update_unchecked(column_vector, difference);
    }

    int apply_column_replacement(int column_index,
                                 const std::vector<T> &new_column) {
        assert(0 <= column_index && column_index < column_size);
        assert(static_cast<int>(new_column.size()) == row_size);
        const std::vector<T> difference =
            column_replacement_difference(column_index, new_column);
        std::vector<T> row_vector(column_size, T());
        row_vector[column_index] = T(1);
        return apply_rank_one_update_unchecked(difference, row_vector);
    }

  private:
    struct IndependentSubmatrix {
        std::vector<int> rows;
        std::vector<int> columns;
    };

    struct RankOneUpdateInfo {
        std::vector<T> alpha;
        std::vector<T> beta;
        std::vector<T> column_residual;
        std::vector<T> row_residual;
        bool column_inside = false;
        bool row_inside = false;
        T schur = T();
        int next_rank = 0;
    };

    static bool is_rectangular(const std::vector<std::vector<T>> &matrix) {
        if (matrix.empty()) {
            return true;
        }
        const int column_count = static_cast<int>(matrix[0].size());
        for (const std::vector<T> &row : matrix) {
            if (static_cast<int>(row.size()) != column_count) {
                return false;
            }
        }
        return true;
    }

    RankOneUpdateInfo
    analyze_rank_one_update(const std::vector<T> &column_vector,
                            const std::vector<T> &row_vector) const {
        RankOneUpdateInfo info;
        info.alpha = multiply_left_inverse(column_vector);
        info.beta = multiply_right_inverse(row_vector);

        info.column_residual = column_vector;
        for (int j = 0; j < matrix_rank; ++j) {
            const T value = info.alpha[j];
            if (value == T()) {
                continue;
            }
            for (int i = 0; i < row_size; ++i) {
                info.column_residual[i] -= column_space_basis[i][j] * value;
            }
        }
        info.row_residual = row_vector;
        for (int i = 0; i < matrix_rank; ++i) {
            const T value = info.beta[i];
            if (value == T()) {
                continue;
            }
            for (int j = 0; j < column_size; ++j) {
                info.row_residual[j] -= value * row_space_basis[i][j];
            }
        }

        info.column_inside = true;
        info.row_inside = true;
        for (int i = 0; i < row_size; ++i) {
            if (info.column_residual[i] != T()) {
                info.column_inside = false;
                break;
            }
        }
        for (int j = 0; j < column_size; ++j) {
            if (info.row_residual[j] != T()) {
                info.row_inside = false;
                break;
            }
        }

        if (info.column_inside != info.row_inside) {
            info.next_rank = matrix_rank;
            return info;
        }

        info.schur = T(1);
        for (int i = 0; i < matrix_rank; ++i) {
            info.schur += info.beta[i] * info.alpha[i];
        }

        if (!info.column_inside && !info.row_inside) {
            info.next_rank = matrix_rank + 1;
        } else {
            info.next_rank = info.schur == T() ? matrix_rank - 1 : matrix_rank;
        }
        return info;
    }

    int rank_after_rank_one_update_unchecked(
        const std::vector<T> &column_vector,
        const std::vector<T> &row_vector) const {
        const std::vector<T> alpha = multiply_left_inverse(column_vector);
        const std::vector<T> beta = multiply_right_inverse(row_vector);

        bool column_inside = true;
        for (int i = 0; i < row_size; ++i) {
            T residual = column_vector[i];
            for (int j = 0; j < matrix_rank; ++j) {
                residual -= column_space_basis[i][j] * alpha[j];
            }
            if (residual != T()) {
                column_inside = false;
                break;
            }
        }

        bool row_inside = true;
        for (int j = 0; j < column_size; ++j) {
            T residual = row_vector[j];
            for (int i = 0; i < matrix_rank; ++i) {
                residual -= beta[i] * row_space_basis[i][j];
            }
            if (residual != T()) {
                row_inside = false;
                break;
            }
        }

        if (!column_inside && !row_inside) {
            return matrix_rank + 1;
        }
        if (column_inside != row_inside) {
            return matrix_rank;
        }

        T schur = T(1);
        for (int i = 0; i < matrix_rank; ++i) {
            schur += beta[i] * alpha[i];
        }
        return schur == T() ? matrix_rank - 1 : matrix_rank;
    }

    std::vector<T>
    row_replacement_difference(int row_index,
                               const std::vector<T> &new_row) const {
        std::vector<T> difference = new_row;
        for (int i = 0; i < matrix_rank; ++i) {
            const T value = column_space_basis[row_index][i];
            if (value == T()) {
                continue;
            }
            for (int j = 0; j < column_size; ++j) {
                difference[j] -= value * row_space_basis[i][j];
            }
        }
        return difference;
    }

    std::vector<T>
    column_replacement_difference(int column_index,
                                  const std::vector<T> &new_column) const {
        std::vector<T> difference = new_column;
        for (int j = 0; j < matrix_rank; ++j) {
            const T value = row_space_basis[j][column_index];
            if (value == T()) {
                continue;
            }
            for (int i = 0; i < row_size; ++i) {
                difference[i] -= column_space_basis[i][j] * value;
            }
        }
        return difference;
    }

    std::vector<T>
    multiply_left_inverse(const std::vector<T> &column_vector) const {
        std::vector<T> result(matrix_rank, T());
        for (int j = 0; j < row_size; ++j) {
            const T value = column_vector[j];
            if (value == T()) {
                continue;
            }
            for (int i = 0; i < matrix_rank; ++i) {
                result[i] += column_space_left_inverse[i][j] * value;
            }
        }
        return result;
    }

    std::vector<T>
    multiply_right_inverse(const std::vector<T> &row_vector) const {
        std::vector<T> result(matrix_rank, T());
        for (int j = 0; j < column_size; ++j) {
            const T value = row_vector[j];
            if (value == T()) {
                continue;
            }
            for (int i = 0; i < matrix_rank; ++i) {
                result[i] += value * row_space_right_inverse[j][i];
            }
        }
        return result;
    }

    std::vector<T>
    multiply_row_space_right_inverse(const std::vector<T> &coefficients) const {
        std::vector<T> result(column_size, T());
        for (int j = 0; j < matrix_rank; ++j) {
            const T value = coefficients[j];
            if (value == T()) {
                continue;
            }
            for (int i = 0; i < column_size; ++i) {
                result[i] += row_space_right_inverse[i][j] * value;
            }
        }
        return result;
    }

    std::vector<T> normalized_left_annihilator(int pivot_row,
                                               const T &pivot_value) const {
        std::vector<T> result(row_size, T());
        result[pivot_row] = T(1);
        for (int j = 0; j < matrix_rank; ++j) {
            const T value = column_space_basis[pivot_row][j];
            if (value == T()) {
                continue;
            }
            for (int i = 0; i < row_size; ++i) {
                result[i] -= value * column_space_left_inverse[j][i];
            }
        }
        const T pivot_inverse = T(1) / pivot_value;
        for (int i = 0; i < row_size; ++i) {
            result[i] *= pivot_inverse;
        }
        return result;
    }

    std::vector<T> normalized_right_annihilator(int pivot_column,
                                                const T &pivot_value) const {
        std::vector<T> result(column_size, T());
        result[pivot_column] = T(1);
        for (int j = 0; j < matrix_rank; ++j) {
            const T value = row_space_basis[j][pivot_column];
            if (value == T()) {
                continue;
            }
            for (int i = 0; i < column_size; ++i) {
                result[i] -= row_space_right_inverse[i][j] * value;
            }
        }
        const T pivot_inverse = T(1) / pivot_value;
        for (int i = 0; i < column_size; ++i) {
            result[i] *= pivot_inverse;
        }
        return result;
    }

    static void append_column(std::vector<std::vector<T>> &matrix,
                              const std::vector<T> &column) {
        for (int i = 0; i < static_cast<int>(matrix.size()); ++i) {
            matrix[i].push_back(column[i]);
        }
    }

    static int first_nonzero(const std::vector<T> &vector) {
        for (int i = 0; i < static_cast<int>(vector.size()); ++i) {
            if (vector[i] != T()) {
                return i;
            }
        }
        return -1;
    }

    static IndependentSubmatrix
    find_independent_submatrix(const std::vector<std::vector<T>> &matrix) {
        const int h = static_cast<int>(matrix.size());
        if (h == 0) {
            return {};
        }
        const int w = static_cast<int>(matrix[0].size());
        std::vector<std::vector<T>> b = matrix;
        std::vector<int> original_rows(h);
        for (int i = 0; i < h; ++i) {
            original_rows[i] = i;
        }

        int rank = 0;
        IndependentSubmatrix result;
        const int maximum_rank = h < w ? h : w;
        result.rows.reserve(maximum_rank);
        result.columns.reserve(maximum_rank);
        for (int column = 0; column < w; ++column) {
            int pivot = -1;
            for (int row = rank; row < h; ++row) {
                if (b[row][column] != T()) {
                    pivot = row;
                    break;
                }
            }
            if (pivot < 0) {
                continue;
            }
            if (pivot != rank) {
                b[pivot].swap(b[rank]);
                const int original_row = original_rows[pivot];
                original_rows[pivot] = original_rows[rank];
                original_rows[rank] = original_row;
            }

            const T inverse = T(1) / b[rank][column];
            b[rank][column] = T(1);
            for (int j = column + 1; j < w; ++j) {
                b[rank][j] *= inverse;
            }
            for (int row = rank + 1; row < h; ++row) {
                const T factor = b[row][column];
                if (factor == T()) {
                    continue;
                }
                b[row][column] = T();
                for (int j = column + 1; j < w; ++j) {
                    b[row][j] -= b[rank][j] * factor;
                }
            }
            result.rows.push_back(original_rows[rank]);
            result.columns.push_back(column);
            ++rank;
            if (rank == h) {
                break;
            }
        }
        return result;
    }

    static std::vector<std::vector<T>>
    inverse_matrix(std::vector<std::vector<T>> matrix) {
        const int n = static_cast<int>(matrix.size());
        std::vector<std::vector<T>> inverse(n, std::vector<T>(n, T()));
        for (int i = 0; i < n; ++i) {
            inverse[i][i] = T(1);
        }

        for (int column = 0; column < n; ++column) {
            int pivot = -1;
            for (int row = column; row < n; ++row) {
                if (matrix[row][column] != T()) {
                    pivot = row;
                    break;
                }
            }
            if (pivot != column) {
                matrix[pivot].swap(matrix[column]);
                inverse[pivot].swap(inverse[column]);
            }

            const T diagonal_inverse = T(1) / matrix[column][column];
            matrix[column][column] = T(1);
            for (int j = column + 1; j < n; ++j) {
                matrix[column][j] *= diagonal_inverse;
            }
            for (int j = 0; j < n; ++j) {
                inverse[column][j] *= diagonal_inverse;
            }
            for (int row = 0; row < n; ++row) {
                if (row == column) {
                    continue;
                }
                const T factor = matrix[row][column];
                if (factor == T()) {
                    continue;
                }
                matrix[row][column] = T();
                for (int j = column + 1; j < n; ++j) {
                    matrix[row][j] -= matrix[column][j] * factor;
                }
                for (int j = 0; j < n; ++j) {
                    inverse[row][j] -= inverse[column][j] * factor;
                }
            }
        }
        return inverse;
    }
};

#endif
