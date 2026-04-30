#ifndef OTHER_RECTANGLE_ADD_MAX_GET_HPP
#define OTHER_RECTANGLE_ADD_MAX_GET_HPP

// 2 次元平面上の重み付き半開長方形を加算し、最大値を求める。
// 座標は整数型とし、最大座標は整数格子点として扱う。
// calc の指定範囲に各長方形を切り詰めて平面走査する。
// 重みは負でもよい。
// 計算量 O(N log N)。

#include <algorithm>
#include <cassert>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

template <class T, class C> struct RectangleAddMaxGet {
    static_assert(std::is_integral_v<T>, "T must be integer.");

    struct Rectangle {
        T l, d, r, u;
        C w;
    };

    std::vector<Rectangle> rectangles;

    RectangleAddMaxGet() = default;

    explicit RectangleAddMaxGet(int n) {
        assert(n >= 0);
        rectangles.reserve(n);
    }

    void add_rectangle(T l, T d, T r, T u, C w = C(1)) {
        assert(l < r);
        assert(d < u);
        rectangles.emplace_back(Rectangle{l, d, r, u, w});
    }

    std::tuple<C, T, T> calc_max_lexicographically_minimum_point(
        T l = std::numeric_limits<T>::lowest(),
        T d = std::numeric_limits<T>::lowest(),
        T r = std::numeric_limits<T>::max(),
        T u = std::numeric_limits<T>::max()) const {
        const auto query = make_query_range(l, d, r, u);
        if (query.empty)
            return {C(), T(), T()};
        const auto result = calc_impl<T, false>(query);
        return {result.max_value, result.minimum_x, result.minimum_y};
    }

    std::tuple<C, T, T> calc_max_lexicographically_maximum_point(
        T l = std::numeric_limits<T>::lowest(),
        T d = std::numeric_limits<T>::lowest(),
        T r = std::numeric_limits<T>::max(),
        T u = std::numeric_limits<T>::max()) const {
        const auto query = make_query_range(l, d, r, u);
        if (query.empty)
            return {C(), T(), T()};
        const auto result = calc_impl<T, false>(query);
        return {result.max_value, result.maximum_x, result.maximum_y};
    }

    template <class T2 = T>
    std::pair<C, T2> calc_max_area(T l = std::numeric_limits<T>::lowest(),
                                   T d = std::numeric_limits<T>::lowest(),
                                   T r = std::numeric_limits<T>::max(),
                                   T u = std::numeric_limits<T>::max()) const {
        const auto query = make_query_range(l, d, r, u);
        if (query.empty)
            return {C(), T2()};
        const auto result = calc_impl<T2, true>(query);
        return {result.max_value, result.max_area};
    }

  private:
    struct QueryRange {
        T l, d, r, u;
        bool empty;
    };

    struct Event {
        T x;
        int d, u;
        C w;
    };

    struct Node {
        C max_value;
        T length;
        T minimum_y;
        T maximum_y;
    };

    template <class T2> struct Result {
        C max_value{};
        T minimum_x{};
        T minimum_y{};
        T maximum_x{};
        T maximum_y{};
        T2 max_area{};
    };

    struct SegmentTree {
        int size = 1;
        std::vector<Node> data;
        std::vector<C> lazy;

        SegmentTree() = default;

        explicit SegmentTree(const std::vector<Node> &leaves) {
            const int n = static_cast<int>(leaves.size());
            while (size < n)
                size <<= 1;
            data.assign(size << 1, Node{C(), T(), T(), T()});
            lazy.assign(size << 1, C());
            for (int i = 0; i < n; ++i)
                data[size + i] = leaves[i];
            for (int i = size - 1; i >= 1; --i)
                data[i] = merge(data[i << 1], data[i << 1 | 1]);
        }

        static Node merge(const Node &a, const Node &b) {
            if (a.length == T())
                return b;
            if (b.length == T())
                return a;
            if (a.max_value > b.max_value)
                return a;
            if (a.max_value < b.max_value)
                return b;
            return Node{a.max_value, a.length + b.length, a.minimum_y,
                        b.maximum_y};
        }

        void apply(int l, int r, C w) { apply(1, 0, size, l, r, w); }

        const Node &all_prod() const { return data[1]; }

        void add_node(int v, C w) {
            data[v].max_value += w;
            lazy[v] += w;
        }

        void push(int v) {
            if (lazy[v] == C())
                return;
            add_node(v << 1, lazy[v]);
            add_node(v << 1 | 1, lazy[v]);
            lazy[v] = C();
        }

        void apply(int v, int l, int r, int ql, int qr, C w) {
            if (qr <= l || r <= ql)
                return;
            if (ql <= l && r <= qr) {
                add_node(v, w);
                return;
            }
            push(v);
            const int m = (l + r) >> 1;
            apply(v << 1, l, m, ql, qr, w);
            apply(v << 1 | 1, m, r, ql, qr, w);
            data[v] = merge(data[v << 1], data[v << 1 | 1]);
        }
    };

    static constexpr T default_l() { return std::numeric_limits<T>::lowest(); }
    static constexpr T default_r() { return std::numeric_limits<T>::max(); }

    QueryRange make_query_range(T l, T d, T r, T u) const {
        const bool use_all = l == default_l() && d == default_l() &&
                             r == default_r() && u == default_r();
        if (!use_all) {
            assert(l < r);
            assert(d < u);
            return QueryRange{l, d, r, u, false};
        }
        if (rectangles.empty())
            return QueryRange{T(), T(), T(), T(), true};
        l = rectangles[0].l;
        d = rectangles[0].d;
        r = rectangles[0].r;
        u = rectangles[0].u;
        for (const auto &rect : rectangles) {
            l = std::min(l, rect.l);
            d = std::min(d, rect.d);
            r = std::max(r, rect.r);
            u = std::max(u, rect.u);
        }
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
            if (l >= r || d >= u)
                continue;
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
            events.emplace_back(Event{rect.l, d, u, rect.w});
            events.emplace_back(Event{rect.r, d, u, -rect.w});
        }
        std::sort(events.begin(), events.end(),
                  [](const Event &a, const Event &b) { return a.x < b.x; });

        std::vector<Node> leaves;
        leaves.reserve(ys.size() - 1);
        for (int i = 0; i + 1 < static_cast<int>(ys.size()); ++i) {
            leaves.emplace_back(Node{C(), ys[i + 1] - ys[i], ys[i],
                                     static_cast<T>(ys[i + 1] - T(1))});
        }
        SegmentTree seg(leaves);

        Result<T2> ret;
        bool found = false;
        int event_index = 0;
        for (int i = 0; i + 1 < static_cast<int>(xs.size()); ++i) {
            while (event_index < static_cast<int>(events.size()) &&
                   events[event_index].x == xs[i]) {
                seg.apply(events[event_index].d, events[event_index].u,
                          events[event_index].w);
                ++event_index;
            }
            const Node &now = seg.all_prod();
            const T minimum_x = xs[i];
            const T maximum_x = static_cast<T>(xs[i + 1] - T(1));
            const T dx = xs[i + 1] - xs[i];
            T2 area = T2();
            if constexpr (NeedArea)
                area = static_cast<T2>(dx) * static_cast<T2>(now.length);
            if (!found || ret.max_value < now.max_value) {
                ret.max_value = now.max_value;
                ret.minimum_x = minimum_x;
                ret.minimum_y = now.minimum_y;
                ret.maximum_x = maximum_x;
                ret.maximum_y = now.maximum_y;
                if constexpr (NeedArea)
                    ret.max_area = area;
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
                if constexpr (NeedArea)
                    ret.max_area += area;
            }
        }
        return ret;
    }
};

#endif
