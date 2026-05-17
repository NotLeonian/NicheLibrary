// competitive-verifier: PROBLEM https://yukicoder.me/problems/no/275
// competitive-verifier: ERROR 0

#include <cassert>
#include <iomanip>
#include <iostream>
#include <limits>

#include "../structure/others/dynamic-median.hpp"

void self_test() {
    DynamicMedian<long long> median;
    assert(!median.erase(0));
    median.add(4);
    assert(median.median() == 4);
    assert(median.median(DynamicMedianMode::Upper) == 4);
    assert(median.median<long double>(DynamicMedianMode::Average) == 4.0L);

    median.add(1);
    assert(median.median(DynamicMedianMode::Lower) == 1);
    assert(median.median(DynamicMedianMode::Upper) == 4);
    assert(median.median<long double>(DynamicMedianMode::Average) == 2.5L);

    median.add(1);
    assert(median.median(DynamicMedianMode::Lower) == 1);
    assert(median.median(DynamicMedianMode::Upper) == 1);
    assert(median.median<long double>(DynamicMedianMode::Average) == 1.0L);

    assert(median.erase(1));
    assert(median.median(DynamicMedianMode::Lower) == 1);
    assert(median.median(DynamicMedianMode::Upper) == 4);
    assert(median.median<long double>(DynamicMedianMode::Average) == 2.5L);

    DynamicMedian<long long> large_median;
    const long long max_value = std::numeric_limits<long long>::max();
    large_median.add(max_value - 1);
    large_median.add(max_value);
    assert(large_median.median(DynamicMedianMode::Average) == max_value - 1);

    DynamicMedian<long long> signed_median;
    const long long min_value = std::numeric_limits<long long>::lowest();
    signed_median.add(min_value);
    signed_median.add(max_value);
    assert(signed_median.median(DynamicMedianMode::Average) == 0);
}

int main() {
#ifndef ONLINE_JUDGE
    self_test();
#endif

    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    std::cin >> n;
    DynamicMedian<long long> median;
    for (int i = 0; i < n; ++i) {
        long long a;
        std::cin >> a;
        median.add(a);
    }

    const long double answer =
        median.median<long double>(DynamicMedianMode::Average);
    std::cout << std::fixed << std::setprecision(1) << answer << '\n';
    return 0;
}
