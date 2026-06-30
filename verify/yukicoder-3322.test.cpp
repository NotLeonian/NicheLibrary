// competitive-verifier: PROBLEM https://yukicoder.me/problems/no/3322

#include <cassert>
#include <cstddef>
#include <iostream>
#include <utility>
#include <vector>

#include "../other/enumerate-maximum-independent-set-path-sums.hpp"

long long solve(const std::vector<long long> &a,
                const std::vector<long long> &b, const int k) {
    const int n = static_cast<int>(a.size());
    long long answer = 0;
    std::vector<std::pair<int, long long>> blocks;

    const auto add_block = [&](const int side, const long long cost) {
        if (!blocks.empty() && blocks.back().first == side) {
            blocks.back().second += cost;
        } else {
            blocks.push_back(std::pair<int, long long>(side, cost));
        }
    };

    add_block(0, 0);
    for (int i = 0; i < n; i += 1) {
        const std::size_t j = static_cast<std::size_t>(i);
        if (a[j] >= b[j]) {
            answer += a[j];
            add_block(0, a[j] - b[j]);
        } else {
            answer += b[j];
            add_block(1, b[j] - a[j]);
        }
    }
    add_block(k % 2, 0);

    const int transitions = static_cast<int>(blocks.size() - 1);
    int remove_count = 0;
    if (transitions > k) {
        assert((transitions - k) % 2 == 0);
        remove_count = (transitions - k) / 2;
    }

    std::vector<long long> penalties;
    penalties.reserve(blocks.size());
    for (std::size_t i = 1; i + 1 < blocks.size(); i += 1) {
        penalties.push_back(-blocks[i].second);
    }

    const std::vector<long long> best =
        enumerate_maximum_independent_set_path_sums(penalties);
    assert(0 <= remove_count &&
           static_cast<std::size_t>(remove_count) < best.size());
    return answer + best[static_cast<std::size_t>(remove_count)];
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int t;
    std::cin >> t;
    for (int case_id = 0; case_id < t; case_id += 1) {
        int n;
        int k;
        std::cin >> n >> k;
        std::vector<long long> a(static_cast<std::size_t>(n)),
            b(static_cast<std::size_t>(n));
        for (int i = 0; i < n; i += 1) {
            std::cin >> a[static_cast<std::size_t>(i)];
        }
        for (int i = 0; i < n; i += 1) {
            std::cin >> b[static_cast<std::size_t>(i)];
        }
        std::cout << solve(a, b, k) << '\n';
    }

    return 0;
}
