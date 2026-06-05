// competitive-verifier: PROBLEM https://yukicoder.me/problems/no/2117

#include <iostream>
#include <vector>

#include "../math/number-theory/generalized-garner.hpp"

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    long long b0, c0, b1, c1;
    std::cin >> b0 >> c0;
    std::cin >> b1 >> c1;
    const std::vector<long long> a = {1, 1};
    const std::vector<long long> b = {c0, c1};
    const std::vector<long long> m = {b0, b1};
    const auto [r, mod] = generalized_garner<long long>(a, b, m);
    if (mod == 0) {
        std::cout << "NaN\n";
    } else {
        std::cout << r << '\n';
    }
    return 0;
}
