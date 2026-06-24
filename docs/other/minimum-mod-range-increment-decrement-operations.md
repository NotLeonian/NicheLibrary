---
title: 区間インクリメント/デクリメントの最小回数（$\text{mod }m$）
documentation_of: other/minimum-mod-range-increment-decrement-operations.hpp
---

## 概要

入力される列の各要素を $a_i,\,b_i$、法を $m$ とおく。

- 数列を $\text{mod }m$ 上で見たとき、区間に $+1$ または $-1$ を 1 回加える操作の最小回数を求める。
- 区間に整数 $x$ を加える操作の最小 $\sum \lvert x\rvert$ と同値である。

## 使い方

- `T minimum_mod_range_increment_decrement_operations(std::vector<T> a, std::vector<T> b, T m)`
  - $a$ を $b$ に一致させるための最小操作回数を返す。
  - 前提: `a[i]` を $a_i$、`b[i]` を $b_i$ として、$\lvert a\rvert=\lvert b\rvert,\,m>0,\,0\le a_i,b_i<m$。
  - 前提: `T` は 64 bit 整数型、$m\le 10^9$ を想定する。
  - 備考: 区間に整数 $x$ を加える操作を許す場合の最小 $\sum \lvert x\rvert$ と同じ値を返す。
  - 備考: `a`, `b` から作る階差を $d_i$ とする。実装は $\sum d_i$ そのものは持たず、$\left\lfloor \sum d_i / m \right\rfloor$ を繰り上がりで求める。
  - 備考: 返り値が `T` に収まらない場合はオーバーフローする。

## 計算量

入力列の長さを $N$ とおく。

- 時間: $O(N \log N)$
- 空間: $O(N)$
