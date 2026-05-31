// competitive-verifier: PROBLEM https://yukicoder.me/problems/no/3148

#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "../graph/tree/01-on-tree.hpp"

int main() {
    int n;
    std::string s;
    std::cin >> n >> s;

    const int root = n;
    std::vector<long long> c0(n + 1, 0), c1(n + 1, 0);
    long long sum = 0;
    for (int i = 0; i < n; ++i) {
        std::cin >> c0[i];
        c1[i] = 1;
        sum += c0[i];
    }

    std::vector<std::pair<int, int>> edges;
    edges.reserve(n);
    std::vector<int> stack;
    stack.reserve(n);
    int id = 0;
    for (char ch : s) {
        if (ch == '(') {
            const int v = id++;
            const int parent = stack.empty() ? root : stack.back();
            edges.push_back({parent, v});
            stack.push_back(v);
        } else {
            stack.pop_back();
        }
    }

    const long long answer =
        solve_01_on_tree<long long>(n + 1, edges, c0, c1, root) + sum;
    std::cout << answer << '\n';
    return 0;
}
