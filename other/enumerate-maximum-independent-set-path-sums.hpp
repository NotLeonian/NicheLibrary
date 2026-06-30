#ifndef OTHER_ENUMERATE_MAXIMUM_INDEPENDENT_SET_PATH_SUMS_HPP
#define OTHER_ENUMERATE_MAXIMUM_INDEPENDENT_SET_PATH_SUMS_HPP

// 長さ n の列から隣り合わない k 個を選ぶ総和の最大値を全ての k について求める。
// 符号なし整数型は使用できない。
// 比較ソートによる実装は O(n log n)、
// バケットソートによる実装は非負整数列の総和を S として O(n + S)。

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>
#include <vector>

namespace enumerate_maximum_independent_set_path_sums_internal {
template <class T>
bool leq(const std::pair<bool, T> &x, const std::pair<bool, T> &y) {
    if (!x.first) {
        return true;
    }
    if (!y.first) {
        return false;
    }
    return !(y.second < x.second);
}

template <class T> std::vector<T> marginal_values(const std::vector<T> &a) {
    using P = std::pair<bool, T>;
    const P none(false, T());
    std::vector<P> stack;
    std::vector<T> res;
    stack.reserve(a.size() + 2);
    res.reserve((a.size() + 1) / 2);
    stack.push_back(none);

    const auto add = [&](P x) {
        while (stack.size() >= 2 && stack.back().first &&
               leq(x, stack.back()) &&
               leq(stack[stack.size() - 2], stack.back())) {
            const P l = stack[stack.size() - 2];
            const P p = stack.back();
            res.push_back(p.second);
            stack.pop_back();
            stack.pop_back();
            x = l.first && x.first ? P(true, l.second - p.second + x.second)
                                   : none;
        }
        stack.push_back(x);
    };

    for (const T &x : a) {
        add(P(true, x));
    }
    add(none);
    assert(res.size() == (a.size() + 1) / 2);
    return res;
}

template <class T> std::size_t bucket_index(const T x, const T offset) {
    assert(-offset <= x && x <= offset);
    const std::size_t base = static_cast<std::size_t>(offset);
    if (T(0) <= x) {
        return base + static_cast<std::size_t>(x);
    }
    return base - static_cast<std::size_t>(T(0) - x);
}
} // namespace enumerate_maximum_independent_set_path_sums_internal

template <class T>
std::vector<T>
enumerate_maximum_independent_set_path_sums(const std::vector<T> &a) {
    static_assert(!std::is_unsigned_v<T>,
                  "T must not be an unsigned type in "
                  "enumerate_maximum_independent_set_path_sums.");

    std::vector<T> xs =
        enumerate_maximum_independent_set_path_sums_internal::marginal_values(
            a);
    std::sort(xs.begin(), xs.end(),
              [](const T &x, const T &y) { return y < x; });

    std::vector<T> res(1, T());
    res.reserve(xs.size() + 1);
    T sum = T();
    for (const T &x : xs) {
        sum += x;
        res.push_back(sum);
    }
    return res;
}

template <class T>
std::vector<T> enumerate_maximum_independent_set_path_sums_bucket_sort(
    const std::vector<T> &a) {
    static_assert(!std::is_unsigned_v<T>,
                  "T must not be an unsigned type in "
                  "enumerate_maximum_independent_set_path_sums_bucket_sort.");
    static_assert(
        std::is_integral_v<T> && std::is_signed_v<T> &&
            !std::is_same_v<T, bool> && sizeof(T) <= sizeof(std::int32_t),
        "T must be a non-bool signed integral type of at most 32 bits in "
        "enumerate_maximum_independent_set_path_sums_bucket_sort.");

    T total = T();
    for (const T &x : a) {
        assert(T(0) <= x);
        assert(x <= std::numeric_limits<T>::max() - total);
        total += x;
    }

    const std::vector<T> xs =
        enumerate_maximum_independent_set_path_sums_internal::marginal_values(
            a);
    const std::size_t offset = static_cast<std::size_t>(total);
    assert(offset <= (std::numeric_limits<std::size_t>::max() - 1) / 2);
    std::vector<std::size_t> count(offset * 2 + 1, 0);
    for (const T &x : xs) {
        count[enumerate_maximum_independent_set_path_sums_internal::
                  bucket_index(x, total)] += 1;
    }

    std::vector<T> res(1, T());
    res.reserve(xs.size() + 1);
    T sum = T();
    for (T x = total;; x -= T(1)) {
        const std::size_t c =
            count[enumerate_maximum_independent_set_path_sums_internal::
                      bucket_index(x, total)];
        for (std::size_t i = 0; i < c; i += 1) {
            sum += x;
            res.push_back(sum);
        }
        if (x == -total) {
            break;
        }
    }
    return res;
}

#endif
