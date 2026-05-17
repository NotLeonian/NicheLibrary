// competitive-verifier: PROBLEM https://yukicoder.me/problems/no/919

#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

#include "../structure/others/dynamic-median.hpp"

void self_test() {
    DynamicMedian<int> median;
    median.add(5);
    assert(median.median() == 5);
    assert(median.median(DynamicMedianMode::Upper) == 5);
    assert(median.median<double>(DynamicMedianMode::Average) == 5.0);

    median.add(-1);
    assert(median.median() == -1);
    assert(median.median(DynamicMedianMode::Upper) == 5);
    assert(median.median<double>(DynamicMedianMode::Average) == 2.0);

    median.add(5);
    assert(median.median() == 5);
    assert(median.median(DynamicMedianMode::Upper) == 5);
    assert(!median.erase(4));
    assert(median.erase(5));
    assert(median.median() == -1);
    assert(median.median(DynamicMedianMode::Upper) == 5);

    median.add(-3);
    median.add(7);
    assert(median.median() == -1);
    assert(median.erase(-1));
    assert(median.median() == 5);
    assert(median.erase(5));
    assert(median.median() == -3);
}

struct Query {
    int l;
    int r;
    int id;
};

int main() {
#ifndef ONLINE_JUDGE
    self_test();
#endif

    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    std::cin >> n;
    std::vector<int> a(n);
    for (int i = 0; i < n; ++i) {
        std::cin >> a[i];
    }

    std::vector<Query> queries;
    std::vector<std::vector<int>> left_ids(n + 1), right_ids(n + 1);
    for (int k = 1; k <= n; ++k) {
        const int count = n / k;
        left_ids[k].reserve(count);
        right_ids[k].reserve(count);

        for (int i = 0; i < count; ++i) {
            const int id = static_cast<int>(queries.size());
            left_ids[k].push_back(id);
            queries.push_back(Query{i * k, (i + 1) * k, id});
        }
        for (int i = 0; i < count; ++i) {
            const int id = static_cast<int>(queries.size());
            right_ids[k].push_back(id);
            queries.push_back(Query{n - (i + 1) * k, n - i * k, id});
        }
    }

    constexpr int block_size = 100;
    std::sort(queries.begin(), queries.end(),
              [](const Query &lhs, const Query &rhs) {
                  const int lhs_block = lhs.l / block_size;
                  const int rhs_block = rhs.l / block_size;
                  if (lhs_block != rhs_block) {
                      return lhs_block < rhs_block;
                  }
                  if (lhs_block & 1) {
                      return lhs.r > rhs.r;
                  }
                  return lhs.r < rhs.r;
              });

    DynamicMedian<int> median;
    std::vector<int> medians(queries.size());
    int current_l = 0;
    int current_r = 0;
    for (const Query &query : queries) {
        while (query.l < current_l) {
            median.add(a[--current_l]);
        }
        while (current_r < query.r) {
            median.add(a[current_r++]);
        }
        while (current_l < query.l) {
            const bool erased = median.erase(a[current_l++]);
            assert(erased);
        }
        while (query.r < current_r) {
            const bool erased = median.erase(a[--current_r]);
            assert(erased);
        }
        medians[query.id] = median.median();
    }

    long long answer = 0;
    for (int k = 1; k <= n; ++k) {
        const int count = static_cast<int>(left_ids[k].size());
        std::vector<long long> left_sum(count + 1), right_sum(count + 1);
        std::vector<long long> right_best(count + 1);
        for (int i = 0; i < count; ++i) {
            left_sum[i + 1] = left_sum[i] + medians[left_ids[k][i]];
            right_sum[i + 1] = right_sum[i] + medians[right_ids[k][i]];
        }
        for (int i = 1; i <= count; ++i) {
            right_best[i] = std::max(right_best[i - 1], right_sum[i]);
        }
        for (int i = 0; i <= count; ++i) {
            const long long score = (left_sum[i] + right_best[count - i]) *
                                    static_cast<long long>(k);
            answer = std::max(answer, score);
        }
    }

    std::cout << answer << '\n';
    return 0;
}
