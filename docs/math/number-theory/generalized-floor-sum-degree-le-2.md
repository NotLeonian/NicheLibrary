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
- `Internal` は内部で計算結果の管理に用いる整数型である。
- `Internal` を省略した場合、標準の 64 bit 以下の整数型では `NicheLibrary::Int128` を用いる。
- 状態変数 $n,m,a,b$ は `T` のまま扱う。

## 使い方

- `generalized_floor_sum_degree_le_2<T, Internal>(n, m, a, b)`
  - `GeneralizedFloorSumDegreeLe2Result<T>` を返す。
  - `Internal` は内部で計算結果の管理に用いる整数型である。
  - `Internal` は省略できる。省略した場合、標準の 64 bit 以下の整数型では `NicheLibrary::Int128` を用いる。
  - 前提: `T` と明示的に与える場合の `Internal` は整数型である。
  - 前提: `T` が符号付きならば、明示的に与える場合の `Internal` も符号付きである。
  - 前提: $n\ge 0,\;m>0$ 。
  - 前提: 内部計算が `Internal` の範囲に収まる。
  - 備考: `T` が符号付き整数型の場合、返り値が `T` の範囲に収まる必要がある。
  - 備考: `T` が符号なし整数型の場合、内部計算の結果を `T` に変換した値を返す。
  - 備考: $n,m,a,b$ および答えが 64 bit 整数型に収まる場合は、`T` を `NicheLibrary::Int128` にせず、`Internal` のみを広い型にすることが望ましい。

- `GeneralizedFloorSumDegreeLe2Result<T>::ans_01`
  - $\displaystyle \sum_{i=0}^{n-1}\left\lfloor\frac{ai+b}{m}\right\rfloor$ を返す。

- `GeneralizedFloorSumDegreeLe2Result<T>::ans_11`
  - $\displaystyle \sum_{i=0}^{n-1}i\left\lfloor\frac{ai+b}{m}\right\rfloor$ を返す。

- `GeneralizedFloorSumDegreeLe2Result<T>::ans_02`
  - $\displaystyle \sum_{i=0}^{n-1}\left\lfloor\frac{ai+b}{m}\right\rfloor^2$ を返す。

## 計算量

- 時間 $O(\log m)$
- 空間 $O(\log m)$ （再帰）
