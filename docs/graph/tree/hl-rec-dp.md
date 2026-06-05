---
title: 重軽再帰 DP
documentation_of: graph/tree/hl-rec-dp.hpp
---

## 概要

- 根付き木の部分木を、1 本の `State` から `K` 本の `State` の組へ写す変換として再帰的に計算する。
- 各頂点では最大部分木サイズの子を重い子とし、重い子だけは全レーンをまとめて 1 回再帰する。
- 軽い子は各レーンごとに再帰し、`Spec` 側の関数で親側の状態に戻す。
- `before_vertex` と `after_vertex` により、固定根のもとで各頂点を部分木根とする値を列挙できる。

## 使い方

- `hl_rec_dp(n, edges, root, initial_state, spec)`
  - 頂点数 `n`、無向辺列 `edges` の木を `root` で根付き木にして実行する。
  - 頂点番号は 0-based indexing とする。
  - `edges` はサイズ $n-1$ の木の辺列である。
  - `initial_state` は各重パスの根側から渡す初期状態である。
  - `spec` の `before_vertex`, `add_vertex`, `after_vertex` などを呼び出し、`spec` の中の答え配列を更新する。
  - 返り値は `root` を通常頂点として加えた後の `std::array<Spec::State, Spec::K>` である。
  - 前提：$n\ge 1$、$0\le \mathrm{root}<n$、`edges` は木である。
- `Spec`
  - `using State = ...;` と `static constexpr int K = ...;` を持つ。
  - `make_pack(v, in)` は、子をまだ処理していない頂点 `v` の DP の組を返す。
  - `take_heavy(v, child, pack)` は、重い子 `child` の返り値を頂点 `v` 側の DP の組に変換して返す。
  - `take_light(v, child, lane, pack)` は、軽い子 `child` をレーン `lane` だけ処理した結果から、親側の 1 本の `State` を返す。
  - `before_vertex(v, pack)` は、頂点 `v` を部分木根として扱う値を記録するために呼ばれる。
  - `add_vertex(v, pack)` は、頂点 `v` を親へ返す通常頂点として `pack` に反映する。
  - `after_vertex(v, pack)` は、`add_vertex` 後の値を必要に応じて記録するために呼ばれる。
  - `pack` の型は `std::array<State, K>` である。

## 計算量

- 木の根付き化と重い子の選択：$O(n)$。
- 軽い辺だけで再帰が深くなるため、再帰段数：$O(\log n)$。
- $K$ を `Spec::K` の値とする。部分木 1 回分の値計算の呼び出し回数は $O(n^{\log_2(K+1)})$。
- 全頂点の `before_vertex` / `after_vertex` を回収する実行では、$K\ge 2$ なら同じく $O(n^{\log_2(K+1)})$ 回、$K=1$ なら $O(n\log n)$ 回である。
- 各呼び出しでの `Spec` 側の処理時間を掛ける。典型的に $K=2$、各処理が $O(X)$ なら $O(n^{\log_2 3}X)$。
