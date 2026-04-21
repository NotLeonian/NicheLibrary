---
title: 区間インクリメント/デクリメントの最小回数（$\text{mod }m$）
documentation_of: other/minimum-mod-range-increment-decrement-operations.hpp
---

## 概要

- 数列を $\text{mod }m$ 上で見たとき、区間に $+1$ または $-1$ を 1 回加える操作の最小回数を求める。
- 区間に整数 $x$ を加える操作の最小 $\sum \lvert x\rvert$ と同値である。
- $c_i = (b_i - a_i) \bmod m$ の階差を $[0, m)$ で取り、その総和が $Cm$ になることを使って大きい値から $C$ 個を負側に回す。

## 使い方

- `T minimum_mod_range_increment_decrement_operations(std::vector<T> a, std::vector<T> b, T m)`
  - `a` を `b` に一致させるための最小操作回数を返す。
  - 前提：`a.size() == b.size()`, `m > 0`, `0 \le a[i], b[i] < m`。
  - 前提：`T` は 64 bit 整数型、`m \le 10^9` を想定する。
  - 備考：区間に整数 `x` を加える操作を許す場合の最小 $\sum \lvert x\rvert$ と同じ値を返す。
  - 備考：実装は階差の総和そのものは持たず、$\left\lfloor \sum d_i / m \right\rfloor$ を繰り上がりで求める。
  - 備考：返り値が `T` に収まらない場合はオーバーフローする。

## 計算量

- 時間：$O(N \log N)$
- 空間：$O(N)$
