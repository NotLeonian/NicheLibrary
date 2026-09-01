---
title: 法 $m$ で 2 つの数列を一致させるために区間の各要素をインクリメントまたはデクリメントする最小回数
documentation_of: other/minimum-mod-range-increment-decrement-operations.hpp
---

## 概要

長さ $n$ の 2 つの列を $a, b$ とし、それぞれの $i$ 番目の要素を $a_i, b_i$ とする。また、法を $m$ とする。

- 各要素を法 $m$ で扱い、 $a$ を $b$ に一致させるために、連続区間の全要素に $+1$ または $-1$ を加える操作の最小回数を求める。
- 連続区間に任意の整数を加える操作を許し、 $j$ 回目の操作で加える整数を $x_j$ とすると、求める値は $\sum_j \lvert x_j \rvert$ の最小値とも等しい。

## 使い方

- `T minimum_mod_range_increment_decrement_operations(const std::vector<T> &a, const std::vector<T> &b, T m)`
  - $a$ を $b$ に一致させるために必要な最小操作回数を返す。
  - 前提: $\lvert a \rvert = \lvert b \rvert, m > 0, 0 \le a_i, b_i < m$ 。
  - 前提: `T` は 64 bit 以上の整数型である。
  - 前提: 返り値が `T` の範囲に収まる。
  - 備考: $a_i$ から $b_i$ への法 $m$ での差として、 $[0,m)$ に属する整数を並べ、その両端に $0$ を補う。この列で隣り合う値の法 $m$ での差を $d_i$ とすると、実装は $\sum_i d_i$ そのものを保持せず、加算時に繰り上がった回数から $\sum_i d_i / m$ を求める。

## 計算量

入力列の長さを $N$ とおく。

- 平均時間計算量: $O(N)$
- 空間計算量: $O(N)$
