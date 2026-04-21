#ifndef OTHER_MINIMUM_MOD_RANGE_INCREMENT_DECREMENT_OPERATIONS_HPP
#define OTHER_MINIMUM_MOD_RANGE_INCREMENT_DECREMENT_OPERATIONS_HPP

// 数列を mod m 上で見たとき、区間に +1 または -1 を加える操作の最小回数を求める。
// 区間に整数 x を加える操作の最小 \sum |x| と同値である。
// a, b は同じ長さで、各要素は [0, m) を仮定する。
// 差分を [0, m) で取り、何個を負側に回すかを貪欲に決めればよい。
// 計算量 O(N log N)。

#include <algorithm>
#include <cassert>
#include <type_traits>
#include <vector>

template <class T>
T minimum_mod_range_increment_decrement_operations(std::vector<T> a,
                                                   std::vector<T> b, T m) {
    static_assert(std::is_integral_v<T>, "T must be integer.");
    static_assert(sizeof(T) >= sizeof(long long),
                  "T must be at least 64-bit integer type.");

    assert(a.size() == b.size());
    assert(m > 0);

    const int n = static_cast<int>(a.size());
    std::vector<T> differences;
    differences.reserve(n + 1);

    auto mod_difference = [m](T from, T to) -> T {
        return from <= to ? to - from : to + m - from;
    };

    T previous = 0;
    T total_difference = 0;
    for (int i = 0; i < n; ++i) {
        if constexpr (std::is_signed_v<T>) {
            assert(0 <= a[i]);
            assert(0 <= b[i]);
        }
        assert(a[i] < m);
        assert(b[i] < m);

        const T current = mod_difference(a[i], b[i]);
        const T diff = mod_difference(previous, current);
        differences.emplace_back(diff);
        total_difference += diff;
        previous = current;
    }

    const T last = mod_difference(previous, T(0));
    differences.emplace_back(last);
    total_difference += last;

    assert(total_difference % m == 0);
    const int negative_count = static_cast<int>(total_difference / m);
    assert(negative_count <= n + 1);

    std::sort(differences.begin(), differences.end());

    const int keep_count = n + 1 - negative_count;
    T answer = 0;
    for (int i = 0; i < keep_count; ++i)
        answer += differences[i];
    return answer;
}

#endif
