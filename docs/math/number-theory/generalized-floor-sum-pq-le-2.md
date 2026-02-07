---
title: 一般化 floor sum ($p+q\le 2$)
documentation_of: math/number-theory/generalized-floor-sum-pq-le-2.hpp
---

## 概要

- 次をまとめて計算する。
  - $\displaystyle f_{0,1}(n,m,a,b)=\sum_{i=0}^{n-1}\left\lfloor\frac{ai+b}{m}\right\rfloor$
  - $\displaystyle f_{1,1}(n,m,a,b)=\sum_{i=0}^{n-1}i\left\lfloor\frac{ai+b}{m}\right\rfloor$
  - $\displaystyle f_{0,2}(n,m,a,b)=\sum_{i=0}^{n-1}\left\lfloor\frac{ai+b}{m}\right\rfloor^2$
- $a,b$ は負でもよい。

## 使い方

- `generalized_floor_sum_pq_le_2<T>(n, m, a, b)`
  - `GeneralizedFloorSumPQLe2Result<T>` を返す。
  - 前提: `n >= 0`, `m > 0`。
  - 備考: 返り値が `T` の範囲を超える場合、オーバーフローした値が返る。

- `GeneralizedFloorSumPQLe2Result<T>::ans_01`
  - $\displaystyle \sum_{i=0}^{n-1}\left\lfloor\frac{ai+b}{m}\right\rfloor$ を返す。

- `GeneralizedFloorSumPQLe2Result<T>::ans_11`
  - $\displaystyle \sum_{i=0}^{n-1}i\left\lfloor\frac{ai+b}{m}\right\rfloor$ を返す。

- `GeneralizedFloorSumPQLe2Result<T>::ans_02`
  - $\displaystyle \sum_{i=0}^{n-1}\left\lfloor\frac{ai+b}{m}\right\rfloor^2$ を返す。

## 計算量

- 時間: $O(\log m)$
- 空間: $O(\log m)$（再帰）
