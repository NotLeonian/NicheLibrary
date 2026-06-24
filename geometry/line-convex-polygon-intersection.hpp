#ifndef GEOMETRY_LINE_CONVEX_POLYGON_INTERSECTION_HPP
#define GEOMETRY_LINE_CONVEX_POLYGON_INTERSECTION_HPP

// 反時計回りの面積正の狭義凸多角形と直線の共通部分を求める。
// 共通部分が空集合なら 0 個、1 点なら 1 個、線分ならその端点 2 個を返す。
// 前提: polygon は面積正の狭義凸多角形で、反時計回りで、
//       3 頂点連続で一直線にならず、line_a != line_b。
// 注意: convex_hull の結果が 1 点または 2 点になる退化ケースは前提外。
//       呼び出し側で点または線分として処理すること。
// 座標型が整数なら内部は整数演算で処理し、交点を有理表現で返す。
// 標準の 64 bit 以下の整数座標では、既定で 128 bit 整数型を内部計算に用いる。
// 計算途中に必要な値が内部計算の型に収まることを仮定する。
// 計算量 O(log N)。

#include <cassert>
#include <complex>
#include <limits>
#include <type_traits>
#include <utility>
#include <vector>

#include "../internal/int128.hpp"

namespace line_convex_polygon_intersection_internal {
template <class T>
constexpr bool is_integer_v =
    std::numeric_limits<std::remove_cv_t<T>>::is_integer;

template <class T>
constexpr bool is_signed_v =
    std::numeric_limits<std::remove_cv_t<T>>::is_signed;

template <class T>
constexpr bool use_int128_by_default_v =
    std::is_integral_v<std::remove_cv_t<T>> &&
    !std::is_same_v<std::remove_cv_t<T>, bool> &&
    std::numeric_limits<std::remove_cv_t<T>>::digits <= 64;

template <class T>
using enable_if_integer_t = std::enable_if_t<is_integer_v<T>, int>;

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

template <class Coord, bool = is_integer_v<Coord>> struct default_calc_type {
    using type = long double;
};

template <class Coord> struct default_calc_type<Coord, true> {
    using type = std::conditional_t<use_int128_by_default_v<Coord>,
                                    NicheLibrary::Int128, Coord>;
};

template <class Point>
using default_calc_t = typename default_calc_type<coordinate_t<Point>>::type;

template <class Point, class Calc> struct resolved_calc {
    using type = Calc;
};

template <class Point> struct resolved_calc<Point, void> {
    using type = default_calc_t<Point>;
};

template <class Point, class Calc>
using resolved_calc_t = typename resolved_calc<Point, Calc>::type;

template <class T> int sign_value(const T &value) {
    if constexpr (is_integer_v<T>) {
        if (value < 0) {
            return -1;
        }
        if (value > 0) {
            return 1;
        }
        return 0;
    } else {
        constexpr long double eps = 1e-12L;
        if (value < -eps) {
            return -1;
        }
        if (value > eps) {
            return 1;
        }
        return 0;
    }
}

template <class Point, class Calc> Calc to_calc_x(const Point &point) {
    return static_cast<Calc>(get_x(point));
}

template <class Point, class Calc> Calc to_calc_y(const Point &point) {
    return static_cast<Calc>(get_y(point));
}

template <class Point, class Calc>
Calc diff_x(const Point &lhs, const Point &rhs) {
    return to_calc_x<Point, Calc>(lhs) - to_calc_x<Point, Calc>(rhs);
}

template <class Point, class Calc>
Calc diff_y(const Point &lhs, const Point &rhs) {
    return to_calc_y<Point, Calc>(lhs) - to_calc_y<Point, Calc>(rhs);
}

template <class Point, class Calc>
Calc cross_diff(const Point &lhs_a, const Point &lhs_b, const Point &rhs_a,
                const Point &rhs_b) {
    return diff_x<Point, Calc>(lhs_a, lhs_b) *
               diff_y<Point, Calc>(rhs_a, rhs_b) -
           diff_y<Point, Calc>(lhs_a, lhs_b) *
               diff_x<Point, Calc>(rhs_a, rhs_b);
}

template <class Point, class Calc>
Point make_point(const Calc &x, const Calc &y) {
    using Coord = coordinate_t<Point>;
    return Point(static_cast<Coord>(x), static_cast<Coord>(y));
}

template <class T> bool equals_value(const T &lhs, const T &rhs) {
    return sign_value(lhs - rhs) == 0;
}

template <class T, enable_if_integer_t<T> = 0> T abs_value(T value) {
    if constexpr (is_signed_v<T>) {
        return value < 0 ? -value : value;
    } else {
        return value;
    }
}

template <class T, enable_if_integer_t<T> = 0> T gcd_value(T a, T b) {
    a = abs_value(a);
    b = abs_value(b);
    while (b != 0) {
        T t = a % b;
        a = b;
        b = t;
    }
    return a;
}

template <class T, enable_if_integer_t<T> = 0> T gcd3_value(T a, T b, T c) {
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
    if (ret < 0) {
        ret += mod;
    }
    return ret;
}

inline int distance_forward(int from, int to, int n) {
    if (from <= to) {
        return to - from;
    }
    return to + n - from;
}

template <class T> int compare_value(const T &lhs, const T &rhs) {
    return sign_value(lhs - rhs);
}

template <class Func>
std::pair<int, int> find_extreme_vertices(int n, Func &&height) {
    assert(n >= 3);
    using Value = std::remove_cvref_t<decltype(height(0))>;

    int minimum_index = -1;
    int maximum_index = -1;

    const Value first = height(0);
    const Value second = height(1);
    const Value last = height(n - 1);

    const int last_vs_first = compare_value(last, first);
    const int first_vs_second = compare_value(first, second);

    if (last_vs_first == 0) {
        if (first_vs_second < 0) {
            minimum_index = n - 1;
        } else {
            maximum_index = n - 1;
        }
    } else if (last_vs_first > 0 && first_vs_second <= 0) {
        minimum_index = 0;
    } else if (last_vs_first < 0 && first_vs_second >= 0) {
        maximum_index = 0;
    } else {
        const Value denominator = static_cast<Value>(n - 1);

        auto lower_envelope_scaled = [&](int index) -> Value {
            const Value left_weight = static_cast<Value>(n - 1 - index);
            const Value right_weight = static_cast<Value>(index);

            const Value chord_scaled =
                first * left_weight + last * right_weight;
            const Value height_scaled = height(index) * denominator;

            if (compare_value(chord_scaled, height_scaled) < 0) {
                return chord_scaled;
            }
            return height_scaled;
        };

        minimum_index = first_non_negative(n - 1, [&](int index) -> Value {
            return lower_envelope_scaled(index + 1) -
                   lower_envelope_scaled(index);
        });
    }

    if (maximum_index == -1) {
        const int base = minimum_index - n;
        const int offset = first_non_negative(n, [&](int index) -> Value {
            return height(base + index) - height(base + index + 1);
        });
        maximum_index = positive_mod(minimum_index + offset, n);
    } else {
        const int base = maximum_index - n;
        const int offset = first_non_negative(n, [&](int index) -> Value {
            return height(base + index + 1) - height(base + index);
        });
        minimum_index = positive_mod(maximum_index + offset, n);
    }

    return {minimum_index, maximum_index};
}

template <class Real, class T> Real number_as_real(const T &value) {
    return static_cast<Real>(value);
}

template <class Real> Real number_as_real(NicheLibrary::UInt128 value) {
    return std::ldexp(static_cast<Real>(value.high()), 64) +
           static_cast<Real>(value.low());
}

template <class Real> Real number_as_real(NicheLibrary::Int128 value) {
    const NicheLibrary::UInt128 bits =
        NicheLibrary::UInt128::from_words(value.high(), value.low());
    if (value.is_negative()) {
        return -number_as_real<Real>(-bits);
    }
    return number_as_real<Real>(bits);
}
} // namespace line_convex_polygon_intersection_internal

