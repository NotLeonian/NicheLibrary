#ifndef GEOMETRY_LINE_CONVEX_POLYGON_INTERSECTION_HPP
#define GEOMETRY_LINE_CONVEX_POLYGON_INTERSECTION_HPP

// 反時計回りの狭義凸多角形と直線の交点を求める。
// 返り値は 0 個, 1 個, 2 個の交点である。
// 前提: polygon は反時計回りで, 3 頂点連続で一直線にならず, line_a != line_b。
// 座標型が整数なら内部は整数演算で処理し, 有理点を返す。
// 計算量 O(log N)。

#include <cassert>
#include <complex>
#include <type_traits>
#include <utility>
#include <vector>

namespace line_convex_polygon_intersection_internal {

template <class T> struct is_integral : std::is_integral<T> {};
template <class T> struct is_signed : std::is_signed<T> {};
#ifdef __SIZEOF_INT128__
template <> struct is_integral<__int128_t> : std::true_type {};
template <> struct is_integral<__uint128_t> : std::true_type {};
template <> struct is_signed<__int128_t> : std::true_type {};
template <> struct is_signed<__uint128_t> : std::false_type {};
#endif

template <class T>
using enable_if_integral_t = std::enable_if_t<is_integral<T>::value, int>;

template <class Point, class = void> struct has_member_xy : std::false_type {};
template <class Point>
struct has_member_xy<Point,
                     std::void_t<decltype(std::declval<const Point &>().x),
                                 decltype(std::declval<const Point &>().y)>>
    : std::true_type {};

template <class Point> decltype(auto) get_x(const Point &point) {
    if constexpr (has_member_xy<Point>::value) {
        return point.x;
    } else {
        using std::real;
        return real(point);
    }
}

template <class Point> decltype(auto) get_y(const Point &point) {
    if constexpr (has_member_xy<Point>::value) {
        return point.y;
    } else {
        using std::imag;
        return imag(point);
    }
}

template <class Point>
using coordinate_t =
    std::remove_cvref_t<decltype(get_x(std::declval<const Point &>()))>;

template <class T, class = void> struct wide_integer {};
template <class T>
struct wide_integer<
    T, std::enable_if_t<is_integral<T>::value && is_signed<T>::value>> {
    using type = __int128_t;
};
template <class T>
struct wide_integer<
    T, std::enable_if_t<is_integral<T>::value && !is_signed<T>::value>> {
    using type = __uint128_t;
};

template <class T> using wide_integer_t = typename wide_integer<T>::type;

template <class Point, bool = is_integral<coordinate_t<Point>>::value>
struct calc_type {
    using type = long double;
};
template <class Point> struct calc_type<Point, true> {
    using type = wide_integer_t<coordinate_t<Point>>;
};

template <class Point> using calc_t = typename calc_type<Point>::type;

template <class T> int sign_value(const T &value) {
    if constexpr (is_integral<T>::value) {
        if (value < 0)
            return -1;
        if (value > 0)
            return 1;
        return 0;
    } else {
        constexpr long double eps = 1e-12L;
        if (value < -eps)
            return -1;
        if (value > eps)
            return 1;
        return 0;
    }
}

template <class T> bool equals_value(const T &lhs, const T &rhs) {
    return sign_value(lhs - rhs) == 0;
}

template <class Point> calc_t<Point> to_calc_x(const Point &point) {
    return static_cast<calc_t<Point>>(get_x(point));
}

template <class Point> calc_t<Point> to_calc_y(const Point &point) {
    return static_cast<calc_t<Point>>(get_y(point));
}

template <class Point> calc_t<Point> cross(const Point &lhs, const Point &rhs) {
    return to_calc_x(lhs) * to_calc_y(rhs) - to_calc_y(lhs) * to_calc_x(rhs);
}

template <class Point>
Point make_point(const calc_t<Point> &x, const calc_t<Point> &y) {
    using Coord = coordinate_t<Point>;
    return Point(static_cast<Coord>(x), static_cast<Coord>(y));
}

template <class T, enable_if_integral_t<T> = 0> T abs_value(T value) {
    if constexpr (is_signed<T>::value) {
        return value < 0 ? -value : value;
    } else {
        return value;
    }
}

template <class T, enable_if_integral_t<T> = 0> T gcd_value(T a, T b) {
    a = abs_value(a);
    b = abs_value(b);
    while (b != 0) {
        T t = a % b;
        a = b;
        b = t;
    }
    return a;
}

template <class T, enable_if_integral_t<T> = 0> T gcd3_value(T a, T b, T c) {
    return gcd_value(gcd_value(a, b), c);
}

template <class Func> int first_non_negative(int length, Func &&func) {
    int left = 0;
    int right = length;
    while (left + 1 < right) {
        int mid = (left + right) >> 1;
        if (sign_value(func(mid)) < 0) {
            left = mid;
        } else {
            right = mid;
        }
    }
    return right;
}

template <class Func> int last_non_positive(int length, Func &&func) {
    int left = 0;
    int right = length;
    while (left < right) {
        int mid = (left + right + 1) >> 1;
        if (sign_value(func(mid)) <= 0) {
            left = mid;
        } else {
            right = mid - 1;
        }
    }
    return left;
}

template <class Func> int last_non_negative(int length, Func &&func) {
    int left = 0;
    int right = length;
    while (left < right) {
        int mid = (left + right + 1) >> 1;
        if (sign_value(func(mid)) >= 0) {
            left = mid;
        } else {
            right = mid - 1;
        }
    }
    return left;
}

inline int positive_mod(int index, int mod) {
    int ret = index % mod;
    if (ret < 0)
        ret += mod;
    return ret;
}

inline int distance_forward(int from, int to, int n) {
    if (from <= to)
        return to - from;
    return to + n - from;
}

template <class Point, class Func>
std::pair<int, int> find_extreme_vertices(int n, Func &&height) {
    assert(n >= 3);

    int minimum_index = -1;
    int maximum_index = -1;
    const auto first = height(0);
    const auto last = height(n - 1);
    if (equals_value(first, last)) {
        const auto second = height(1);
        if (sign_value(first - second) < 0) {
            minimum_index = n - 1;
        } else {
            maximum_index = n - 1;
        }
    } else {
        const auto second = height(1);
        if (sign_value(last - first) > 0 && sign_value(first - second) <= 0) {
            minimum_index = 0;
        } else if (sign_value(last - first) < 0 &&
                   sign_value(first - second) >= 0) {
            maximum_index = 0;
        } else {
            const auto denom = static_cast<calc_t<Point>>(n - 1);
            auto g_scaled = [&](int index) {
                const auto base =
                    first * static_cast<calc_t<Point>>(n - 1 - index) +
                    last * static_cast<calc_t<Point>>(index);
                const auto value = height(index) * denom;
                return sign_value(base - value) < 0 ? base : value;
            };
            minimum_index = first_non_negative(n - 1, [&](int index) {
                return g_scaled(index + 1) - g_scaled(index);
            });
        }
    }

    if (maximum_index == -1) {
        const int base = minimum_index - n;
        const int k = first_non_negative(n, [&](int index) {
            return height(base + index) - height(base + index + 1);
        });
        maximum_index = positive_mod(minimum_index + k, n);
    } else {
        const int base = maximum_index - n;
        const int k = first_non_negative(n, [&](int index) {
            return height(base + index + 1) - height(base + index);
        });
        minimum_index = positive_mod(maximum_index + k, n);
    }
    return {minimum_index, maximum_index};
}

} // namespace line_convex_polygon_intersection_internal

