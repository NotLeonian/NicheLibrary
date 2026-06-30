// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <vector>

#include "../other/enumerate-maximum-independent-set-path-sums.hpp"

template <class T> std::vector<T> brute_force(const std::vector<T> &a) {
    const std::size_t n = a.size();
    const std::size_t m = (n + 1) / 2;
    std::vector<T> answer(m + 1, T());
    std::vector<bool> found(m + 1, false);
    const std::uint64_t masks = std::uint64_t{1} << n;
    for (std::uint64_t mask = 0; mask < masks; mask += 1) {
        bool ok = true;
        std::size_t count = 0;
        T sum = T();
        for (std::size_t i = 0; i < n; i += 1) {
            if (((mask / (std::uint64_t{1} << i)) % 2) != 0) {
                if (i > 0 &&
                    ((mask / (std::uint64_t{1} << (i - 1))) % 2) != 0) {
                    ok = false;
                }
                count += 1;
                sum += a[i];
            }
        }
        if (ok && (!found[count] || answer[count] < sum)) {
            answer[count] = sum;
            found[count] = true;
        }
    }
    for (std::size_t i = 0; i <= m; i += 1) {
        assert(found[i]);
    }
    return answer;
}

void test_signed_exhaustive() {
    for (std::size_t n = 0; n <= 6; n += 1) {
        std::vector<long long> a(n);
        auto dfs = [&](auto self, const std::size_t i) -> void {
            if (i == n) {
                assert(enumerate_maximum_independent_set_path_sums(a) ==
                       brute_force(a));
                return;
            }
            for (long long x = -3; x <= 3; x += 1) {
                a[i] = x;
                self(self, i + 1);
            }
        };
        dfs(dfs, 0);
    }
}

void test_small_signed_type_exhaustive() {
    for (std::size_t n = 0; n <= 6; n += 1) {
        std::vector<signed char> a(n);
        auto dfs = [&](auto self, const std::size_t i) -> void {
            if (i == n) {
                assert(enumerate_maximum_independent_set_path_sums(a) ==
                       brute_force(a));
                return;
            }
            for (signed char x = -3; x <= 3;
                 x = static_cast<signed char>(x + 1)) {
                a[i] = x;
                self(self, i + 1);
            }
        };
        dfs(dfs, 0);
    }
}

void test_signed_bucket_sort() {
    for (std::size_t n = 0; n <= 7; n += 1) {
        std::vector<int> a(n);
        auto dfs = [&](auto self, const std::size_t i) -> void {
            if (i == n) {
                assert(enumerate_maximum_independent_set_path_sums_bucket_sort(
                           a) == brute_force(a));
                return;
            }
            for (int x = 0; x <= 4; x += 1) {
                a[i] = x;
                self(self, i + 1);
            }
        };
        dfs(dfs, 0);
    }
}

void test_bucket_large_values() {
    const std::vector<int> a = {100, 1, 100, 1, 100, 1, 100};
    const std::vector<int> expected = brute_force(a);
    assert(enumerate_maximum_independent_set_path_sums(a) == expected);
    assert(enumerate_maximum_independent_set_path_sums_bucket_sort(a) ==
           expected);

    const std::vector<std::int32_t> b = {3, 0, 5, 0, 4, 1};
    const std::vector<std::int32_t> expected_b = brute_force(b);
    assert(enumerate_maximum_independent_set_path_sums_bucket_sort(b) ==
           expected_b);
}

void test_bucket_small_signed_types() {
    const std::vector<short> a = {2, 0, 3, 1, 4};
    const std::vector<short> expected = brute_force(a);
    assert(enumerate_maximum_independent_set_path_sums_bucket_sort(a) ==
           expected);

    const std::vector<signed char> b = {2, 0, 3, 1, 4};
    const std::vector<signed char> expected_b = brute_force(b);
    assert(enumerate_maximum_independent_set_path_sums_bucket_sort(b) ==
           expected_b);
}

struct TestNumber {
    long long value;

    TestNumber() : value(0) {}

    explicit TestNumber(const long long value_) : value(value_) {}

    TestNumber operator+(const TestNumber &other) const {
        return TestNumber(value + other.value);
    }

    TestNumber operator-(const TestNumber &other) const {
        return TestNumber(value - other.value);
    }

    TestNumber &operator+=(const TestNumber &other) {
        value += other.value;
        return *this;
    }

    bool operator<(const TestNumber &other) const {
        return value < other.value;
    }

    bool operator==(const TestNumber &other) const {
        return value == other.value;
    }
};

void test_custom_type() {
    const std::vector<TestNumber> a = {TestNumber(1), TestNumber(100),
                                       TestNumber(1), TestNumber(4),
                                       TestNumber(5)};
    assert(enumerate_maximum_independent_set_path_sums(a) == brute_force(a));
}

void test_floating_point() {
    const std::vector<double> a = {1.5, 100.25, 1.5, -3.0, 10.0, 2.0};
    assert(enumerate_maximum_independent_set_path_sums(a) == brute_force(a));
}

void test_type_traits() {
    static_assert(!std::is_unsigned_v<int>,
                  "std::is_unsigned_v<int> must be false in this verify.");
    static_assert(!std::is_unsigned_v<double>,
                  "std::is_unsigned_v<double> must be false in this verify.");
    static_assert(
        !std::is_unsigned_v<TestNumber>,
        "std::is_unsigned_v<TestNumber> must be false in this verify.");
    static_assert(
        std::is_unsigned_v<unsigned int>,
        "std::is_unsigned_v<unsigned int> must be true in this verify.");
}

int main() {
    test_signed_exhaustive();
    test_small_signed_type_exhaustive();
    test_signed_bucket_sort();
    test_bucket_large_values();
    test_bucket_small_signed_types();
    test_custom_type();
    test_floating_point();
    test_type_traits();

    return 0;
}
