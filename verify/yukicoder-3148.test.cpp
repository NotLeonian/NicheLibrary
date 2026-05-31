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

    std::vector<long long> c(n + 1, 0);
    for (int i = 0; i < n; ++i) {
        std::cin >> c[i];
    }

    const int root = n;
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

    std::cout << solve_01_on_tree<long long>(n + 1, edges, c, root) << '\n';
    return 0;
}
