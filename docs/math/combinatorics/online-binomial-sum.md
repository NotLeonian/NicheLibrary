---
title: 二項係数の和（オンライン）
documentation_of: math/combinatorics/online-binomial-sum.hpp
---

## 概要

- 整数 $n,m$ と重み $r$ に対する二項係数の prefix sum を $\displaystyle F(n,m)=\sum_{i=0}^{n-1}r^i\binom{m}{i}$ とおく。
- 半開区間の左端、右端をそれぞれ $l,u$ とする。$\displaystyle \sum_{i=l}^{u-1}r^i\binom{m}{i}$ をオンラインで求める。
- $\binom{m}{i}=0\;(i>m)$ として扱う。
- 前計算を行う長さを $k$ とする。$m$ をバケットに分け、境界を $m_0$ とおいて $F(k,m_0)$ を前計算する。
- $d=m-m_0$ とおく。$m=m_0+d$ では $\displaystyle F(n,m)=\sum_{j=0}^{d}r^j\binom{d}{j}F(n-j,m_0)$ を使う。
- $r=0$ や $r=-1$ でも $r+1$ による除算は行わない。

## 使い方

- `max_m` を $M$ とおく。
- `OnlineBinomialSum<T>(int max_m, T r = T(1))`
  - $0\le m\le M$ のクエリに対する前計算を行う。
  - `r` は重みで、省略時は $1$ である。
  - 前提：`T` は四則演算を持つ。`std::numeric_limits<T>::is_integer` が `false` の場合、$T(1),T(2),\ldots,T(M)$ で除算できる。
  - 備考：整数型では中間値が `T` の範囲を超えない必要がある。
- `T binom_prefix_sum(int n, int m) const`
  - $\displaystyle \sum_{i=0}^{n-1}r^i\binom{m}{i}$ を返す。
  - 前提：$n\ge 0,\;0\le m\le M$。
  - 備考：$n>m+1$ のときも assert 違反にせず、全体の和を返す。
- `T binom_sum(int l, int u, int m) const`
  - $\displaystyle \sum_{i=l}^{u-1}r^i\binom{m}{i}$ を返す。
  - 前提：$0\le l\le u,\;0\le m\le M$。
  - 備考：$u>m+1$ や $l>m$ のときも assert 違反にしない。

## 計算量

`max_m` を $M$ とおく。

- コンストラクタ：時間・空間 $O(M\sqrt M)$
- `binom_prefix_sum(n, m)`：時間 $O(\sqrt M)$
- `binom_sum(l, u, m)`：時間 $O(\sqrt M)$