template <class T> struct LinePolygonIntersectionPoint {
    T x_numerator;
    T y_numerator;
    T denominator;

    template <class Real> Real x_as() const {
        return static_cast<Real>(x_numerator) / static_cast<Real>(denominator);
    }

    template <class Real> Real y_as() const {
        return static_cast<Real>(y_numerator) / static_cast<Real>(denominator);
    }

    template <class Point> Point to_point() const {
        using Coord =
            line_convex_polygon_intersection_internal::coordinate_t<Point>;
        if constexpr (line_convex_polygon_intersection_internal::is_integral<
                          Coord>::value) {
            assert(denominator == 1);
            return Point(static_cast<Coord>(x_numerator),
                         static_cast<Coord>(y_numerator));
        } else {
            return Point(static_cast<Coord>(x_as<long double>()),
                         static_cast<Coord>(y_as<long double>()));
        }
    }
};

namespace line_convex_polygon_intersection_internal {

template <class Point, bool = is_integral<coordinate_t<Point>>::value>
struct result_value_type {
    using type = Point;
};
template <class Point> struct result_value_type<Point, true> {
    using type = LinePolygonIntersectionPoint<calc_t<Point>>;
};

template <class Point>
using result_value_t = typename result_value_type<Point>::type;

} // namespace line_convex_polygon_intersection_internal

