// competitive-verifier: PROBLEM https://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=CGL_4_C
// competitive-verifier: ERROR 0.00001

#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <utility>
#include <vector>

#include "../geometry/line-convex-polygon-intersection.hpp"

struct Point {
    long long x;
    long long y;

    Point() : x(0), y(0) {}
    Point(long long x_, long long y_) : x(x_), y(y_) {}

    Point operator-(const Point &other) const {
        return Point(x - other.x, y - other.y);
    }
};

struct RealPoint {
    long double x;
    long double y;

    RealPoint(long double x_, long double y_) : x(x_), y(y_) {}
};

__int128_t cross_value(const Point &a, const Point &b) {
    return static_cast<__int128_t>(a.x) * static_cast<__int128_t>(b.y) -
           static_cast<__int128_t>(a.y) * static_cast<__int128_t>(b.x);
}

int sign_value(__int128_t x) {
    if (x < 0) {
        return -1;
    }
    if (x > 0) {
        return 1;
    }
    return 0;
}

Point read_point() {
    long long x, y;
    std::cin >> x >> y;
    return Point(x, y);
}

RealPoint to_real_point(const Point &p) {
    return RealPoint(static_cast<long double>(p.x),
                     static_cast<long double>(p.y));
}

long double cross_ld(const RealPoint &a, const RealPoint &b) {
    return a.x * b.y - a.y * b.x;
}

long double dot_ld(const RealPoint &a, const RealPoint &b) {
    return a.x * b.x + a.y * b.y;
}

std::vector<RealPoint> convex_cut(const std::vector<Point> &polygon,
                                  const Point &line_a, const Point &line_b) {
    const int n = static_cast<int>(polygon.size());
    const Point direction = line_b - line_a;
    const RealPoint real_direction = to_real_point(direction);

    const auto exact_intersections =
        line_polygon_intersection(polygon, line_a, line_b);
    std::vector<RealPoint> intersections;
    intersections.reserve(exact_intersections.size());
    for (const auto &point : exact_intersections) {
        intersections.emplace_back(point.template x_as<long double>(),
                                   point.template y_as<long double>());
    }
    if (intersections.size() == 2 &&
        dot_ld(intersections[1], real_direction) <
            dot_ld(intersections[0], real_direction)) {
        std::swap(intersections[0], intersections[1]);
    }

    std::vector<RealPoint> result;
    result.reserve(n + 2);
    for (int i = 0; i < n; ++i) {
        const Point &now = polygon[i];
        const Point &next = polygon[(i + 1) % n];

        // AOJ CGL_4_C は有向直線 line_a -> line_b の左側を残す。
        const __int128_t current = cross_value(direction, now - line_a);
        const __int128_t next_value = cross_value(direction, next - line_a);

        if (sign_value(current) >= 0) {
            result.push_back(to_real_point(now));
        }
        if (current < 0 && next_value > 0) {
            assert(intersections.size() == 2);
            result.push_back(intersections[1]);
        } else if (current > 0 && next_value < 0) {
            assert(intersections.size() == 2);
            result.push_back(intersections[0]);
        }
    }
    return result;
}

long double polygon_area(const std::vector<RealPoint> &polygon) {
    const int n = static_cast<int>(polygon.size());
    long double area = 0;
    for (int i = 0; i < n; ++i) {
        area += cross_ld(polygon[i], polygon[(i + 1) % n]);
    }
    return std::abs(area) / 2;
}

std::vector<Point>
remove_collinear_vertices(const std::vector<Point> &polygon) {
    const int n = static_cast<int>(polygon.size());
    std::vector<Point> result;
    result.reserve(n);
    for (int i = 0; i < n; ++i) {
        const Point &prev = polygon[(i + n - 1) % n];
        const Point &cur = polygon[i];
        const Point &next = polygon[(i + 1) % n];
        if (sign_value(cross_value(cur - prev, next - cur)) == 0) {
            continue;
        }
        result.push_back(cur);
    }
    assert(result.size() >= 3);
    return result;
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    std::cin >> n;
    std::vector<Point> polygon(n);
    for (auto &p : polygon) {
        p = read_point();
    }
    polygon = remove_collinear_vertices(polygon);

    int q;
    std::cin >> q;
    std::cout << std::fixed << std::setprecision(20);
    while (q--) {
        const Point line_a = read_point();
        const Point line_b = read_point();
        std::cout << polygon_area(convex_cut(polygon, line_a, line_b)) << '\n';
    }

    return 0;
}
