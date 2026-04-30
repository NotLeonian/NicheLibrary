// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/area_of_union_of_rectangles

#include <algorithm>
#include <cassert>
#include <iostream>
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

BruteResult brute_force(const std::vector<TestRectangle> &rectangles, int l,
                        int d, int r, int u) {
    bool found = false;
    BruteResult ret{};
    for (int x = l; x < r; ++x) {
        for (int y = d; y < u; ++y) {
            long long value = 0;
            for (const auto &rect : rectangles) {
                if (rect.l <= x && x < rect.r && rect.d <= y && y < rect.u)
                    value += rect.w;
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

void check(const std::vector<TestRectangle> &rectangles, int l, int d, int r,
           int u) {
    RectangleAddMaxGet<int, long long> solver(
        static_cast<int>(rectangles.size()));
    for (const auto &rect : rectangles)
        solver.add_rectangle(rect.l, rect.d, rect.r, rect.u, rect.w);

    const auto expected = brute_force(rectangles, l, d, r, u);
    const auto [min_value, min_x, min_y] =
        solver.calc_max_lexicographically_minimum_point(l, d, r, u);
    const auto [max_value, max_x, max_y] =
        solver.calc_max_lexicographically_maximum_point(l, d, r, u);
    const auto [area_value, area] = solver.calc_max_area<long long>(l, d, r, u);
    assert(min_value == expected.max_value);
    assert(min_x == expected.minimum_x);
    assert(min_y == expected.minimum_y);
    assert(max_value == expected.max_value);
    assert(max_x == expected.maximum_x);
    assert(max_y == expected.maximum_y);
    assert(area_value == expected.max_value);
    assert(area == expected.area);

    for (int x = l; x < r; ++x) {
        for (int y = d; y < u; ++y) {
            long long value = 0;
            for (const auto &rect : rectangles) {
                if (rect.l <= x && x < rect.r && rect.d <= y && y < rect.u)
                    value += rect.w;
            }
            const auto [point_value, point_x, point_y] =
                solver.calc_max_lexicographically_minimum_point(x, y, x + 1,
                                                                y + 1);
            const auto [point_max_value, point_max_x, point_max_y] =
                solver.calc_max_lexicographically_maximum_point(x, y, x + 1,
                                                                y + 1);
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

void self_test() {
    const std::vector<std::vector<TestRectangle>> cases = {
        {},
        {{0, 0, 2, 2, 1}},
        {{0, 0, 3, 3, -1}},
        {{0, 0, 3, 2, 2}, {1, 1, 4, 4, 3}},
        {{-1, -1, 2, 1, 5}, {0, -2, 3, 2, -2}, {1, 0, 2, 3, 4}},
        {{0, 0, 2, 2, 1}, {0, 0, 2, 2, 1}, {1, 1, 3, 3, -3}},
    };
    const std::vector<std::tuple<int, int, int, int>> queries = {
        {-2, -2, 4, 4}, {0, 0, 3, 3}, {1, 1, 4, 5}, {-1, 0, 2, 2}, {2, 2, 5, 5},
    };
    for (const auto &rectangles : cases) {
        for (const auto &[l, d, r, u] : queries)
            check(rectangles, l, d, r, u);
    }
    for (const auto &rectangles : cases) {
        if (rectangles.empty())
            continue;
        int l = rectangles[0].l;
        int d = rectangles[0].d;
        int r = rectangles[0].r;
        int u = rectangles[0].u;
        RectangleAddMaxGet<int, long long> solver(
            static_cast<int>(rectangles.size()));
        for (const auto &rect : rectangles) {
            solver.add_rectangle(rect.l, rect.d, rect.r, rect.u, rect.w);
            l = std::min(l, rect.l);
            d = std::min(d, rect.d);
            r = std::max(r, rect.r);
            u = std::max(u, rect.u);
        }
        const auto expected = brute_force(rectangles, l, d, r, u);
        const auto [min_value, min_x, min_y] =
            solver.calc_max_lexicographically_minimum_point();
        const auto [max_value, max_x, max_y] =
            solver.calc_max_lexicographically_maximum_point();
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
    {
        RectangleAddMaxGet<int, long long> solver;
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

        const auto [bbox_value, bbox_x, bbox_y] =
            solver.calc_max_lexicographically_minimum_point();
        assert(bbox_value == -5);
        assert(bbox_x == -1);
        assert(bbox_y == -1);
    }
    RectangleAddMaxGet<int, long long> empty_solver;
    const auto [value, x, y] =
        empty_solver.calc_max_lexicographically_minimum_point();
    const auto [area_value, area] = empty_solver.calc_max_area<long long>();
    assert(value == 0);
    assert(x == 0);
    assert(y == 0);
    assert(area_value == 0);
    assert(area == 0);
}
} // namespace

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

#ifndef ONLINE_JUDGE
    self_test();
#endif

    int n;
    std::cin >> n;
    RectangleAddMaxGet<long long, long long> solver(n);
    long long min_l = 0, min_d = 0, max_r = 0, max_u = 0;
    for (int i = 0; i < n; ++i) {
        long long l, d, r, u;
        std::cin >> l >> d >> r >> u;
        solver.add_rectangle(l, d, r, u, -1);
        if (i == 0) {
            min_l = l;
            min_d = d;
            max_r = r;
            max_u = u;
        } else {
            min_l = std::min(min_l, l);
            min_d = std::min(min_d, d);
            max_r = std::max(max_r, r);
            max_u = std::max(max_u, u);
        }
    }
    if (n == 0) {
        std::cout << 0 << '\n';
        return 0;
    }
    const auto [max_value, outside_area] =
        solver.calc_max_area<long long>(min_l, min_d, max_r, max_u);
    const long long total_area = (max_r - min_l) * (max_u - min_d);
    long long answer = total_area;
    if (max_value == 0)
        answer -= outside_area;
    std::cout << answer << '\n';
    return 0;
}
