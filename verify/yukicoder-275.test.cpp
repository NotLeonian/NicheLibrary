// competitive-verifier: PROBLEM https://yukicoder.me/problems/no/275
// competitive-verifier: ERROR 0

#include <iomanip>
#include <iostream>

#include "../structure/others/dynamic-median.hpp"

int main() {
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
