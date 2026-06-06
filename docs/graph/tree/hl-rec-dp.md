---
title: 重軽再帰 DP
documentation_of: graph/tree/hl-rec-dp.hpp
---

## 概要

- 根付き木の部分木を、1 本の `State` から `K` 本の `State` の組へ写す変換として再帰的に計算する。
- 各頂点では最大部分木サイズの子を重い子とし、重い子だけは全レーンをまとめて 1 回再帰する。
- 軽い子は各レーンごとに再帰し、`Spec` 側の関数で親側の状態に戻す。
- `before_vertex` と `after_vertex` により、固定した根のもとで各頂点を部分木の根とする値を列挙できる。

## 使い方

- `hl_rec_dp(n, edges, root, initial_state, spec)`
  - 頂点数が `n` で、無向辺列が `edges` である木を `root` で根付き木にして実行する。
  - 頂点番号は 0-based indexing とする。
  - `edges` はサイズ $n-1$ の木の辺列である。
  - `initial_state` は各重パスの根側から渡す初期状態である。
  - `spec` の `before_vertex`, `add_vertex`, `after_vertex` などを呼び出し、`spec` の中の答えの配列を更新する。
  - 返り値は `root` を通常の頂点として加えた後の `std::array<Spec::State, Spec::K>` である。
  - 前提：$n\ge 1$、$0\le \mathrm{root}<n$、`edges` は木である。
- `Spec`
  - `using State = ...;` と `static constexpr int K = ...;` を持つ。
  - `make_pack(v, in)` は、子をまだ処理していない頂点 `v` の DP の組を返す。
  - `take_heavy(v, child, pack)` は、重い子 `child` の返り値を頂点 `v` 側の DP の組に変換して返す。
  - `take_light(v, child, lane, pack)` は、軽い子 `child` をレーン `lane` だけ処理した結果から、親側の 1 本の `State` を返す。
  - `before_vertex(v, pack)` は、頂点 `v` を部分木根として扱う値を記録するために呼ばれる。
  - `add_vertex(v, pack)` は、頂点 `v` を親へ返す通常の頂点として `pack` に反映する。
  - `after_vertex(v, pack)` は、`add_vertex` 後の値を必要に応じて記録するために呼ばれる。
  - `pack` の型は `std::array<State, K>` である。

<details>
<summary>使用例（問題のネタバレを含む）</summary>
<div markdown="1">
例として、AtCoder Beginner Contest 311 Ex - Many Illumination Plans を解くソースコードを示す。

competitive-verifier によるドキュメントの自動生成はドキュメント中のリンクを自動検出してまとめることから、ネタバレを防ぐために問題の URL は記載していない。
適宜、各自で調べること。

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

- 木の根付き化と重い子の選択：$O(n)$。
- 軽い辺だけで再帰が深くなるため、再帰段数：$O(\log n)$。
- $K$ を `Spec::K` の値とする。部分木 1 回分の値計算の呼び出し回数は $O(n^{\log_2(K+1)})$。
- 全頂点の `before_vertex` / `after_vertex` を回収する実行では、$K\ge 2$ なら同じく $O(n^{\log_2(K+1)})$ 回、$K=1$ なら $O(n\log n)$ 回である。
- 各呼び出しでの `Spec` 側の処理時間が掛かる。典型的に $K=2$、各処理が $O(X)$ なら $O(n^{\log_2 3}X)$。

## 参考文献
1. Soh Kumabe, Takanori Maehara, and Ryoma Sin'ya. Linear Pseudo-Polynomial Factor Algorithm for Automaton Constrained Tree Knapsack Problem. In WALCOM: Algorithms and Computation, Lecture Notes in Computer Science, Vol. 11355, pp. 248–260. Springer, 2019. doi:10.1007/978-3-030-10564-8_20.（[arXiv のリンク](https://arxiv.org/abs/1807.04942)）
