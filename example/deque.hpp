#ifndef EXAMPLE_DEQUE_HPP
#define EXAMPLE_DEQUE_HPP

#include <vector>

template <class T> struct deque {
    int begin, end;
    std::vector<T> data;

    deque(int q) : begin(q), end(q) { data.resize(2 * q); }

    void push_left(const T x) {
        --begin;
        data[begin] = x;
    }

    void push_right(const T x) {
        data[end] = x;
        ++end;
    }

    void pop_left() { ++begin; }

    void pop_right() { --end; }

    T &operator[](int i) { return data[i]; }
};

#endif