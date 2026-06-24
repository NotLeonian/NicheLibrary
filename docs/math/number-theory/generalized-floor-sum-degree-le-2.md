---
title: 一般化 floor sum（次数 2 以下）
documentation_of: math/number-theory/generalized-floor-sum-degree-le-2.hpp
---

## 概要

- 引数 $n,m,a,b$ に対し、以下をまとめて計算する。
  - $\displaystyle f_{0,1}(n,m,a,b)=\sum_{i=0}^{n-1}\left\lfloor\frac{ai+b}{m}\right\rfloor$
  - $\displaystyle f_{1,1}(n,m,a,b)=\sum_{i=0}^{n-1}i\left\lfloor\frac{ai+b}{m}\right\rfloor$
  - $\displaystyle f_{0,2}(n,m,a,b)=\sum_{i=0}^{n-1}\left\lfloor\frac{ai+b}{m}\right\rfloor^2$
- $a,b$ は負でもよい。

## 使い方

- `generalized_floor_sum_degree_le_2<T, Internal>(n, m, a, b)`
  - `GeneralizedFloorSumDegreeLe2Result<T>` を返す。
  - `Internal` を省略した場合は `T` と同じ型になる。
  - 前提: `T` と `Internal` は `std::numeric_limits<...>::is_integer` が `true` である。
  - 前提: `T` が符号付きなら、`Internal` も符号付きである。
  - 前提: $n\ge 0,\;m>0$。
  - 前提: $n,m,a,b$ は `Internal` に変換でき、内部計算が `Internal` の範囲に収まる。
  - 前提: 返り値は `T` の範囲に収まる。
  - 備考: `T` が符号付きなら $a,b$ は負でもよい。

- `GeneralizedFloorSumDegreeLe2Result<T>::ans_01`
  - $\displaystyle \sum_{i=0}^{n-1}\left\lfloor\frac{ai+b}{m}\right\rfloor$ を返す。

- `GeneralizedFloorSumDegreeLe2Result<T>::ans_11`
  - $\displaystyle \sum_{i=0}^{n-1}i\left\lfloor\frac{ai+b}{m}\right\rfloor$ を返す。

- `GeneralizedFloorSumDegreeLe2Result<T>::ans_02`
  - $\displaystyle \sum_{i=0}^{n-1}\left\lfloor\frac{ai+b}{m}\right\rfloor^2$ を返す。

## 計算量

- 時間: $O(\log m)$
- 空間: $O(\log m)$（再帰）