template <class Point>
using LinePolygonIntersectionValue =
    line_convex_polygon_intersection_internal::result_value_t<Point>;

template <class Point>
using LinePolygonIntersectionResult =
    std::vector<LinePolygonIntersectionValue<Point>>;

namespace line_convex_polygon_intersection_internal {

template <class Point>
LinePolygonIntersectionPoint<calc_t<Point>>
make_integral_point(const Point &point) {
    using Calc = calc_t<Point>;
    return {static_cast<Calc>(get_x(point)), static_cast<Calc>(get_y(point)),
            Calc(1)};
}

template <class Point>
LinePolygonIntersectionPoint<calc_t<Point>>
line_edge_intersection_integral(const Point &line_a, const Point &line_b,
                                const Point &segment_a,
                                const Point &segment_b) {
    using Calc = calc_t<Point>;
    const Point direction = line_b - line_a;
    const Point edge = segment_b - segment_a;
    Calc denominator = cross(edge, direction);
    assert(denominator != 0);
    Calc numerator = cross(line_a - segment_a, direction);

    Calc x_numerator = static_cast<Calc>(get_x(segment_a)) * denominator +
                       static_cast<Calc>(get_x(edge)) * numerator;
    Calc y_numerator = static_cast<Calc>(get_y(segment_a)) * denominator +
                       static_cast<Calc>(get_y(edge)) * numerator;

    if (denominator < 0) {
        denominator = -denominator;
        x_numerator = -x_numerator;
        y_numerator = -y_numerator;
    }

    const Calc g = gcd3_value(x_numerator, y_numerator, denominator);
    if (g != 0) {
        x_numerator /= g;
        y_numerator /= g;
        denominator /= g;
    }
    return {x_numerator, y_numerator, denominator};
}

template <class Point>
Point line_edge_intersection_floating(const Point &line_a, const Point &line_b,
                                      const Point &segment_a,
                                      const Point &segment_b) {
    using Calc = calc_t<Point>;
    const Point direction = line_b - line_a;
    const Point edge = segment_b - segment_a;
    const Calc denominator = cross(edge, direction);
    assert(sign_value(denominator) != 0);
    const Calc numerator = cross(line_a - segment_a, direction);
    const Calc t = numerator / denominator;
    const Calc x = static_cast<Calc>(get_x(segment_a)) +
                   static_cast<Calc>(get_x(edge)) * t;
    const Calc y = static_cast<Calc>(get_y(segment_a)) +
                   static_cast<Calc>(get_y(edge)) * t;
    return make_point<Point>(x, y);
}

template <class Point>
result_value_t<Point> make_vertex_result(const Point &point) {
    if constexpr (is_integral<coordinate_t<Point>>::value) {
        return make_integral_point(point);
    } else {
        return point;
    }
}

template <class Point>
result_value_t<Point> make_edge_result(const Point &line_a, const Point &line_b,
                                       const Point &segment_a,
                                       const Point &segment_b) {
    if constexpr (is_integral<coordinate_t<Point>>::value) {
        return line_edge_intersection_integral(line_a, line_b, segment_a,
                                               segment_b);
    } else {
        return line_edge_intersection_floating(line_a, line_b, segment_a,
                                               segment_b);
    }
}

} // namespace line_convex_polygon_intersection_internal

