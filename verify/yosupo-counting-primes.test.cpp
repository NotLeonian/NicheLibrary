// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/counting_primes

#include <iostream>

#include "../math/multiplicative-function/prime-counting-modulo.hpp"

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    long long N;
    std::cin >> N;
    std::cout << prime_counting_modulo(N, 1)[0] << '\n';

    return 0;
}
