// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <limits>
#include <tuple>
#include <utility>
#include <vector>

#include "../other/rectangle-add-max-get.hpp"

namespace {
struct TestRectangle {
    int l, d, r, u;
    long long w;
};

struct BruteResult {
    long long max_value;
    int minimum_x;
    int minimum_y;
    int maximum_x;
    int maximum_y;
    long long area;
};

BruteResult brute_force_rectangle(const std::vector<TestRectangle> &rectangles,
                                  int l, int d, int r, int u) {
    bool found = false;
    BruteResult ret{};
    for (int x = l; x < r; ++x) {
        for (int y = d; y < u; ++y) {
            long long value = 0;
            for (const auto &rect : rectangles) {
                if (rect.l <= x && x < rect.r && rect.d <= y && y < rect.u) {
                    value += rect.w;
                }
            }
            if (!found || ret.max_value < value) {
                ret = BruteResult{value, x, y, x, y, 1};
                found = true;
            } else if (ret.max_value == value) {
                ++ret.area;
                if (std::make_pair(x, y) <
                    std::make_pair(ret.minimum_x, ret.minimum_y)) {
                    ret.minimum_x = x;
                    ret.minimum_y = y;
                }
                if (std::make_pair(ret.maximum_x, ret.maximum_y) <
                    std::make_pair(x, y)) {
                    ret.maximum_x = x;
                    ret.maximum_y = y;
                }
            }
        }
    }
    assert(found);
    return ret;
}

template <class Lower, class Upper>
BruteResult brute_force_variable(const std::vector<TestRectangle> &rectangles,
                                 int l, int r, Lower lower_y, Upper upper_y) {
    bool found = false;
    BruteResult ret{};
    for (int x = l; x < r; ++x) {
        const int d = lower_y(x);
        const int u = upper_y(x);
        assert(d <= u);
        for (int y = d; y < u; ++y) {
            long long value = 0;
            for (const auto &rect : rectangles) {
                if (rect.l <= x && x < rect.r && rect.d <= y && y < rect.u) {
                    value += rect.w;
                }
            }
            if (!found || ret.max_value < value) {
                ret = BruteResult{value, x, y, x, y, 1};
                found = true;
            } else if (ret.max_value == value) {
                ++ret.area;
                if (std::make_pair(x, y) <
                    std::make_pair(ret.minimum_x, ret.minimum_y)) {
                    ret.minimum_x = x;
                    ret.minimum_y = y;
                }
                if (std::make_pair(ret.maximum_x, ret.maximum_y) <
                    std::make_pair(x, y)) {
                    ret.maximum_x = x;
                    ret.maximum_y = y;
                }
            }
        }
    }
    assert(found);
    return ret;
}

template <class Solver>
void add_all(Solver &solver, const std::vector<TestRectangle> &rectangles) {
    for (const auto &rect : rectangles) {
        solver.add_rectangle(rect.l, rect.d, rect.r, rect.u, rect.w);
    }
}

template <class Solver>
void check_rectangle_solver(const Solver &solver,
                            const std::vector<TestRectangle> &rectangles, int l,
                            int d, int r, int u) {
    const auto expected = brute_force_rectangle(rectangles, l, d, r, u);
    const auto [min_value, min_x, min_y] =
        solver.calc_max_lexicographically_minimum_point(l, d, r, u);
    const auto [max_value, max_x, max_y] =
        solver.calc_max_lexicographically_maximum_point(l, d, r, u);
    const auto [area_value, area] =
        solver.template calc_max_area<long long>(l, d, r, u);
    assert(min_value == expected.max_value);
    assert(min_x == expected.minimum_x);
    assert(min_y == expected.minimum_y);
    assert(max_value == expected.max_value);
    assert(max_x == expected.maximum_x);
    assert(max_y == expected.maximum_y);
    assert(area_value == expected.max_value);
    assert(area == expected.area);
}

void check_rectangle(const std::vector<TestRectangle> &rectangles, int l, int d,
                     int r, int u) {
    RectangleAddMaxGet<int, long long> solver(
        static_cast<int>(rectangles.size()));
    CompressedRectangleAddMaxGet<int, long long> compressed_solver(
        static_cast<int>(rectangles.size()));
    add_all(solver, rectangles);
    add_all(compressed_solver, rectangles);
    check_rectangle_solver(solver, rectangles, l, d, r, u);
    check_rectangle_solver(compressed_solver, rectangles, l, d, r, u);

    for (int x = l; x < r; ++x) {
        for (int y = d; y < u; ++y) {
            long long value = 0;
            for (const auto &rect : rectangles) {
                if (rect.l <= x && x < rect.r && rect.d <= y && y < rect.u) {
                    value += rect.w;
                }
            }
            const auto [point_value, point_x, point_y] =
                solver.calc_max_lexicographically_minimum_point(x, y, x + 1,
                                                                y + 1);
            const auto [point_max_value, point_max_x, point_max_y] =
                compressed_solver.calc_max_lexicographically_maximum_point(
                    x, y, x + 1, y + 1);
            const auto [point_area_value, point_area] =
                solver.calc_max_area<long long>(x, y, x + 1, y + 1);
            assert(point_value == value);
            assert(point_x == x);
            assert(point_y == y);
            assert(point_max_value == value);
            assert(point_max_x == x);
            assert(point_max_y == y);
            assert(point_area_value == value);
            assert(point_area == 1);
        }
    }
}

template <class Lower, class Upper>
void check_variable(const std::vector<TestRectangle> &rectangles, int l, int r,
                    Lower lower_y, Upper upper_y) {
    RectangleAddMaxGet<int, long long> solver(
        static_cast<int>(rectangles.size()));
    add_all(solver, rectangles);
    const auto expected =
        brute_force_variable(rectangles, l, r, lower_y, upper_y);
    const auto [min_value, min_x, min_y] =
        solver.calc_max_lexicographically_minimum_point(l, r, lower_y, upper_y);
    const auto [max_value, max_x, max_y] =
        solver.calc_max_lexicographically_maximum_point(l, r, lower_y, upper_y);
    const auto [area_value, area] =
        solver.calc_max_area<long long>(l, r, lower_y, upper_y);
    assert(min_value == expected.max_value);
    assert(min_x == expected.minimum_x);
    assert(min_y == expected.minimum_y);
    assert(max_value == expected.max_value);
    assert(max_x == expected.maximum_x);
    assert(max_y == expected.maximum_y);
    assert(area_value == expected.max_value);
    assert(area == expected.area);
}

void check_no_argument(const std::vector<TestRectangle> &rectangles) {
    bool found = false;
    int l = 0, d = 0, r = 0, u = 0;
    RectangleAddMaxGet<int, long long> solver(
        static_cast<int>(rectangles.size()));
    CompressedRectangleAddMaxGet<int, long long> compressed_solver(
        static_cast<int>(rectangles.size()));
    for (const auto &rect : rectangles) {
        solver.add_rectangle(rect.l, rect.d, rect.r, rect.u, rect.w);
        compressed_solver.add_rectangle(rect.l, rect.d, rect.r, rect.u, rect.w);
        if (rect.l == rect.r || rect.d == rect.u) {
            continue;
        }
        if (!found) {
            l = rect.l;
            d = rect.d;
            r = rect.r;
            u = rect.u;
            found = true;
        } else {
            l = std::min(l, rect.l);
            d = std::min(d, rect.d);
            r = std::max(r, rect.r);
            u = std::max(u, rect.u);
        }
    }
    if (!found) {
        return;
    }
    const auto expected = brute_force_rectangle(rectangles, l, d, r, u);
    const auto [min_value, min_x, min_y] =
        solver.calc_max_lexicographically_minimum_point();
    const auto [max_value, max_x, max_y] =
        compressed_solver.calc_max_lexicographically_maximum_point();
    const auto [area_value, area] = solver.calc_max_area<long long>();
    assert(min_value == expected.max_value);
    assert(min_x == expected.minimum_x);
    assert(min_y == expected.minimum_y);
    assert(max_value == expected.max_value);
    assert(max_x == expected.maximum_x);
    assert(max_y == expected.maximum_y);
    assert(area_value == expected.max_value);
    assert(area == expected.area);
}

void self_test() {
    const long long max_weight = std::numeric_limits<long long>::max();
    const std::vector<std::vector<TestRectangle>> cases = {
        {},
        {{0, 0, 2, 2, 1}},
        {{0, 0, 3, 3, -1}},
        {{0, 0, 3, 2, 2}, {1, 1, 4, 4, 3}},
        {{-1, -1, 2, 1, 5}, {0, -2, 3, 2, -2}, {1, 0, 2, 3, 4}},
        {{0, 0, 2, 2, 1}, {0, 0, 2, 2, 1}, {1, 1, 3, 3, -3}},
        {{0, 0, 1, 1, max_weight}, {1, 0, 2, 1, max_weight}},
        {{0, 0, 2, 2, 3},
         {0, 0, 0, 2, std::numeric_limits<long long>::lowest()},
         {1, 1, 2, 1, std::numeric_limits<long long>::lowest()}},
    };
    const std::vector<std::tuple<int, int, int, int>> queries = {
        {-2, -2, 4, 4}, {0, 0, 3, 3}, {1, 1, 4, 5},
        {-1, 0, 2, 2},  {2, 2, 5, 5}, {0, 0, 2, 1},
    };
    for (const auto &rectangles : cases) {
        for (const auto &[l, d, r, u] : queries) {
            check_rectangle(rectangles, l, d, r, u);
        }
        check_no_argument(rectangles);
    }

    for (const auto &rectangles : cases) {
        check_variable(
            rectangles, -2, 5, [](int x) { return x <= 0 ? -2 : x - 2; },
            [](int x) { return x <= 1 ? 2 : x + 1; });
        check_variable(
            rectangles, -1, 4, [](int x) { return x == 1 ? 0 : -1; },
            [](int x) { return x == 1 ? 0 : 3; });
    }

    {
        CompressedRectangleAddMaxGet<int, long long> solver;
        solver.add_rectangle(-1, -1, 1, 1, -5);
        const int l = std::numeric_limits<int>::lowest();
        const int d = std::numeric_limits<int>::lowest();
        const int r = std::numeric_limits<int>::max();
        const int u = std::numeric_limits<int>::max();
        const auto [min_value, min_x, min_y] =
            solver.calc_max_lexicographically_minimum_point(l, d, r, u);
        const auto [max_value, max_x, max_y] =
            solver.calc_max_lexicographically_maximum_point(l, d, r, u);
        assert(min_value == 0);
        assert(min_x == l);
        assert(min_y == d);
        assert(max_value == 0);
        assert(max_x == r - 1);
        assert(max_y == u - 1);
    }
    {
        RectangleAddMaxGet<int, long long> solver;
        CompressedRectangleAddMaxGet<int, long long> compressed_solver;
        solver.add_rectangle(0, 0, 0, 5,
                             std::numeric_limits<long long>::lowest());
        solver.add_rectangle(2, 2, 7, 2,
                             std::numeric_limits<long long>::lowest());
        compressed_solver.add_rectangle(
            0, 0, 0, 5, std::numeric_limits<long long>::lowest());
        assert(solver.rectangles.empty());
        assert(compressed_solver.rectangles.empty());

        const auto [value, x, y] =
            solver.calc_max_lexicographically_minimum_point();
        const auto [area_value, area] = solver.calc_max_area<long long>();
        assert(value == 0);
        assert(x == 0);
        assert(y == 0);
        assert(area_value == 0);
        assert(area == 0);
    }
}
} // namespace

int main() {
    self_test();

    return 0;
}