template <class T> struct LinePolygonIntersectionPoint {
    T x_numerator;
    T y_numerator;
    T denominator;

    template <class Real> Real x_as() const {
        namespace lpi_internal = line_convex_polygon_intersection_internal;
        return lpi_internal::number_as_real<Real>(x_numerator) /
               lpi_internal::number_as_real<Real>(denominator);
    }

    template <class Real> Real y_as() const {
        namespace lpi_internal = line_convex_polygon_intersection_internal;
        return lpi_internal::number_as_real<Real>(y_numerator) /
               lpi_internal::number_as_real<Real>(denominator);
    }

    template <class Point> Point to_point() const {
        namespace lpi_internal = line_convex_polygon_intersection_internal;
        using Coord = lpi_internal::coordinate_t<Point>;
        if constexpr (lpi_internal::is_integer_v<Coord>) {
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
template <class Point, class Calc, bool = is_integer_v<coordinate_t<Point>>>
struct result_value_type {
    using type = Point;
};

template <class Point, class Calc> struct result_value_type<Point, Calc, true> {
    using type = LinePolygonIntersectionPoint<Calc>;
};

template <class Point, class Calc>
using result_value_t = typename result_value_type<Point, Calc>::type;
} // namespace line_convex_polygon_intersection_internal

template <class Point, class Calc = void>
using LinePolygonIntersectionValue =
    line_convex_polygon_intersection_internal::result_value_t<
        Point, line_convex_polygon_intersection_internal::resolved_calc_t<
                   Point, Calc>>;

template <class Point, class Calc = void>
using LinePolygonIntersectionResult =
    std::vector<LinePolygonIntersectionValue<Point, Calc>>;

namespace line_convex_polygon_intersection_internal {
template <class Point, class Calc>
LinePolygonIntersectionPoint<Calc> make_integral_point(const Point &point) {
    return {to_calc_x<Point, Calc>(point), to_calc_y<Point, Calc>(point),
            Calc(1)};
}

template <class Point, class Calc>
LinePolygonIntersectionPoint<Calc>
line_edge_intersection_integral(const Point &line_a, const Point &line_b,
                                const Point &segment_a,
                                const Point &segment_b) {
    const Calc edge_x = diff_x<Point, Calc>(segment_b, segment_a);
    const Calc edge_y = diff_y<Point, Calc>(segment_b, segment_a);

    Calc denominator =
        cross_diff<Point, Calc>(segment_b, segment_a, line_b, line_a);
    assert(denominator != 0);
    Calc numerator = cross_diff<Point, Calc>(line_a, segment_a, line_b, line_a);

    Calc x_numerator =
        to_calc_x<Point, Calc>(segment_a) * denominator + edge_x * numerator;
    Calc y_numerator =
        to_calc_y<Point, Calc>(segment_a) * denominator + edge_y * numerator;

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

template <class Point, class Calc>
Point line_edge_intersection_floating(const Point &line_a, const Point &line_b,
                                      const Point &segment_a,
                                      const Point &segment_b) {
    const Calc edge_x = diff_x<Point, Calc>(segment_b, segment_a);
    const Calc edge_y = diff_y<Point, Calc>(segment_b, segment_a);

    const Calc denominator =
        cross_diff<Point, Calc>(segment_b, segment_a, line_b, line_a);
    assert(sign_value(denominator) != 0);

    const Calc numerator =
        cross_diff<Point, Calc>(line_a, segment_a, line_b, line_a);

    const Calc t = numerator / denominator;
    const Calc x = to_calc_x<Point, Calc>(segment_a) + edge_x * t;
    const Calc y = to_calc_y<Point, Calc>(segment_a) + edge_y * t;

    return make_point<Point, Calc>(x, y);
}

template <class Point, class Calc>
result_value_t<Point, Calc> make_vertex_result(const Point &point) {
    if constexpr (is_integer_v<coordinate_t<Point>>) {
        return make_integral_point<Point, Calc>(point);
    } else {
        return point;
    }
}

template <class Point, class Calc>
result_value_t<Point, Calc>
make_edge_result(const Point &line_a, const Point &line_b,
                 const Point &segment_a, const Point &segment_b) {
    if constexpr (is_integer_v<coordinate_t<Point>>) {
        return line_edge_intersection_integral<Point, Calc>(
            line_a, line_b, segment_a, segment_b);
    } else {
        return line_edge_intersection_floating<Point, Calc>(
            line_a, line_b, segment_a, segment_b);
    }
}
} // namespace line_convex_polygon_intersection_internal

template <class Point, class Calc = void>
LinePolygonIntersectionResult<Point, Calc>
line_polygon_intersection(const std::vector<Point> &polygon,
                          const Point &line_a, const Point &line_b) {
    namespace lpi_internal = line_convex_polygon_intersection_internal;
    using Coord = lpi_internal::coordinate_t<Point>;
    using Number = lpi_internal::resolved_calc_t<Point, Calc>;

    static_assert(!lpi_internal::is_integer_v<Coord> || lpi_internal::is_signed_v<Coord>,
                  "integer coordinate type must be signed");
    static_assert(!lpi_internal::is_integer_v<Coord> || lpi_internal::is_integer_v<Number>,
                  "integer coordinate type requires integer calculation type");
    static_assert(!lpi_internal::is_integer_v<Number> || lpi_internal::is_signed_v<Number>,
                  "integer calculation type must be signed");

    const int n = static_cast<int>(polygon.size());
    assert(n >= 3);
    assert(!lpi_internal::equals_value(lpi_internal::to_calc_x<Point, Number>(line_a),
                              lpi_internal::to_calc_x<Point, Number>(line_b)) ||
           !lpi_internal::equals_value(lpi_internal::to_calc_y<Point, Number>(line_a),
                              lpi_internal::to_calc_y<Point, Number>(line_b)));

    auto vertex = [&](int index) -> const Point & {
        return polygon[lpi_internal::positive_mod(index, n)];
    };

    auto height = [&](int index) -> Number {
        return lpi_internal::cross_diff<Point, Number>(vertex(index), line_a, line_b,
                                              line_a);
    };

    auto chain_index = [&](int start, int step, int offset) {
        return lpi_internal::positive_mod(start + step * offset, n);
    };

    const auto [minimum_index, maximum_index] =
        lpi_internal::find_extreme_vertices<Point>(n, height);
    const Number minimum_value = height(minimum_index);
    const Number maximum_value = height(maximum_index);

    LinePolygonIntersectionResult<Point, Calc> result;

    if (lpi_internal::sign_value(minimum_value) > 0 ||
        lpi_internal::sign_value(maximum_value) < 0) {
        return result;
    }

    const int forward_length =
        lpi_internal::distance_forward(minimum_index, maximum_index, n);
    const int backward_length = n - forward_length;

    if (lpi_internal::sign_value(minimum_value) == 0) {
        const int forward_zero =
            lpi_internal::last_non_positive(forward_length, [&](int offset) {
                return height(minimum_index + offset);
            });
        const int backward_zero =
            lpi_internal::last_non_positive(backward_length, [&](int offset) {
                return height(minimum_index - offset);
            });

        const int forward_index = chain_index(minimum_index, +1, forward_zero);
        const int backward_index =
            chain_index(minimum_index, -1, backward_zero);

        result.push_back(
            lpi_internal::make_vertex_result<Point, Number>(vertex(forward_index)));
        if (forward_index != backward_index) {
            result.push_back(
                lpi_internal::make_vertex_result<Point, Number>(vertex(backward_index)));
        }
        return result;
    }

    if (lpi_internal::sign_value(maximum_value) == 0) {
        const int forward_length_from_max =
            lpi_internal::distance_forward(maximum_index, minimum_index, n);
        const int backward_length_from_max = n - forward_length_from_max;

        const int forward_zero =
            lpi_internal::last_non_negative(forward_length_from_max, [&](int offset) {
                return height(maximum_index + offset);
            });
        const int backward_zero =
            lpi_internal::last_non_negative(backward_length_from_max, [&](int offset) {
                return height(maximum_index - offset);
            });

        const int forward_index = chain_index(maximum_index, +1, forward_zero);
        const int backward_index =
            chain_index(maximum_index, -1, backward_zero);

        result.push_back(
            lpi_internal::make_vertex_result<Point, Number>(vertex(forward_index)));
        if (forward_index != backward_index) {
            result.push_back(
                lpi_internal::make_vertex_result<Point, Number>(vertex(backward_index)));
        }
        return result;
    }

    const int first_cross =
        lpi_internal::first_non_negative(forward_length, [&](int offset) {
            return height(minimum_index + offset);
        });
    const int first_cross_index = chain_index(minimum_index, +1, first_cross);

    if (lpi_internal::sign_value(height(first_cross_index)) == 0) {
        result.push_back(
            lpi_internal::make_vertex_result<Point, Number>(vertex(first_cross_index)));
    } else {
        const int prev_index = chain_index(minimum_index, +1, first_cross - 1);
        result.push_back(lpi_internal::make_edge_result<Point, Number>(
            line_a, line_b, vertex(prev_index), vertex(first_cross_index)));
    }

    const int second_cross =
        lpi_internal::first_non_negative(backward_length, [&](int offset) {
            return height(minimum_index - offset);
        });
    const int second_cross_index = chain_index(minimum_index, -1, second_cross);

    if (lpi_internal::sign_value(height(second_cross_index)) == 0) {
        result.push_back(
            lpi_internal::make_vertex_result<Point, Number>(vertex(second_cross_index)));
    } else {
        const int prev_index = chain_index(minimum_index, -1, second_cross - 1);
        result.push_back(lpi_internal::make_edge_result<Point, Number>(
            line_a, line_b, vertex(prev_index), vertex(second_cross_index)));
    }

    return result;
}

#endif
