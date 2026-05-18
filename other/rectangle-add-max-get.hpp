#ifndef OTHER_RECTANGLE_ADD_MAX_GET_HPP
#define OTHER_RECTANGLE_ADD_MAX_GET_HPP

// 2 次元平面上の重み付き半開長方形を加算し、最大値を求める。
// RectangleAddMaxGet は座標圧縮せず、x 座標を 1 ずつ走査する。
// CompressedRectangleAddMaxGet は座標圧縮して平面走査する。
// 面積 0 の長方形は無視する。
// 重みは負でもよいが、正の面積で符号付き整数型の最小値は使わない。
// 計算量は各 calc の説明を参照する。

#include <algorithm>
#include <cassert>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace rectangle_add_max_get_internal {

template <class T> using CoordinateLength = std::make_unsigned_t<T>;

template <class T, class C> struct Rectangle {
    T l, d, r, u;
    C w;
};

template <class C> void assert_valid_weight(C w) {
    if constexpr (std::numeric_limits<C>::is_specialized &&
                  std::numeric_limits<C>::is_integer &&
                  std::numeric_limits<C>::is_signed) {
        assert(w != std::numeric_limits<C>::lowest());
    }
}

template <class T> CoordinateLength<T> coordinate_difference(T l, T r) {
    assert(l <= r);
    return static_cast<CoordinateLength<T>>(r) -
           static_cast<CoordinateLength<T>>(l);
}

template <class Length> int checked_size(Length n) {
    assert(n <= static_cast<Length>(std::numeric_limits<int>::max()));
    return static_cast<int>(n);
}

template <class T> T add_length(T x, int n) {
    return static_cast<T>(x + static_cast<T>(n));
}

template <class T, class C> struct SegmentTree {
    using Length = CoordinateLength<T>;

    struct Node {
        C max_value;
        Length length;
        T minimum_y;
        T maximum_y;
    };

    int size = 1;
    std::vector<Node> data;
    std::vector<C> lazy;

    SegmentTree() = default;

    explicit SegmentTree(const std::vector<Node> &leaves) {
        const int n = static_cast<int>(leaves.size());
        assert(n > 0);
        while (size < n) {
            size <<= 1;
        }
        data.assign(size << 1, Node{C(), Length(), T(), T()});
        lazy.assign(size << 1, C());
        for (int i = 0; i < n; ++i) {
            data[size + i] = leaves[i];
        }
        for (int i = size - 1; i >= 1; --i) {
            data[i] = merge(data[i << 1], data[i << 1 | 1]);
        }
    }

    static Node merge(const Node &a, const Node &b) {
        if (a.length == Length()) {
            return b;
        }
        if (b.length == Length()) {
            return a;
        }
        if (a.max_value > b.max_value) {
            return a;
        }
        if (a.max_value < b.max_value) {
            return b;
        }
        return Node{a.max_value, a.length + b.length, a.minimum_y, b.maximum_y};
    }

    void apply(int l, int r, C w, bool add) { apply(1, 0, size, l, r, w, add); }

    const Node &all_prod() const { return data[1]; }

    Node prod(int l, int r) { return prod(1, 0, size, l, r); }

    void add_node(int v, C w, bool add) {
        if (add) {
            data[v].max_value += w;
            lazy[v] += w;
        } else {
            data[v].max_value -= w;
            lazy[v] -= w;
        }
    }

    void push(int v) {
        if (lazy[v] == C()) {
            return;
        }
        add_node(v << 1, lazy[v], true);
        add_node(v << 1 | 1, lazy[v], true);
        lazy[v] = C();
    }

    void apply(int v, int l, int r, int ql, int qr, C w, bool add) {
        if (qr <= l || r <= ql) {
            return;
        }
        if (ql <= l && r <= qr) {
            add_node(v, w, add);
            return;
        }
        push(v);
        const int m = (l + r) >> 1;
        apply(v << 1, l, m, ql, qr, w, add);
        apply(v << 1 | 1, m, r, ql, qr, w, add);
        data[v] = merge(data[v << 1], data[v << 1 | 1]);
    }

    Node prod(int v, int l, int r, int ql, int qr) {
        if (qr <= l || r <= ql) {
            return Node{C(), Length(), T(), T()};
        }
        if (ql <= l && r <= qr) {
            return data[v];
        }
        push(v);
        const int m = (l + r) >> 1;
        return merge(prod(v << 1, l, m, ql, qr),
                     prod(v << 1 | 1, m, r, ql, qr));
    }
};

} // namespace rectangle_add_max_get_internal

