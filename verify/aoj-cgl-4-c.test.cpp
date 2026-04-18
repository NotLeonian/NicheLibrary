// competitive-verifier: PROBLEM https://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=CGL_4_C
#define ERROR 0.00001

#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

#include "../geometry/line-convex-polygon-intersection.hpp"

struct Point {
    long long x;
    long long y;

    Point() : x(0), y(0) {}
    Point(long long x_, long long y_) : x(x_), y(y_) {}

    Point operator+(const Point &other) const {
        return Point(x + other.x, y + other.y);
    }
    Point operator-(const Point &other) const {
        return Point(x - other.x, y - other.y);
    }
};

struct RealPoint {
    long double x;
    long double y;

    RealPoint() : x(0), y(0) {}
    RealPoint(long double x_, long double y_) : x(x_), y(y_) {}

    RealPoint operator+(const RealPoint &other) const {
        return RealPoint(x + other.x, y + other.y);
    }
    RealPoint operator-(const RealPoint &other) const {
        return RealPoint(x - other.x, y - other.y);
    }
    RealPoint operator*(long double k) const { return RealPoint(x * k, y * k); }
};

__int128_t cross_value(const Point &a, const Point &b) {
    return static_cast<__int128_t>(a.x) * static_cast<__int128_t>(b.y) -
           static_cast<__int128_t>(a.y) * static_cast<__int128_t>(b.x);
}

int sign_value(__int128_t x) {
    if (x < 0)
        return -1;
    if (x > 0)
        return 1;
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

bool same_point(const RealPoint &a, const RealPoint &b) {
    return std::abs(a.x - b.x) <= 1e-12L && std::abs(a.y - b.y) <= 1e-12L;
}

void add_unique(std::vector<RealPoint> &points, const RealPoint &point) {
    for (const auto &q : points) {
        if (same_point(q, point))
            return;
    }
    points.push_back(point);
}

RealPoint line_intersection(const Point &line_a, const Point &line_b,
                            const Point &segment_a, const Point &segment_b) {
    const RealPoint u = to_real_point(segment_a);
    const RealPoint v = to_real_point(segment_b);
    const RealPoint dir = to_real_point(line_b - line_a);
    const RealPoint edge = v - u;
    const long double denominator = cross_ld(edge, dir);
    const long double numerator =
        cross_ld(to_real_point(line_a - segment_a), dir);
    const long double t = numerator / denominator;
    return u + edge * t;
}

std::vector<RealPoint> brute_intersections(const std::vector<Point> &polygon,
                                           const Point &line_a,
                                           const Point &line_b) {
    const int n = static_cast<int>(polygon.size());
    const Point direction = line_b - line_a;
    std::vector<RealPoint> points;
    for (int i = 0; i < n; ++i) {
        const Point &u = polygon[i];
        const Point &v = polygon[(i + 1) % n];
        const __int128_t hu = cross_value(u - line_a, direction);
        const __int128_t hv = cross_value(v - line_a, direction);
        const int su = sign_value(hu);
        const int sv = sign_value(hv);
        if (su == 0 && sv == 0) {
            add_unique(points, to_real_point(u));
            add_unique(points, to_real_point(v));
        } else if (su == 0) {
            add_unique(points, to_real_point(u));
        } else if (sv == 0) {
            add_unique(points, to_real_point(v));
        } else if (su != sv) {
            add_unique(points, line_intersection(line_a, line_b, u, v));
        }
    }
    if (points.size() <= 2)
        return points;
    const RealPoint dir = to_real_point(direction);
    int min_index = 0;
    int max_index = 0;
    for (int i = 1; i < static_cast<int>(points.size()); ++i) {
        if (dot_ld(points[i], dir) < dot_ld(points[min_index], dir))
            min_index = i;
        if (dot_ld(points[i], dir) > dot_ld(points[max_index], dir))
            max_index = i;
    }
    return {points[min_index], points[max_index]};
}

std::vector<RealPoint> convex_cut(const std::vector<Point> &polygon,
                                  const Point &line_a, const Point &line_b) {
    const int n = static_cast<int>(polygon.size());
    const Point direction = line_b - line_a;
    std::vector<RealPoint> result;
    for (int i = 0; i < n; ++i) {
        const Point &now = polygon[i];
        const Point &next = polygon[(i + 1) % n];

        // AOJ CGL_4_C は有向直線 line_a -> line_b の左側を残す。
        const __int128_t current = cross_value(direction, now - line_a);
        const __int128_t next_value = cross_value(direction, next - line_a);

        if (sign_value(current) >= 0)
            result.push_back(to_real_point(now));
        if ((current < 0 && next_value > 0) ||
            (current > 0 && next_value < 0)) {
            result.push_back(line_intersection(line_a, line_b, now, next));
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
        if (sign_value(cross_value(cur - prev, next - cur)) == 0)
            continue;
        result.push_back(cur);
    }
    assert(result.size() >= 3);
    return result;
}

void verify_intersection(const std::vector<Point> &polygon, const Point &line_a,
                         const Point &line_b) {
    const auto fast = line_polygon_intersection(polygon, line_a, line_b);
    const auto slow = brute_intersections(polygon, line_a, line_b);
    assert(fast.size() == slow.size());
    if (fast.empty())
        return;

    auto to_real = [](const auto &point) {
        return RealPoint(point.template x_as<long double>(),
                         point.template y_as<long double>());
    };

    if (fast.size() == 1) {
        assert(same_point(to_real(fast[0]), slow[0]));
        return;
    }
    const RealPoint p0 = to_real(fast[0]);
    const RealPoint p1 = to_real(fast[1]);
    const bool ok = (same_point(p0, slow[0]) && same_point(p1, slow[1])) ||
                    (same_point(p0, slow[1]) && same_point(p1, slow[0]));
    assert(ok);
}

int main() {
    int n;
    std::cin >> n;
    std::vector<Point> polygon(n);
    for (auto &p : polygon)
        p = read_point();
    polygon = remove_collinear_vertices(polygon);

    int q;
    std::cin >> q;
    std::cout << std::fixed << std::setprecision(20);
    while (q--) {
        const Point line_a = read_point();
        const Point line_b = read_point();
        verify_intersection(polygon, line_a, line_b);
        std::cout << polygon_area(convex_cut(polygon, line_a, line_b)) << '\n';
    }
}
