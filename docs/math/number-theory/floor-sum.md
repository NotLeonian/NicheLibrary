---
title: floor sum
documentation_of: math/number-theory/floor-sum.hpp
---

## 概要

- 引数 $n,m,a,b$ に対し、$\displaystyle \sum_{i=0}^{n-1}\left\lfloor\frac{a i+b}{m}\right\rfloor$ を求める。
- $a,b$ は負でもよい（$m>0$）。
- $a,b$ を $0\le a,b<m$ に帰着してから、ユークリッドの互除法と同型の遷移で計算する。

## 使い方

- `T floor_sum(T n, T m, T a, T b)`
  - $\displaystyle \sum_{i=0}^{n-1}\left\lfloor\frac{a i+b}{m}\right\rfloor$ を返す。
  - 前提：`std::numeric_limits<T>::is_integer` が `true` である。
  - 前提：$n\ge 0,\;m>0$。
  - 備考：`T` が符号付きのとき、$a,b$ は負でもよい。戻り値も負になり得る。
  - 備考：中間計算と戻り値が `T` の範囲を超えない必要がある。

## 計算量

- $O(\log m)$