template <class T, class C> struct CompressedRectangleAddMaxGet {
    static_assert(std::is_integral_v<T> && !std::is_same_v<T, bool>,
                  "T must be integer.");

    using Rectangle = rectangle_add_max_get_internal::Rectangle<T, C>;

    std::vector<Rectangle> rectangles;

    CompressedRectangleAddMaxGet() = default;

    explicit CompressedRectangleAddMaxGet(int n) {
        assert(n >= 0);
        rectangles.reserve(n);
    }

    void add_rectangle(T l, T d, T r, T u, C w = C(1)) {
        assert(l <= r);
        assert(d <= u);
        if (l == r || d == u) {
            return;
        }
        rectangle_add_max_get_internal::assert_valid_weight(w);
        rectangles.emplace_back(Rectangle{l, d, r, u, w});
    }

    std::tuple<C, T, T> calc_max_lexicographically_minimum_point() const {
        const auto query = make_bounding_query_range();
        if (query.empty) {
            return {C(), T(), T()};
        }
        const auto result = calc_impl<T, false>(query);
        return {result.max_value, result.minimum_x, result.minimum_y};
    }

    std::tuple<C, T, T> calc_max_lexicographically_minimum_point(T l, T d, T r,
                                                                 T u) const {
        const auto query = make_query_range(l, d, r, u);
        const auto result = calc_impl<T, false>(query);
        return {result.max_value, result.minimum_x, result.minimum_y};
    }

    std::tuple<C, T, T> calc_max_lexicographically_maximum_point() const {
        const auto query = make_bounding_query_range();
        if (query.empty) {
            return {C(), T(), T()};
        }
        const auto result = calc_impl<T, false>(query);
        return {result.max_value, result.maximum_x, result.maximum_y};
    }

    std::tuple<C, T, T> calc_max_lexicographically_maximum_point(T l, T d, T r,
                                                                 T u) const {
        const auto query = make_query_range(l, d, r, u);
        const auto result = calc_impl<T, false>(query);
        return {result.max_value, result.maximum_x, result.maximum_y};
    }

    template <class T2 = T> std::pair<C, T2> calc_max_area() const {
        const auto query = make_bounding_query_range();
        if (query.empty) {
            return {C(), T2()};
        }
        const auto result = calc_impl<T2, true>(query);
        return {result.max_value, result.max_area};
    }

    template <class T2 = T>
    std::pair<C, T2> calc_max_area(T l, T d, T r, T u) const {
        const auto query = make_query_range(l, d, r, u);
        const auto result = calc_impl<T2, true>(query);
        return {result.max_value, result.max_area};
    }

  private:
    using SegmentTree = rectangle_add_max_get_internal::SegmentTree<T, C>;
    using Node = typename SegmentTree::Node;

    struct QueryRange {
        T l, d, r, u;
        bool empty;
    };

    struct Event {
        T x;
        int d, u;
        C w;
        bool add;
    };

    template <class T2> struct Result {
        C max_value{};
        T minimum_x{};
        T minimum_y{};
        T maximum_x{};
        T maximum_y{};
        T2 max_area{};
    };

    QueryRange make_bounding_query_range() const {
        if (rectangles.empty()) {
            return QueryRange{T(), T(), T(), T(), true};
        }
        T l = rectangles[0].l;
        T d = rectangles[0].d;
        T r = rectangles[0].r;
        T u = rectangles[0].u;
        for (const auto &rect : rectangles) {
            l = std::min(l, rect.l);
            d = std::min(d, rect.d);
            r = std::max(r, rect.r);
            u = std::max(u, rect.u);
        }
        return QueryRange{l, d, r, u, false};
    }

    QueryRange make_query_range(T l, T d, T r, T u) const {
        assert(l < r);
        assert(d < u);
        return QueryRange{l, d, r, u, false};
    }

    template <class T2, bool NeedArea>
    Result<T2> calc_impl(const QueryRange &query) const {
        std::vector<T> xs{query.l, query.r};
        std::vector<T> ys{query.d, query.u};
        std::vector<Rectangle> clipped;
        clipped.reserve(rectangles.size());
        for (const auto &rect : rectangles) {
            const T l = std::max(rect.l, query.l);
            const T d = std::max(rect.d, query.d);
            const T r = std::min(rect.r, query.r);
            const T u = std::min(rect.u, query.u);
            if (l >= r || d >= u) {
                continue;
            }
            clipped.emplace_back(Rectangle{l, d, r, u, rect.w});
            xs.emplace_back(l);
            xs.emplace_back(r);
            ys.emplace_back(d);
            ys.emplace_back(u);
        }
        std::sort(xs.begin(), xs.end());
        xs.erase(std::unique(xs.begin(), xs.end()), xs.end());
        std::sort(ys.begin(), ys.end());
        ys.erase(std::unique(ys.begin(), ys.end()), ys.end());

        std::vector<Event> events;
        events.reserve(clipped.size() * 2);
        for (const auto &rect : clipped) {
            const int d = static_cast<int>(
                std::lower_bound(ys.begin(), ys.end(), rect.d) - ys.begin());
            const int u = static_cast<int>(
                std::lower_bound(ys.begin(), ys.end(), rect.u) - ys.begin());
            events.emplace_back(Event{rect.l, d, u, rect.w, true});
            events.emplace_back(Event{rect.r, d, u, rect.w, false});
        }
        std::sort(events.begin(), events.end(),
                  [](const Event &a, const Event &b) {
                      if (a.x != b.x) {
                          return a.x < b.x;
                      }
                      return a.add < b.add;
                  });

        std::vector<Node> leaves;
        leaves.reserve(ys.size() - 1);
        for (int i = 0; i + 1 < static_cast<int>(ys.size()); ++i) {
            leaves.emplace_back(
                Node{C(),
                     rectangle_add_max_get_internal::coordinate_difference(
                         ys[i], ys[i + 1]),
                     ys[i], static_cast<T>(ys[i + 1] - T(1))});
        }
        SegmentTree seg(leaves);

        Result<T2> ret;
        bool found = false;
        int event_index = 0;
        for (int i = 0; i + 1 < static_cast<int>(xs.size()); ++i) {
            while (event_index < static_cast<int>(events.size()) &&
                   events[event_index].x == xs[i]) {
                seg.apply(events[event_index].d, events[event_index].u,
                          events[event_index].w, events[event_index].add);
                ++event_index;
            }
            const Node &now = seg.all_prod();
            const T minimum_x = xs[i];
            const T maximum_x = static_cast<T>(xs[i + 1] - T(1));
            T2 area = T2();
            if constexpr (NeedArea) {
                const auto dx =
                    rectangle_add_max_get_internal::coordinate_difference(
                        xs[i], xs[i + 1]);
                area = static_cast<T2>(dx) * static_cast<T2>(now.length);
            }
            if (!found || ret.max_value < now.max_value) {
                ret.max_value = now.max_value;
                ret.minimum_x = minimum_x;
                ret.minimum_y = now.minimum_y;
                ret.maximum_x = maximum_x;
                ret.maximum_y = now.maximum_y;
                if constexpr (NeedArea) {
                    ret.max_area = area;
                }
                found = true;
            } else if (ret.max_value == now.max_value) {
                if (std::make_pair(minimum_x, now.minimum_y) <
                    std::make_pair(ret.minimum_x, ret.minimum_y)) {
                    ret.minimum_x = minimum_x;
                    ret.minimum_y = now.minimum_y;
                }
                if (std::make_pair(ret.maximum_x, ret.maximum_y) <
                    std::make_pair(maximum_x, now.maximum_y)) {
                    ret.maximum_x = maximum_x;
                    ret.maximum_y = now.maximum_y;
                }
                if constexpr (NeedArea) {
                    ret.max_area += area;
                }
            }
        }
        return ret;
    }
};

