// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/area_of_union_of_rectangles

#include <algorithm>
#include <iostream>

#include "../other/rectangle-add-max-get.hpp"

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    std::cin >> n;
    CompressedRectangleAddMaxGet<long long, long long> solver(n);
    long long min_l = 0, min_d = 0, max_r = 0, max_u = 0;
    for (int i = 0; i < n; ++i) {
        long long l, d, r, u;
        std::cin >> l >> d >> r >> u;
        solver.add_rectangle(l, d, r, u, -1);
        if (i == 0) {
            min_l = l;
            min_d = d;
            max_r = r;
            max_u = u;
        } else {
            min_l = std::min(min_l, l);
            min_d = std::min(min_d, d);
            max_r = std::max(max_r, r);
            max_u = std::max(max_u, u);
        }
    }
    if (n == 0) {
        std::cout << 0 << '\n';

        return 0;
    }
    const auto [max_value, outside_area] =
        solver.calc_max_area<long long>(min_l, min_d, max_r, max_u);
    const long long total_area = (max_r - min_l) * (max_u - min_d);
    long long answer = total_area;
    if (max_value == 0) {
        answer -= outside_area;
    }
    std::cout << answer << '\n';

    return 0;
}
