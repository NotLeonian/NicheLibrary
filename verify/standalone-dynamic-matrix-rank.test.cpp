// competitive-verifier: STANDALONE

#include <cassert>
#include <vector>

#include "../math/matrix/dynamic-matrix-rank.hpp"

namespace {
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

    [[maybe_unused]] friend F2 operator+(F2 lhs, const F2 &rhs) {
        return lhs += rhs;
    }

    [[maybe_unused]] friend F2 operator-(F2 lhs, const F2 &rhs) {
        return lhs -= rhs;
    }

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
    if (h == 0) {
        return 0;
    }
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
        if (pivot < 0) {
            continue;
        }
        if (pivot != rank) {
            matrix[pivot].swap(matrix[rank]);
        }
        const T inverse = T(1) / matrix[rank][column];
        for (int j = column; j < w; ++j) {
            matrix[rank][j] *= inverse;
        }
        for (int row = rank + 1; row < h; ++row) {
            const T factor = matrix[row][column];
            if (factor == T()) {
                continue;
            }
            for (int j = column; j < w; ++j) {
                matrix[row][j] -= matrix[rank][j] * factor;
            }
        }
        ++rank;
        if (rank == h) {
            break;
        }
    }
    return rank;
}

std::vector<std::vector<F2>> matrix_from_mask(int h, int w, int mask) {
    std::vector<std::vector<F2>> matrix(h, std::vector<F2>(w, F2()));
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            matrix[i][j] = F2((mask >> (i * w + j)) & 1);
        }
    }
    return matrix;
}

std::vector<F2> vector_from_mask(int n, int mask) {
    std::vector<F2> values(n, F2());
    for (int i = 0; i < n; ++i) {
        values[i] = F2((mask >> i) & 1);
    }
    return values;
}

std::vector<std::vector<F2>>
apply_rank_one(std::vector<std::vector<F2>> matrix,
               const std::vector<F2> &column_vector,
               const std::vector<F2> &row_vector) {
    const int h = static_cast<int>(matrix.size());
    const int w = h == 0 ? 0 : static_cast<int>(matrix[0].size());
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            matrix[i][j] += column_vector[i] * row_vector[j];
        }
    }
    return matrix;
}

void check_solver(const DynamicMatrixRank<F2> &solver,
                  const std::vector<std::vector<F2>> &matrix) {
    const int h = static_cast<int>(matrix.size());
    const int w = h == 0 ? 0 : static_cast<int>(matrix[0].size());
    assert(solver.rank() == brute_rank(matrix));
    assert(solver.materialize_matrix() == matrix);
    for (int row = 0; row < h; ++row) {
        assert(solver.get_row(row) == matrix[row]);
    }
    for (int column = 0; column < w; ++column) {
        std::vector<F2> expected(h, F2());
        for (int row = 0; row < h; ++row) {
            expected[row] = matrix[row][column];
        }
        assert(solver.get_column(column) == expected);
    }
}

void check_updates(const std::vector<std::vector<F2>> &matrix) {
    const int h = static_cast<int>(matrix.size());
    const int w = h == 0 ? 0 : static_cast<int>(matrix[0].size());
    DynamicMatrixRank<F2> solver(matrix);
    check_solver(solver, matrix);

    for (int column_mask = 0; column_mask < (1 << h); ++column_mask) {
        const std::vector<F2> column_vector = vector_from_mask(h, column_mask);
        for (int row_mask = 0; row_mask < (1 << w); ++row_mask) {
            const std::vector<F2> row_vector = vector_from_mask(w, row_mask);
            const auto updated =
                apply_rank_one(matrix, column_vector, row_vector);
            const int expected = brute_rank(updated);
            assert(solver.rank_after_rank_one_update(column_vector,
                                                     row_vector) == expected);

            auto applied = solver;
            assert(applied.apply_rank_one_update(column_vector, row_vector) ==
                   expected);
            check_solver(applied, updated);
            applied.build();
            check_solver(applied, updated);

            for (int row = 0; row < h; ++row) {
                for (int new_row_mask = 0; new_row_mask < (1 << w);
                     ++new_row_mask) {
                    const std::vector<F2> new_row =
                        vector_from_mask(w, new_row_mask);
                    auto row_updated = updated;
                    row_updated[row] = new_row;
                    const int row_expected = brute_rank(row_updated);
                    assert(applied.rank_after_row_replacement(row, new_row) ==
                           row_expected);
                    auto row_applied = applied;
                    assert(row_applied.apply_row_replacement(row, new_row) ==
                           row_expected);
                    check_solver(row_applied, row_updated);
                }
            }

            for (int column = 0; column < w; ++column) {
                for (int new_column_mask = 0; new_column_mask < (1 << h);
                     ++new_column_mask) {
                    const std::vector<F2> new_column =
                        vector_from_mask(h, new_column_mask);
                    auto column_updated = updated;
                    for (int row = 0; row < h; ++row) {
                        column_updated[row][column] = new_column[row];
                    }
                    const int column_expected = brute_rank(column_updated);
                    assert(applied.rank_after_column_replacement(
                               column, new_column) == column_expected);
                    auto column_applied = applied;
                    assert(column_applied.apply_column_replacement(
                               column, new_column) == column_expected);
                    check_solver(column_applied, column_updated);
                }
            }
        }
    }
}

void self_test() {
    {
        DynamicMatrixRank<F2> solver;
        solver.build(std::vector<std::vector<F2>>{});
        assert(solver.rank() == 0);
        assert(solver.materialize_matrix().empty());
    }

    for (int h = 1; h <= 3; ++h) {
        for (int w = 1; w <= 3; ++w) {
            const int states = 1 << (h * w);
            for (int mask = 0; mask < states; ++mask) {
                check_updates(matrix_from_mask(h, w, mask));
            }
        }
    }
}
} // namespace

int main() {
    self_test();

    return 0;
}
