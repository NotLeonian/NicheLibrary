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

- 頂点数を $n$ とし、$M_i=\lvert \mathrm{edges}_i\rvert$ とする。$M_i$ は多重辺を重複込みで数え、自己ループも入力 $1$ 個を $1$ 本として数える。
- $U$ を $2$ つのグラフの内部辺要素の個数の合計とする。内部辺要素は、同じ頂点対に入った辺を $1$ つにまとめ、本数を `count` として持つ。自己ループの頂点対も $1$ つと数える。
- 多重辺は重複を消して同一視するのではなく、内部辺要素の `count` まで比較される。
- $D$ を内部隣接リストの全長の合計とする。$D\le 2U$ である。
- $L$ を色分割中に作られる signature の最大長とする。$L\le 2n+2$ である。
- $S$ を DFS で訪問した状態数、$R$ を全状態を通した色分割更新回数の合計とする。$R\le S(n+1)$ である。
- 探索メモは平衡二分探索木で持つ。状態キーの比較は最悪 $O(n)$ 時間である。
- $C_{\mathrm{build}}=M_1\log(M_1+2)+M_2\log(M_2+2)+n\log(n+2)+n+M_1+M_2$ とする。
- $C_{\mathrm{ref}}=D\log(n+2)+nL\log(n+2)+nL+D+n$ とする。
- $C_{\mathrm{memo}}=n\log(S+2)+n$ とする。
- $C_{\mathrm{check}}=U\log(U+2)+U+n$ とする。
- 構築は $O(C_{\mathrm{build}})$ 時間である。
- 色分割更新 $1$ 回は $O(C_{\mathrm{ref}})$ 時間である。
- 探索メモの検索または挿入は $O(C_{\mathrm{memo}})$ 時間である。
- 色がすべて単独になった状態での辺集合比較は $O(C_{\mathrm{check}})$ 時間である。
- 全体は $O(C_{\mathrm{build}}+RC_{\mathrm{ref}}+S(C_{\mathrm{memo}}+C_{\mathrm{check}}))$ 時間である。
- 最悪の場合、$S$ は指数的に大きくなる。
