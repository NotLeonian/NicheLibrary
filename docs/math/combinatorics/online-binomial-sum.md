---
title: 二項係数の和（オンライン）
documentation_of: math/combinatorics/online-binomial-sum.hpp
---

## 概要

- 整数 $n,m$ と重み $r$ に対する二項係数の prefix sum を $\displaystyle F(n,m)=\sum_{i=0}^{n-1}r^i\binom{m}{i}$ とおく。
- 半開区間の左端、右端をそれぞれ $l,u$ とする。 $\displaystyle \sum_{i=l}^{u-1}r^i\binom{m}{i}$ をオンラインで求める。
- $\binom{m}{i}=0\;(i>m)$ として扱う。
- クエリでは、前計算を行ったうちの最も近い点から復元する。
- バケットサイズを指定できる。
  - 以下、クエリで与えられる $m$ の最大値を $M$ 、バケットサイズを $B$ とする。
  - クエリの時間計算量は $O(B)$ であり、 $B$ が小さいほど各クエリは高速である。
  - 前計算は時間計算量が $O(M^2/B+B^2+M)$ 、空間計算量が $O((M/B)^2+B^2+M)$ である。 $B$ が小さい場合、時空間ともに前計算のコストが大きい。
  - バケットサイズ $B$ を指定しない場合、 $B^2\ge M+1$ を満たす最小の $2$ の冪が使われる。
- $r=0$ のときは閉形式を用い、 $r=-1$ でも $r+1$ による除算は行わない。

## 使い方

`max_m` を $M$ 、`bucket_size` を $B$ とおく。

- `OnlineBinomialSum<T>(int max_m, T r, int bucket_size)`
  - $0\le m\le M$ のクエリに対する前計算を行う。
  - `r` は重みである。
  - 引数の `bucket_size` をバケットサイズとして、前計算を行う。
  - 前提: $M\ge 0,\,B>0$ 。
  - 前提: `T` は四則演算と `T()` との等値比較を持つ。
  - 前提: $r \ne 0$ の場合は `r` で除算できる。
  - 前提: `std::numeric_limits<T>::is_integer` が `false` の場合、 $T(1),T(2),\ldots,T(M)$ で除算できる。
  - 備考: 整数型では中間値が `T` の範囲を超えない必要がある。
- `OnlineBinomialSum<T>(int max_m, T r = T(1))`
  - $0\le m\le M$ のクエリに対する前計算を行う。
  - `r` は重みで、省略時は $1$ である。
  - バケットサイズ $B$ は $B^2>M$ を満たす最小の $2$ の冪として選ばれる。
  - 前提: $M\ge 0$ 。
  - 前提: `T` は四則演算と `T()` との等値比較を持つ。
  - 前提: $r \ne 0$ の場合は `r` で除算できる。
  - 前提: `std::numeric_limits<T>::is_integer` が `false` の場合、 $T(1),T(2),\ldots,T(M)$ で除算できる。
  - 備考: 整数型では中間値が `T` の範囲を超えない必要がある。
- `T binom_prefix_sum(int n, int m) const`
  - $\displaystyle \sum_{i=0}^{n-1}r^i\binom{m}{i}$ を返す。
  - 前提: $n\ge 0,\;0\le m\le M$ 。
  - 備考: $n>m+1$ のときも assert 違反にせず、全体の和を返す。
- `T binom_sum(int l, int u, int m) const`
  - $\displaystyle \sum_{i=l}^{u-1}r^i\binom{m}{i}$ を返す。
  - 前提: $0\le l\le u,\;0\le m\le M$ 。
  - 備考: $u>m+1$ や $l>m$ のときも assert 違反にしない。

## 計算量

`max_m` を $M$ 、`bucket_size` を $B$ とおく。

- コンストラクタ: 時間 $O(M^2/B+B^2+M)$ 、空間 $O((M/B)^2+B^2+M)$
- `binom_prefix_sum(n, m)`: 時間 $O(B)$
- `binom_sum(l, u, m)`: 時間 $O(B)$

コンストラクタで $B$ を指定しない場合、 $B=O(\sqrt M)$ である。
このとき、

- コンストラクタ: 時間 $O(M\sqrt M)$ 、空間 $O(M)$
- `binom_prefix_sum(n, m)`: 時間 $O(\sqrt M)$
- `binom_sum(l, u, m)`: 時間 $O(\sqrt M)$

である。
