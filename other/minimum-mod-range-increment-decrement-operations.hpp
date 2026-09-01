#ifndef OTHER_MINIMUM_MOD_RANGE_INCREMENT_DECREMENT_OPERATIONS_HPP
#define OTHER_MINIMUM_MOD_RANGE_INCREMENT_DECREMENT_OPERATIONS_HPP

// 法 m で数列 a を b に一致させるために、
// 連続区間の全要素に +1 または -1 を加える操作の最小回数を求める。
// 連続区間に任意の整数 x を加える操作を繰り返す場合の、各 |x| の総和の最小値と等しい。
// a, b は同じ長さで、各要素は [0, m) に属すると仮定する。
// T は 64 bit 以上の整数型であることを仮定する。
// 入力列の長さを N とすると、平均時間計算量は O(N) である。

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <type_traits>
#include <vector>

template <class T>
T minimum_mod_range_increment_decrement_operations(const std::vector<T> &a,
                                                   const std::vector<T> &b,
                                                   T m) {
    static_assert(std::is_integral_v<T>, "T must be integer.");
    static_assert(sizeof(T) >= sizeof(long long),
                  "T must be at least 64-bit integer type.");

    assert(a.size() == b.size());
    assert(m > 0);

    const std::size_t n = a.size();
    std::vector<T> differences;
    differences.reserve(n + 1);

    auto mod_difference = [m](T from, T to) -> T {
        return from <= to ? to - from : m - (from - to);
    };

    auto add_mod_difference = [m](std::size_t &quotient, T &remainder, T diff) {
        if (remainder >= m - diff) {
            remainder -= m - diff;
            ++quotient;
        } else {
            remainder += diff;
        }
    };

    T previous = 0;
    std::size_t negative_count = 0;
    T remainder = 0;
    for (std::size_t i = 0; i < n; ++i) {
        if constexpr (std::is_signed_v<T>) {
            assert(0 <= a[i]);
            assert(0 <= b[i]);
        }
        assert(a[i] < m);
        assert(b[i] < m);

        const T current = mod_difference(a[i], b[i]);
        const T diff = mod_difference(previous, current);
        differences.emplace_back(diff);
        add_mod_difference(negative_count, remainder, diff);
        previous = current;
    }

    const T last = mod_difference(previous, T(0));
    differences.emplace_back(last);
    add_mod_difference(negative_count, remainder, last);

    assert(remainder == 0);
    assert(negative_count <= n + 1);

    const std::size_t keep_count = n + 1 - negative_count;
    if (0 < keep_count && keep_count < differences.size()) {
        std::nth_element(differences.begin(),
                         differences.begin() +
                             static_cast<std::ptrdiff_t>(keep_count),
                         differences.end());
    }
    T answer = 0;
    for (std::size_t i = 0; i < keep_count; ++i) {
        answer += differences[i];
    }
    return answer;
}

#endif