template <class T, class C> struct RectangleAddMaxGet {
    static_assert(std::is_integral_v<T> && !std::is_same_v<T, bool>,
                  "T must be integer.");

    using Rectangle = rectangle_add_max_get_internal::Rectangle<T, C>;

    std::vector<Rectangle> rectangles;

    RectangleAddMaxGet() = default;

    explicit RectangleAddMaxGet(int n) {
        assert(n >= 0);
        rectangles.reserve(n);
    }

    void add_rectangle(T l, T d, T r, T u, C w = C(1)) {
        assert(l <= r);
        assert(d <= u);
        if (l == r || d == u) {
            return;
        }
        rectangle_add_max_get_internal::assert_valid_weight(w);
        rectangles.emplace_back(Rectangle{l, d, r, u, w});
    }

    std::tuple<C, T, T> calc_max_lexicographically_minimum_point() const {
        const auto query = make_bounding_query_range();
        if (query.empty) {
            return {C(), T(), T()};
        }
        const auto result = calc_rectangle_impl<T, false>(query);
        return {result.max_value, result.minimum_x, result.minimum_y};
    }

    std::tuple<C, T, T> calc_max_lexicographically_minimum_point(T l, T d, T r,
                                                                 T u) const {
        const auto query = make_query_range(l, d, r, u);
        const auto result = calc_rectangle_impl<T, false>(query);
        return {result.max_value, result.minimum_x, result.minimum_y};
    }

    template <class Lower, class Upper>
    std::tuple<C, T, T>
    calc_max_lexicographically_minimum_point(T l, T r, Lower lower_y,
                                             Upper upper_y) const {
        const auto query = make_variable_query_range(l, r, lower_y, upper_y);
        const auto result = calc_variable_impl<T, false>(query);
        return {result.max_value, result.minimum_x, result.minimum_y};
    }

    std::tuple<C, T, T> calc_max_lexicographically_maximum_point() const {
        const auto query = make_bounding_query_range();
        if (query.empty) {
            return {C(), T(), T()};
        }
        const auto result = calc_rectangle_impl<T, false>(query);
        return {result.max_value, result.maximum_x, result.maximum_y};
    }

    std::tuple<C, T, T> calc_max_lexicographically_maximum_point(T l, T d, T r,
                                                                 T u) const {
        const auto query = make_query_range(l, d, r, u);
        const auto result = calc_rectangle_impl<T, false>(query);
        return {result.max_value, result.maximum_x, result.maximum_y};
    }

    template <class Lower, class Upper>
    std::tuple<C, T, T>
    calc_max_lexicographically_maximum_point(T l, T r, Lower lower_y,
                                             Upper upper_y) const {
        const auto query = make_variable_query_range(l, r, lower_y, upper_y);
        const auto result = calc_variable_impl<T, false>(query);
        return {result.max_value, result.maximum_x, result.maximum_y};
    }

    template <class T2 = T> std::pair<C, T2> calc_max_area() const {
        const auto query = make_bounding_query_range();
        if (query.empty) {
            return {C(), T2()};
        }
        const auto result = calc_rectangle_impl<T2, true>(query);
        return {result.max_value, result.max_area};
    }

    template <class T2 = T>
    std::pair<C, T2> calc_max_area(T l, T d, T r, T u) const {
        const auto query = make_query_range(l, d, r, u);
        const auto result = calc_rectangle_impl<T2, true>(query);
        return {result.max_value, result.max_area};
    }

    template <class T2 = T, class Lower, class Upper>
    std::pair<C, T2> calc_max_area(T l, T r, Lower lower_y,
                                   Upper upper_y) const {
        const auto query = make_variable_query_range(l, r, lower_y, upper_y);
        const auto result = calc_variable_impl<T2, true>(query);
        return {result.max_value, result.max_area};
    }

  private:
    using Length = rectangle_add_max_get_internal::CoordinateLength<T>;
    using SegmentTree = rectangle_add_max_get_internal::SegmentTree<T, C>;
    using Node = typename SegmentTree::Node;

    struct QueryRange {
        T l, d, r, u;
        bool empty;
    };

    struct VariableQueryRange {
        T l, r, d, u;
        std::vector<T> lower_y, upper_y;
    };

    struct Event {
        int next;
        int d, u;
        C w;
    };

    struct EventList {
        std::vector<int> add_head, remove_head;
        std::vector<Event> events;
    };

    template <class T2> struct Result {
        C max_value{};
        T minimum_x{};
        T minimum_y{};
        T maximum_x{};
        T maximum_y{};
        T2 max_area{};
    };

    QueryRange make_bounding_query_range() const {
        if (rectangles.empty()) {
            return QueryRange{T(), T(), T(), T(), true};
        }
        T l = rectangles[0].l;
        T d = rectangles[0].d;
        T r = rectangles[0].r;
        T u = rectangles[0].u;
        for (const auto &rect : rectangles) {
            l = std::min(l, rect.l);
            d = std::min(d, rect.d);
            r = std::max(r, rect.r);
            u = std::max(u, rect.u);
        }
        return QueryRange{l, d, r, u, false};
    }

    QueryRange make_query_range(T l, T d, T r, T u) const {
        assert(l < r);
        assert(d < u);
        return QueryRange{l, d, r, u, false};
    }

    template <class Lower, class Upper>
    VariableQueryRange make_variable_query_range(T l, T r, Lower lower_y,
                                                 Upper upper_y) const {
        assert(l < r);
        const int x_count = rectangle_add_max_get_internal::checked_size(
            rectangle_add_max_get_internal::coordinate_difference(l, r));
        std::vector<T> lower_values(x_count), upper_values(x_count);
        bool found = false;
        T d = T(), u = T();
        for (int i = 0; i < x_count; ++i) {
            const T x = rectangle_add_max_get_internal::add_length(l, i);
            const T lower = static_cast<T>(lower_y(x));
            const T upper = static_cast<T>(upper_y(x));
            assert(lower <= upper);
            lower_values[i] = lower;
            upper_values[i] = upper;
            if (lower == upper) {
                continue;
            }
            if (!found) {
                d = lower;
                u = upper;
                found = true;
            } else {
                d = std::min(d, lower);
                u = std::max(u, upper);
            }
        }
        assert(found);
        return VariableQueryRange{
            l, r, d, u, std::move(lower_values), std::move(upper_values)};
    }

    static void add_event(std::vector<int> &head, std::vector<Event> &events,
                          int x, int d, int u, C w) {
        events.emplace_back(Event{head[x], d, u, w});
        head[x] = static_cast<int>(events.size()) - 1;
    }

    EventList make_event_list(T l, T d, T r, T u, int x_count) const {
        EventList event_list;
        event_list.add_head.assign(x_count, -1);
        event_list.remove_head.assign(x_count, -1);
        event_list.events.reserve(rectangles.size() * 2);
        for (const auto &rect : rectangles) {
            const T cl = std::max(rect.l, l);
            const T cd = std::max(rect.d, d);
            const T cr = std::min(rect.r, r);
            const T cu = std::min(rect.u, u);
            if (cl >= cr || cd >= cu) {
                continue;
            }
            const int xl = rectangle_add_max_get_internal::checked_size(
                rectangle_add_max_get_internal::coordinate_difference(l, cl));
            const int xr = rectangle_add_max_get_internal::checked_size(
                rectangle_add_max_get_internal::coordinate_difference(l, cr));
            const int yd = rectangle_add_max_get_internal::checked_size(
                rectangle_add_max_get_internal::coordinate_difference(d, cd));
            const int yu = rectangle_add_max_get_internal::checked_size(
                rectangle_add_max_get_internal::coordinate_difference(d, cu));
            add_event(event_list.add_head, event_list.events, xl, yd, yu,
                      rect.w);
            if (xr < x_count) {
                add_event(event_list.remove_head, event_list.events, xr, yd, yu,
                          rect.w);
            }
        }
        return event_list;
    }

    static std::vector<Node> make_unit_leaves(T d, int y_count) {
        std::vector<Node> leaves;
        leaves.reserve(y_count);
        for (int i = 0; i < y_count; ++i) {
            const T y = rectangle_add_max_get_internal::add_length(d, i);
            leaves.emplace_back(Node{C(), Length(1), y, y});
        }
        return leaves;
    }

    static void apply_events(SegmentTree &seg, const EventList &event_list,
                             int x) {
        for (int i = event_list.remove_head[x]; i != -1;
             i = event_list.events[i].next) {
            seg.apply(event_list.events[i].d, event_list.events[i].u,
                      event_list.events[i].w, false);
        }
        for (int i = event_list.add_head[x]; i != -1;
             i = event_list.events[i].next) {
            seg.apply(event_list.events[i].d, event_list.events[i].u,
                      event_list.events[i].w, true);
        }
    }

    template <class T2, bool NeedArea>
    static void update_result(Result<T2> &ret, bool &found, C value,
                              T minimum_x, T minimum_y, T maximum_x,
                              T maximum_y, T2 area) {
        if (!found || ret.max_value < value) {
            ret.max_value = value;
            ret.minimum_x = minimum_x;
            ret.minimum_y = minimum_y;
            ret.maximum_x = maximum_x;
            ret.maximum_y = maximum_y;
            if constexpr (NeedArea) {
                ret.max_area = area;
            }
            found = true;
        } else if (ret.max_value == value) {
            if (std::make_pair(minimum_x, minimum_y) <
                std::make_pair(ret.minimum_x, ret.minimum_y)) {
                ret.minimum_x = minimum_x;
                ret.minimum_y = minimum_y;
            }
            if (std::make_pair(ret.maximum_x, ret.maximum_y) <
                std::make_pair(maximum_x, maximum_y)) {
                ret.maximum_x = maximum_x;
                ret.maximum_y = maximum_y;
            }
            if constexpr (NeedArea) {
                ret.max_area += area;
            }
        }
    }

    template <class T2, bool NeedArea>
    Result<T2> calc_rectangle_impl(const QueryRange &query) const {
        const int x_count = rectangle_add_max_get_internal::checked_size(
            rectangle_add_max_get_internal::coordinate_difference(query.l,
                                                                  query.r));
        const int y_count = rectangle_add_max_get_internal::checked_size(
            rectangle_add_max_get_internal::coordinate_difference(query.d,
                                                                  query.u));
        SegmentTree seg(make_unit_leaves(query.d, y_count));
        const EventList event_list =
            make_event_list(query.l, query.d, query.r, query.u, x_count);

        Result<T2> ret;
        bool found = false;
        for (int i = 0; i < x_count; ++i) {
            apply_events(seg, event_list, i);
            const Node &now = seg.all_prod();
            const T x = rectangle_add_max_get_internal::add_length(query.l, i);
            T2 area = T2();
            if constexpr (NeedArea) {
                area = static_cast<T2>(now.length);
            }
            update_result<T2, NeedArea>(ret, found, now.max_value, x,
                                        now.minimum_y, x, now.maximum_y, area);
        }
        assert(found);
        return ret;
    }

    template <class T2, bool NeedArea>
    Result<T2> calc_variable_impl(const VariableQueryRange &query) const {
        const int x_count = static_cast<int>(query.lower_y.size());
        const int y_count = rectangle_add_max_get_internal::checked_size(
            rectangle_add_max_get_internal::coordinate_difference(query.d,
                                                                  query.u));
        SegmentTree seg(make_unit_leaves(query.d, y_count));
        const EventList event_list =
            make_event_list(query.l, query.d, query.r, query.u, x_count);

        Result<T2> ret;
        bool found = false;
        for (int i = 0; i < x_count; ++i) {
            apply_events(seg, event_list, i);
            if (query.lower_y[i] == query.upper_y[i]) {
                continue;
            }
            const int d = rectangle_add_max_get_internal::checked_size(
                rectangle_add_max_get_internal::coordinate_difference(
                    query.d, query.lower_y[i]));
            const int u = rectangle_add_max_get_internal::checked_size(
                rectangle_add_max_get_internal::coordinate_difference(
                    query.d, query.upper_y[i]));
            const Node now = seg.prod(d, u);
            const T x = rectangle_add_max_get_internal::add_length(query.l, i);
            T2 area = T2();
            if constexpr (NeedArea) {
                area = static_cast<T2>(now.length);
            }
            update_result<T2, NeedArea>(ret, found, now.max_value, x,
                                        now.minimum_y, x, now.maximum_y, area);
        }
        assert(found);
        return ret;
    }
};

#endif
