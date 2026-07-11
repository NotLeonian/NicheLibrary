---
title: 二項係数の和（オンライン）
documentation_of: math/combinatorics/online-binomial-sum.hpp
---

## 概要

- 整数 $n,m$ と重み $r$ に対する二項係数の prefix sum を $\displaystyle F(n,m)=\sum_{i=0}^{n-1}r^i\binom{m}{i}$ とおく。
- 半開区間の左端、右端をそれぞれ $l,u$ とする。 $\displaystyle \sum_{i=l}^{u-1}r^i\binom{m}{i}$ をオンラインで求める。
- $\binom{m}{i}=0\;(i>m)$ として扱う。
- $n$ と $m$ をそれぞれバケットに分け、バケット境界と各軸の上端をサンプル座標とする。両軸のサンプル座標の直積で $F(n,m)$ を前計算する。
- クエリでは、マンハッタン距離が最小であるサンプル点から $n$ 方向と $m$ 方向に遷移する。
- バケットサイズを指定できる。

以下、クエリで与えられる $m$ の最大値を $M$ 、バケットサイズを $B$ とする。
- $r=0$ の場合は閉形式を用いる。
- $r=-1$ の場合は $\displaystyle F(n,m)=(-1)^{n-1}\binom{m-1}{n-1}$ を用い、 $r+1$ による除算を行わない。ただし、 $n=0$ または $m=0$ の場合は別に処理する。
- $r$ が $0$ でも $-1$ でもない場合、 $n$ 方向の遷移には境界項 $\displaystyle r^n\binom{m}{n}$ を用いる。 $m$ 方向の遷移にはパスカルの三角形の等式から得られる $\displaystyle F(n,m+1)=(r+1)F(n,m)-r^n\binom{m}{n-1}$ を用いる。
- バケットサイズ $B$ を指定しない場合、 $r=0$ であれば $B=1$ とし、既定のバケットサイズを計算しない。
- バケットサイズ $B$ を指定せず、 $r\ne 0$ の場合、 $B^2>M$ を満たす最小の $2$ の冪が選ばれる。

## 使い方

`max_m` を $M$ 、`bucket_size` を $B$ とおく。

- `OnlineBinomialSum(int max_m, T r, int bucket_size)`
  - $0\le m\le M$ のクエリに対する前計算を行う。
  - `r` は重みである。
  - 引数の `bucket_size` をバケットサイズとして前計算を行う。
  - 前提: $M\ge 0,\;B>0$ 。
  - 前提: `T` は素数 $p$ を法とする体の型であり、整数からの構築、四則演算、等値比較を持つ。
  - 前提: `std::numeric_limits<T>::is_integer` は `false` である。
  - 前提: $M<p$ 。
  - 備考: 合成数を法とする型は対象としない。
- `OnlineBinomialSum(int max_m, T r = T(1))`
  - $0\le m\le M$ のクエリに対する前計算を行う。
  - `r` は重みであり、省略時は $1$ である。
  - $r=0$ の場合、バケットサイズ $B$ は $1$ であり、既定のバケットサイズを計算しない。
  - $r\ne 0$ の場合、バケットサイズ $B$ は、 $B^2>M$ を満たす最小の $2$ の冪が選ばれる。
  - 前提: $M\ge 0$ 。
  - 前提: `T` は素数 $p$ を法とする体の型であり、整数からの構築、四則演算、等値比較を持つ。
  - 前提: `std::numeric_limits<T>::is_integer` は `false` である。
  - 前提: $M<p$ 。
  - 備考: 合成数を法とする型は対象としない。
- `T binom_prefix_sum(int n, int m) const`
  - $\displaystyle \sum_{i=0}^{n-1}r^i\binom{m}{i}$ を返す。
  - 前提: $n\ge 0,\;0\le m\le M$ 。
  - 備考: $n>m+1$ の場合も assert 違反にせず、全体の和を返す。
- `T binom_sum(int l, int u, int m) const`
  - $\displaystyle \sum_{i=l}^{u-1}r^i\binom{m}{i}$ を返す。
  - 前提: $0\le l\le u,\;0\le m\le M$ 。
  - 備考: $u>m+1$ または $l>m$ の場合も assert 違反にしない。

## 計算量

`max_m` を $M$ 、`bucket_size` を $B$ とおく。

$r$ が $0$ でも $-1$ でもない場合、

- コンストラクタ: 時間 $O(M^2/B+M)$ 、空間 $O((M/B+1)^2+M)$
- `binom_prefix_sum(n, m)`: 時間 $O(B)$
- `binom_sum(l, u, m)`: 時間 $O(B)$

である。各軸の隣接するサンプル座標の差は高々 $B$ であるため、最も近いサンプル点からクエリ点までの遷移回数は高々 $B$ である。

$r=-1$ の場合、

- コンストラクタ: 時間 $O(M)$ 、空間 $O(M)$
- `binom_prefix_sum(n, m)`: 時間 $O(1)$
- `binom_sum(l, u, m)`: 時間 $O(1)$

である。

$r=0$ の場合、全ての処理は時間 $O(1)$ 、空間 $O(1)$ である。

コンストラクタで $B$ を指定せず、 $r$ が $0$ でも $-1$ でもない場合は $B=O(\sqrt M)$ である。このとき、

- コンストラクタ: 時間 $O(M\sqrt M)$ 、空間 $O(M)$
- `binom_prefix_sum(n, m)`: 時間 $O(\sqrt M)$
- `binom_sum(l, u, m)`: 時間 $O(\sqrt M)$

である。
