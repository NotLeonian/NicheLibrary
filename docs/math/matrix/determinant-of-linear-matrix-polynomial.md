---
title: 正方行列の 1 次式の行列式 (yukicoder No.1907 DETERMINATION)
documentation_of: math/matrix/determinant-of-linear-matrix-polynomial.hpp
---

## 概要

- 体上の $N\times N$ 行列 `M0, M1` に対し、`det(M0 + x M1)` を次数 $N$ 以下の多項式として求める。
- `M1` を掃き出して単位行列にし、`det(xI + A)` を特性多項式計算に帰着する。
- `M1` が特異な列では「列に $x$ を掛ける」操作を挟み、次数 1 の表現を壊さずに進める。

## 使い方

- `void hessenberg_reduction(std::vector<std::vector<T>>& matrix)`
  - `matrix`（$N\times N$）を上ヘッセンベルグ行列へ相似変換する。
  - 前提：`T` は除算ができる（体）。

- `std::vector<T> characteristic_polynomial(std::vector<std::vector<T>> matrix)`
  - `det(xI - matrix)` の係数列を返す（昇順、サイズ $N+1$、最高次係数は 1）。
  - 前提：`matrix` は $N\times N$、`T` は体。

- `std::vector<T> determinant_of_linear_matrix_polynomial(std::vector<std::vector<T>> M0, std::vector<std::vector<T>> M1)`
  - `det(M0 + x M1)` の係数列を返す（昇順、サイズ $N+1$）。
  - 前提：`M0, M1` は同じ $N\times N$、`T` は体。
  - 備考：内部で「列に $x$ を掛ける」操作を行った回数だけ、最後に低次係数を削って補正する。

## 計算量

- `hessenberg_reduction`：$O(N^3)$
- `characteristic_polynomial`：$O(N^3)$
- `determinant_of_linear_matrix_polynomial`：$O(N^3)$