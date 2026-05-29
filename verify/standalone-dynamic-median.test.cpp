// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <limits>
#include <vector>

#include "../structure/others/dynamic-median.hpp"

namespace {
template <class T> std::vector<T> sorted_values(std::vector<T> values) {
    std::sort(values.begin(), values.end());
    return values;
}

template <class T>
void check_dynamic_median(const DynamicMedian<T> &median,
                          const std::vector<T> &values) {
    assert(!values.empty());
    const auto sorted = sorted_values(values);
    assert(median.median(DynamicMedianMode::Lower) ==
           sorted[(sorted.size() - 1) / 2]);
    assert(median.median() == sorted[(sorted.size() - 1) / 2]);
    assert(median.median(DynamicMedianMode::Upper) ==
           sorted[sorted.size() / 2]);
    const long double expected_average =
        (static_cast<long double>(sorted[(sorted.size() - 1) / 2]) +
         static_cast<long double>(sorted[sorted.size() / 2])) /
        2.0L;
    assert(median.template median<long double>(DynamicMedianMode::Average) ==
           expected_average);
}

void check_scripted_operations() {
    DynamicMedian<int> median;
    std::vector<int> values;

    const std::vector<int> additions{5, -1, 5, -3, 7, 0, 0, 12, -8};
    for (int value : additions) {
        median.add(value);
        values.push_back(value);
        check_dynamic_median(median, values);
    }

    const std::vector<int> erasures{4, 5, -1, 0, 0, 12, -8, -3, 5, 7};
    for (int value : erasures) {
        const auto it = std::find(values.begin(), values.end(), value);
        const bool expected = it != values.end();
        assert(median.erase(value) == expected);
        if (expected) {
            values.erase(it);
            if (!values.empty()) {
                check_dynamic_median(median, values);
            }
        }
    }
}

void check_known_cases() {
    DynamicMedian<long long> median;
    assert(!median.erase(0));
    median.add(4);
    assert(median.median() == 4);
    assert(median.median(DynamicMedianMode::Upper) == 4);
    assert(median.median<long double>(DynamicMedianMode::Average) == 4.0L);

    median.add(1);
    assert(median.median(DynamicMedianMode::Lower) == 1);
    assert(median.median(DynamicMedianMode::Upper) == 4);
    assert(median.median<long double>(DynamicMedianMode::Average) == 2.5L);

    median.add(1);
    assert(median.median(DynamicMedianMode::Lower) == 1);
    assert(median.median(DynamicMedianMode::Upper) == 1);
    assert(median.median<long double>(DynamicMedianMode::Average) == 1.0L);

    assert(median.erase(1));
    assert(median.median(DynamicMedianMode::Lower) == 1);
    assert(median.median(DynamicMedianMode::Upper) == 4);
    assert(median.median<long double>(DynamicMedianMode::Average) == 2.5L);

    DynamicMedian<long long> large_median;
    const long long max_value = std::numeric_limits<long long>::max();
    large_median.add(max_value - 1);
    large_median.add(max_value);
    assert(large_median.median(DynamicMedianMode::Average) == max_value - 1);

    DynamicMedian<long long> signed_median;
    const long long min_value = std::numeric_limits<long long>::lowest();
    signed_median.add(min_value);
    signed_median.add(max_value);
    assert(signed_median.median(DynamicMedianMode::Average) == 0);
}

void self_test() {
    check_known_cases();
    check_scripted_operations();
}
} // namespace

int main() {
    self_test();
    return 0;
}
