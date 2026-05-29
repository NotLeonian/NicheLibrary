---
title: グラフ同型判定
documentation_of: graph/others/graph-isomorphism.hpp
---

## 概要

- 重みなし無向グラフ $2$ 個が同型か判定する。
- 頂点番号は $0,1,\ldots,\mathrm{n}-1$ とする。
- 自己ループと多重辺を許す。
- 1-WL 色分割で頂点を細分し、残った色クラスを個別化して探索する。

## 使い方

- `is_graph_isomorphic(n, edges1, edges2)`
  - 頂点数 $\mathrm{n}$ の $2$ つの無向グラフが同型なら `true` を返す。
  - `edges1`, `edges2` は各グラフの辺集合である。各辺は 0-based indexing の頂点対で、端点の順序は問わない。
  - 自己ループと多重辺を区別して判定する。多重辺は同じ頂点対を複数回入れる。
  - `edges1`, `edges2` の頂点番号は $0$ 以上 $\mathrm{n}$ 未満である必要がある。
- `GraphIsomorphism(n, edges1, edges2)`
  - 同じ判定を行うオブジェクトを作る。
  - 構築後に `run()` を呼び出す。
- `GraphIsomorphism::run()`
  - 構築時に渡した $2$ つのグラフが同型なら `true` を返す。
  - 再度呼び出すと探索メモを初期化して判定し直す。

## 計算量

- $\mathrm{N}$ を頂点数、$\mathrm{M}_i=\lvert \mathrm{edges}_i\rvert$ とする。
- $\mathrm{U}$ を $2$ つのグラフに現れる、重複を除いた無向辺の個数の合計とする。自己ループも $1$ 個として数える。
- $\mathrm{D}$ を重複除去後の隣接リストの全長の合計とする。$\mathrm{D}\leq 2\mathrm{U}$ である。
- $\mathrm{L}$ を色分割中に作られる signature の最大長とする。$\mathrm{L}\leq 2\mathrm{N}+2$ である。
- $\mathrm{S}$ を DFS で訪問した状態数、$\mathrm{R}$ を全状態を通した色分割更新回数の合計とする。$\mathrm{R}\leq \mathrm{S}(\mathrm{N}+1)$ である。
- `std::unordered_set` の操作は期待計算量で評価する。
- 構築は $O(\mathrm{M}_1\log(\mathrm{M}_1+2)+\mathrm{M}_2\log(\mathrm{M}_2+2)+\mathrm{N}\log(\mathrm{N}+2)+\mathrm{N}+\mathrm{M}_1+\mathrm{M}_2)$ 時間である。
- 色分割更新 $1$ 回は $O(\mathrm{D}\log(\mathrm{N}+2)+\mathrm{N}\mathrm{L}\log(\mathrm{N}+2)+\mathrm{D}+\mathrm{N})$ 時間である。
- 色がすべて単独になった状態での辺集合比較は $O(\mathrm{U}\log(\mathrm{U}+2)+\mathrm{U}+\mathrm{N})$ 時間である。
- 全体は $O(\mathrm{M}_1\log(\mathrm{M}_1+2)+\mathrm{M}_2\log(\mathrm{M}_2+2)+\mathrm{N}\log(\mathrm{N}+2)+\mathrm{N}+\mathrm{M}_1+\mathrm{M}_2+\mathrm{R}(\mathrm{D}\log(\mathrm{N}+2)+\mathrm{N}\mathrm{L}\log(\mathrm{N}+2)+\mathrm{D}+\mathrm{N})+\mathrm{S}(\mathrm{U}\log(\mathrm{U}+2)+\mathrm{U}+\mathrm{N}))$ 時間である。
- 最悪の場合、$\mathrm{S}$ は指数的に大きくなる。
