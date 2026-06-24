// competitive-verifier: PROBLEM https://yukicoder.me/problems/no/2603

#include <iostream>
#include <vector>

#include "../other/minimum-mod-range-increment-decrement-operations.hpp"

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    long long m;
    std::cin >> n >> m;

    std::vector<long long> a(n), b(n);
    for (int i = 0; i < n; ++i) {
        std::cin >> a[i];
    }
    for (int i = 0; i < n; ++i) {
        std::cin >> b[i];
    }

    std::cout << minimum_mod_range_increment_decrement_operations(a, b, m)
              << '\n';

    return 0;
}
