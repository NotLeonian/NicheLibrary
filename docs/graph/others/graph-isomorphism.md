---
title: グラフ同型判定
documentation_of: graph/others/graph-isomorphism.hpp
---

## 概要

- 重みなし無向グラフ $2$ 個が同型か判定する。
- 頂点番号は $0,1,\ldots,n-1$ とする。
- 自己ループと多重辺を許す。
- 1-WL 色分割で頂点を細分し、残った色クラスを個別化して探索する。

## 使い方

- `is_graph_isomorphic(n, edges_1, edges_2)`
  - 頂点数 $n$ の $2$ つの無向グラフが同型なら `true` を返す。
  - `edges_1`, `edges_2` は各グラフの辺集合である。各辺は 0-based indexing の頂点対で、端点の順序は問わない。
  - 自己ループと多重辺を区別して判定する。多重辺は同じ頂点対を複数回入れる。
  - `edges_1`, `edges_2` の頂点番号は $0$ 以上 $n$ 未満である必要がある。
- `GraphIsomorphism(n, edges_1, edges_2)`
  - 同じ判定を行うオブジェクトを作る。
  - 構築後に `run()` を呼び出す。
- `GraphIsomorphism::run()`
  - 構築時に渡した $2$ つのグラフが同型なら `true` を返す。
  - 再度呼び出すと探索メモを初期化して判定し直す。

## 計算量

- $N$ を頂点数、$M_i=\lvert \mathrm{edges}_i\rvert$ とする。
- $U$ を $2$ つのグラフに現れる、重複を除いた無向辺の個数の合計とする。自己ループも $1$ 個として数える。
- $D$ を重複除去後の隣接リストの全長の合計とする。$D\le 2U$ である。
- $L$ を色分割中に作られる signature の最大長とする。$L\le 2N+2$ である。
- $S$ を DFS で訪問した状態数、$R$ を全状態を通した色分割更新回数の合計とする。$R\le S(N+1)$ である。
- 探索メモは平衡二分探索木で持つ。状態キーの比較は最悪 $O(N)$ 時間である。
- $C_{\mathrm{build}}=M_1\log(M_1+2)+M_2\log(M_2+2)+N\log(N+2)+N+M_1+M_2$ とする。
- $C_{\mathrm{ref}}=D\log(N+2)+NL\log(N+2)+NL+D+N$ とする。
- $C_{\mathrm{memo}}=N\log(S+2)+N$ とする。
- $C_{\mathrm{check}}=U\log(U+2)+U+N$ とする。
- 構築は $O(C_{\mathrm{build}})$ 時間である。
- 色分割更新 $1$ 回は $O(C_{\mathrm{ref}})$ 時間である。
- 探索メモの検索または挿入は $O(C_{\mathrm{memo}})$ 時間である。
- 色がすべて単独になった状態での辺集合比較は $O(C_{\mathrm{check}})$ 時間である。
- 全体は $O(C_{\mathrm{build}}+RC_{\mathrm{ref}}+S(C_{\mathrm{memo}}+C_{\mathrm{check}}))$ 時間である。
- 最悪の場合、$S$ は指数的に大きくなる。
