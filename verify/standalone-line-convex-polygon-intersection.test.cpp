// competitive-verifier: STANDALONE

#include <cassert>
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

    return 0;
}
