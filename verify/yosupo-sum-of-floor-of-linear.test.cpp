// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/sum_of_floor_of_linear

#include <iostream>

#include "../math/number-theory/floor-sum.hpp"

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int T;
    std::cin >> T;
    while (T--) {
        long long n, m, a, b;
        std::cin >> n >> m >> a >> b;
        std::cout << floor_sum<long long>(n, m, a, b) << '\n';
    }

    return 0;
}
