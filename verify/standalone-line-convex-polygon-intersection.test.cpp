// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <complex>
#include <vector>

#include "../geometry/line-convex-polygon-intersection.hpp"

int main() {
    using Point = std::complex<long long>;
    using NicheLibrary::Int128;

    {
        std::vector<Point> hull = {Point(2, 3)};
        const auto result =
            line_convex_hull_intersection(hull, Point(0, 3), Point(1, 3));

        assert(result.size() == 1);
        assert(result[0].x_numerator == Int128(2));
        assert(result[0].y_numerator == Int128(3));
        assert(result[0].denominator == Int128(1));
    }

    {
        std::vector<Point> hull = {Point(2, 3)};
        const auto result =
            line_convex_hull_intersection(hull, Point(0, 4), Point(1, 4));

        assert(result.empty());
    }

    {
        std::vector<Point> hull = {Point(0, 0), Point(4, 4)};
        const auto result =
            line_convex_hull_intersection(hull, Point(0, 2), Point(1, 2));

        assert(result.size() == 1);
        assert(result[0].x_numerator == Int128(2));
        assert(result[0].y_numerator == Int128(2));
        assert(result[0].denominator == Int128(1));
    }

    {
        std::vector<Point> hull = {Point(0, 0), Point(4, 4)};
        const auto result =
            line_convex_hull_intersection(hull, Point(0, 0), Point(1, 1));

        assert(result.size() == 2);
        assert(result[0].denominator == Int128(1));
        assert(result[1].denominator == Int128(1));
    }

    {
        std::vector<Point> hull = {Point(0, 0), Point(4, 4)};
        const auto result =
            line_convex_hull_intersection(hull, Point(0, 5), Point(1, 5));

        assert(result.empty());
    }

    {
        std::vector<Point> hull = {Point(0, 0), Point(4, 0), Point(0, 4)};
        const auto result =
            line_convex_hull_intersection(hull, Point(0, 2), Point(1, 2));

        assert(result.size() == 2);
    }

    {
        std::vector<Point> hull = {Point(0, 0), Point(4, 0), Point(0, 4)};
        const auto result =
            line_convex_hull_intersection(hull, Point(0, 1), Point(3, 2));

        assert(result.size() == 2);
        auto is_point = [&](int index, Int128 x_numerator, Int128 y_numerator,
                            Int128 denominator) {
            return result[index].x_numerator == x_numerator &&
                   result[index].y_numerator == y_numerator &&
                   result[index].denominator == denominator;
        };
        const bool expected_order =
            is_point(0, Int128(0), Int128(1), Int128(1)) &&
            is_point(1, Int128(9), Int128(7), Int128(4));
        const bool reverse_order =
            is_point(1, Int128(0), Int128(1), Int128(1)) &&
            is_point(0, Int128(9), Int128(7), Int128(4));
        assert(expected_order || reverse_order);

        const int rational_index = result[0].denominator == Int128(4) ? 0 : 1;
        const auto real_point =
            result[rational_index]
                .template to_point<std::complex<long double>>();
        assert(std::abs(real_point.real() - 2.25L) < 1e-15L);
        assert(std::abs(real_point.imag() - 1.75L) < 1e-15L);
    }

    return 0;
}
