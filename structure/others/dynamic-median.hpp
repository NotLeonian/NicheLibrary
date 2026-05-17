#ifndef STRUCTURE_OTHERS_DYNAMIC_MEDIAN_HPP
#define STRUCTURE_OTHERS_DYNAMIC_MEDIAN_HPP

// 値の多重集合に対して追加、削除、中央値取得を行う。
// 中央値は下側、上側、両側の算術平均から選べる。
// T はコピー可能で、std::multiset で扱える比較を持つ型を仮定する。
// median は空でないことを仮定する。
// add, erase は O(log N)、median は O(1) である。

#include <cassert>
#include <set>
#include <type_traits>

enum class DynamicMedianMode { Lower, Upper, Average };

template <class T> struct DynamicMedian {
    std::multiset<T> lower_values;
    std::multiset<T> upper_values;

    void add(T x) {
        if (lower_values.empty() || !(*lower_values.rbegin() < x)) {
            lower_values.insert(x);
        } else {
            upper_values.insert(x);
        }
        balance();
    }

    bool erase(T x) {
        auto lower_itr = lower_values.find(x);
        if (lower_itr != lower_values.end()) {
            lower_values.erase(lower_itr);
            balance();
            return true;
        }

        auto upper_itr = upper_values.find(x);
        if (upper_itr != upper_values.end()) {
            upper_values.erase(upper_itr);
            balance();
            return true;
        }

        return false;
    }

    template <class Result = T>
    Result median(DynamicMedianMode mode = DynamicMedianMode::Lower) const {
        assert(!lower_values.empty());
        const T &lower_median = *lower_values.rbegin();

        if (mode == DynamicMedianMode::Lower) {
            return static_cast<Result>(lower_median);
        }
        if (lower_values.size() != upper_values.size()) {
            return static_cast<Result>(lower_median);
        }

        const T &upper_median = *upper_values.begin();
        if (mode == DynamicMedianMode::Upper) {
            return static_cast<Result>(upper_median);
        }

        assert(mode == DynamicMedianMode::Average);
        const Result lower = static_cast<Result>(lower_median);
        const Result upper = static_cast<Result>(upper_median);
        if constexpr (std::is_integral_v<Result>) {
            if constexpr (std::is_signed_v<Result>) {
                if (lower < 0 && 0 < upper) {
                    return (lower + upper) / 2;
                }
            }
            return lower / 2 + upper / 2 + (lower % 2 + upper % 2) / 2;
        } else if constexpr (requires(Result lhs, Result rhs) {
                                 (lhs + rhs) / Result(2);
                             }) {
            return (lower + upper) / Result(2);
        } else {
            assert(false);
            return static_cast<Result>(lower_median);
        }
    }

  private:
    void balance() {
        while (lower_values.size() < upper_values.size()) {
            lower_values.insert(*upper_values.begin());
            upper_values.erase(upper_values.begin());
        }
        while (lower_values.size() > upper_values.size() + 1) {
            auto itr = lower_values.end();
            --itr;
            upper_values.insert(*itr);
            lower_values.erase(itr);
        }
    }
};

#endif
