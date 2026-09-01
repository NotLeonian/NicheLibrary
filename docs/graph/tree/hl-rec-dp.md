---
title: 重軽再帰 DP
documentation_of: graph/tree/hl-rec-dp.hpp
---

## 概要

- 根付き木の各部分木について、1 つの `State` を入力として `K` 個の `State` を計算する処理を再帰的に行う。
- 各頂点では、部分木のサイズが最大の子を重い子とする。重い辺を連ねたパスでは、全レーンの状態をまとめて処理する。
- 軽い子は各レーンごとに再帰し、`Spec` 側の関数で親側の状態に戻す。
- `before_vertex` と `after_vertex` は、固定した根のもとで各頂点を部分木の根として扱うときの値を記録するために使える。

## 使い方

- `hl_rec_dp(n, edges, root, initial_state, spec)`
  - `n` を頂点数とする。`edges` で辺を与えた木について、`root` を根に定めて重軽再帰 DP を実行する。
  - 頂点番号は 0-based indexing とする。
  - `edges` はサイズ $n-1$ の木の辺の列である。
  - `initial_state` は、根の部分木を計算するときの入力状態である。軽い子を処理するときは、親側の各レーンの状態を新たな入力として使う。
  - `spec` の `make_pack`, `take_heavy`, `take_light`, `before_vertex`, `add_vertex`, `after_vertex` を呼び出す。
  - 返り値は `root` を通常の頂点として加えた後の `std::array<Spec::State, Spec::K>` である。
  - 前提: `root` を $r$ として $n\ge 1,\,0\le r<n$ が成り立ち、`edges` は木を表す。頂点番号が範囲外である場合や、自己ループ、閉路、根から到達できない頂点がある場合は `assert` に失敗する。
- `Spec`
  - `using State = ...;` と `static constexpr int K = ...;` を持つ。
  - 前提: $K\ge 1$ である。
  - `make_pack(v, in)` は、子を持たない頂点 `v` について、入力状態 `in` から DP の組を作って返す。
  - `take_heavy(v, child, pack)` は、重い子 `child` を処理した後の `pack` を、頂点 `v` 側の DP の組に変換して返す。
  - `take_light(v, child, lane, pack)` は、親側のレーン `lane` の状態を入力として軽い子 `child` の部分木を処理した結果 `pack` から、親側の新しい `State` を返す。
  - `before_vertex(v, pack)` は、すべての子を処理してから頂点 `v` を加えるまでの間に呼ばれる。
  - `add_vertex(v, pack)` は、頂点 `v` の情報を `pack` に反映する。
  - `after_vertex(v, pack)` は、`add_vertex` の後に呼ばれる。
  - `before_vertex` と `after_vertex` は、各頂点で 1 回ずつ呼ばれる。
  - `pack` の型は `std::array<State, K>` である。

<details>
<summary>使用例（問題のネタバレを含む）</summary>
<div markdown="1">
例として、[AtCoder Beginner Contest 311 Ex - Many Illumination Plans](https://atcoder.jp/contests/abc311/tasks/abc311_h) を解くソースコードを示す。

```C++
#include <bits/stdc++.h>

#include "graph/tree/hl-rec-dp.hpp"

using namespace std;
using ll = long long;

#define rep(i, r) for (int i = 0; i < (int)(r); ++i)

template <class T1, class T2> bool chmax(T1 &l, const T2 &r) {
    if (r > l) {
        l = r;
        return true;
    }
    return false;
}

namespace abc311 {
struct Spec {
    static constexpr int K = 2;
    using State = vector<ll>;
    using Pack = array<State, K>;

    static constexpr ll NEG = -2'000'000'000'000'000'001;

    vector<ll> ans;

    int x = 0;
    const vector<ll> &b;
    const vector<int> &w, c;

    Spec(int n, int x, const vector<ll> &b, const vector<int> &w,
         const vector<int> &c)
        : x(x), b(b), w(w), c(c) {
        ans.assign(n, NEG);
    }

    Pack make_pack(int, const State &in) { return Pack{in, in}; }

    Pack take_heavy(int, int, Pack &&child_dp) { return move(child_dp); }

    State take_light(int, int, int lane, Pack &&child_dp) {
        return move(child_dp[lane]);
    }

    void before_vertex(int v, const Pack &dp) {
        int c_v = c[v];
        int w_v = w[v];
        ll best = NEG;
        rep(i, x - w_v + 1) { chmax(best, dp[c_v ^ 1][i] + b[v]); }
        chmax(ans[v], best);
    }

    void add_vertex(int v, Pack &dp) {
        int c_v = c[v];
        int w_v = w[v];
        rep(i, x - w_v + 1) { chmax(dp[c_v][i + w_v], dp[c_v ^ 1][i] + b[v]); }
    }

    void after_vertex(int, const Pack &) {}
};
} // namespace abc311

int main() {
    int n, x;
    cin >> n >> x;

    vector<pair<int, int>> edges(n - 1);
    rep(i, n - 1) {
        int p_i;
        cin >> p_i;
        p_i -= 1;
        edges[i] = {i + 1, p_i};
    }

    vector<int> w(n), c(n);
    vector<ll> b(n);
    rep(i, n) { cin >> b[i] >> w[i] >> c[i]; }

    using abc311::Spec;

    Spec::State initial_state(x + 1, Spec::NEG);
    initial_state[0] = 0;
    Spec spec(n, x, b, w, c);

    hl_rec_dp(n, edges, 0, initial_state, spec);

    rep(i, n) { cout << spec.ans[i] << "\n"; }

    return 0;
}
```

</div>
</details>

## 計算量

頂点数を $n$ とし、`Spec::K` の値を $K$ とする。

- 木に根を設定して重い子を選ぶ処理には、時間 $O(n)$ が掛かる。
- 再帰呼び出しは軽い辺を通るときだけ入れ子になるため、再帰の深さは $O(\log n)$ である。
- 1 つの部分木について 1 つの入力状態から DP の組を計算するとき、`Spec` の関数を呼び出す回数は $O(n^{\log_2(K+1)})$ である。
- `before_vertex` と `after_vertex` をすべての頂点で呼び出す処理では、`Spec` の関数を呼び出す回数は $K\ge 2$ の場合に $O(n^{\log_2(K+1)})$ である。 $K=1$ の場合は $O(n\log n)$ である。
- `Spec` の各関数を 1 回呼び出す時間の上界を $X$ とする。典型的な場合として、 $K=2$ ならば全体の時間計算量は $O(n^{\log_2 3}X)$ である。

## 参考文献

1. Soh Kumabe, Takanori Maehara, and Ryoma Sin'ya. Linear Pseudo-Polynomial Factor Algorithm for Automaton Constrained Tree Knapsack Problem. In WALCOM: Algorithms and Computation, Lecture Notes in Computer Science, Vol. 11355, pp. 248–260. Springer, 2019. doi:10.1007/978-3-030-10564-8_20.（[arXiv のリンク](https://arxiv.org/abs/1807.04942)）
