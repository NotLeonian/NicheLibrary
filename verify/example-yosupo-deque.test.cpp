// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/deque

#include <iostream>

#include "../example/deque.hpp"

int main() {
    int q;
    std::cin >> q;
    deque<int> dq(q);
    while (q--) {
        int type;
        std::cin >> type;
        if (type == 0) {
            int x;
            std::cin >> x;
            dq.push_left(x);
        } else if (type == 1) {
            int x;
            std::cin >> x;
            dq.push_right(x);
        } else if (type == 2) {
            dq.pop_left();
        } else if (type == 3) {
            dq.pop_right();
        } else {
            int i;
            std::cin >> i;
            std::cout << dq[i] << "\n";
        }
    }
}