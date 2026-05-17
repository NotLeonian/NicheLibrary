// competitive-verifier: PROBLEM https://yukicoder.me/problems/no/275

#include <cassert>
#include <iomanip>
#include <iostream>

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
