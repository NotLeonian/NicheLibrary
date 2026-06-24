// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/many_aplusb_128bit

#include <iostream>

#include "../internal/int128.hpp"

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int t;
    std::cin >> t;
    while (t--) {
        NicheLibrary::Int128 a, b;
        std::cin >> a >> b;
        std::cout << a + b << '\n';
    }

    return 0;
}