template <class Point>
LinePolygonIntersectionResult<Point>
line_polygon_intersection(const std::vector<Point> &polygon,
                          const Point &line_a, const Point &line_b) {
    using namespace line_convex_polygon_intersection_internal;
    using Calc = calc_t<Point>;

    static_assert(!is_integral<coordinate_t<Point>>::value ||
                      is_signed<coordinate_t<Point>>::value,
                  "integral coordinate type must be signed");

    const int n = static_cast<int>(polygon.size());
    assert(n >= 3);
    assert(!equals_value(static_cast<Calc>(get_x(line_a)),
                         static_cast<Calc>(get_x(line_b))) ||
           !equals_value(static_cast<Calc>(get_y(line_a)),
                         static_cast<Calc>(get_y(line_b))));

    const Point direction = line_b - line_a;
    auto vertex = [&](int index) -> const Point & {
        return polygon[positive_mod(index, n)];
    };
    auto height = [&](int index) -> Calc {
        return cross(vertex(index) - line_a, direction);
    };
    auto chain_index = [&](int start, int step, int offset) {
        return positive_mod(start + step * offset, n);
    };

    const auto [minimum_index, maximum_index] =
        find_extreme_vertices<Point>(n, height);
    const Calc minimum_value = height(minimum_index);
    const Calc maximum_value = height(maximum_index);

    LinePolygonIntersectionResult<Point> result;
    if (sign_value(minimum_value) > 0 || sign_value(maximum_value) < 0) {
        return result;
    }

    const int forward_length =
        distance_forward(minimum_index, maximum_index, n);
    const int backward_length = n - forward_length;

    if (sign_value(minimum_value) == 0) {
        const int forward_zero =
            last_non_positive(forward_length, [&](int offset) {
                return height(minimum_index + offset);
            });
        const int backward_zero =
            last_non_positive(backward_length, [&](int offset) {
                return height(minimum_index - offset);
            });
        const int forward_index = chain_index(minimum_index, +1, forward_zero);
        const int backward_index =
            chain_index(minimum_index, -1, backward_zero);
        result.push_back(make_vertex_result(vertex(forward_index)));
        if (forward_index != backward_index) {
            result.push_back(make_vertex_result(vertex(backward_index)));
        }
        return result;
    }

    if (sign_value(maximum_value) == 0) {
        const int forward_length_from_max =
            distance_forward(maximum_index, minimum_index, n);
        const int backward_length_from_max = n - forward_length_from_max;
        const int forward_zero =
            last_non_negative(forward_length_from_max, [&](int offset) {
                return height(maximum_index + offset);
            });
        const int backward_zero =
            last_non_negative(backward_length_from_max, [&](int offset) {
                return height(maximum_index - offset);
            });
        const int forward_index = chain_index(maximum_index, +1, forward_zero);
        const int backward_index =
            chain_index(maximum_index, -1, backward_zero);
        result.push_back(make_vertex_result(vertex(forward_index)));
        if (forward_index != backward_index) {
            result.push_back(make_vertex_result(vertex(backward_index)));
        }
        return result;
    }

    const int first_cross = first_non_negative(forward_length, [&](int offset) {
        return height(minimum_index + offset);
    });
    const int first_cross_index = chain_index(minimum_index, +1, first_cross);
    if (sign_value(height(first_cross_index)) == 0) {
        result.push_back(make_vertex_result(vertex(first_cross_index)));
    } else {
        const int prev_index = chain_index(minimum_index, +1, first_cross - 1);
        result.push_back(make_edge_result(line_a, line_b, vertex(prev_index),
                                          vertex(first_cross_index)));
    }

    const int second_cross =
        first_non_negative(backward_length, [&](int offset) {
            return height(minimum_index - offset);
        });
    const int second_cross_index = chain_index(minimum_index, -1, second_cross);
    if (sign_value(height(second_cross_index)) == 0) {
        result.push_back(make_vertex_result(vertex(second_cross_index)));
    } else {
        const int prev_index = chain_index(minimum_index, -1, second_cross - 1);
        result.push_back(make_edge_result(line_a, line_b, vertex(prev_index),
                                          vertex(second_cross_index)));
    }

    return result;
}

#endif
