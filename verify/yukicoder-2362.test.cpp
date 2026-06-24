// competitive-verifier: PROBLEM https://yukicoder.me/problems/no/2362

#include <cstdint>
#include <iostream>

#include "../math/number-theory/generalized-floor-sum-degree-le-2.hpp"

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int T;
    std::cin >> T;
    while (T--) {
        std::uint64_t N, M, X, Y;
        std::cin >> N >> M >> X >> Y;

        const auto r0 =
            generalized_floor_sum_degree_le_2<std::uint64_t>(N, M, X, 0);
        const auto r1 =
            generalized_floor_sum_degree_le_2<std::uint64_t>(N, M, X, Y);

        std::uint64_t ans = 0;
        ans -= r0.ans_01 * N - r0.ans_11;
        ans -= r1.ans_01 * (N - 1) - 2 * r1.ans_11;

        std::cout << ans << '\n';
    }

    return 0;
}
