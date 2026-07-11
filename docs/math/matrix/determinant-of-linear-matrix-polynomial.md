---
title: 正方行列の 1 次式の行列式 (yukicoder No.1907 DETERMINATION)
documentation_of: math/matrix/determinant-of-linear-matrix-polynomial.hpp
---

## 概要

正方行列のサイズを $N$ 、入力の正方行列を $M_0,\,M_1$ 、多項式の変数を $x$ とおく。

- 体上の $N\times N$ 行列 $M_0,\,M_1$ に対し、 $\det(M_0+xM_1)$ を次数 $N$ 以下の多項式として求める。
- 変形後の正方行列を $A$ とおく。 $M_1$ を掃き出して単位行列にし、 $\det(xI+A)$ を求める問題を特性多項式の計算に帰着させる。
- $M_1$ が特異な列では、列に $x$ を掛ける操作を挟み、次数 1 の表現を壊さずに進める。

## 使い方

正方行列のサイズを $N$ 、多項式の変数を $x$ とおく。

- `void hessenberg_reduction(std::vector<std::vector<T>>& matrix)`
  - $N\times N$ 行列 `matrix` を上ヘッセンベルグ行列に相似変換する。
  - 前提: `T` は除算ができる（体である）。

- `std::vector<T> characteristic_polynomial(std::vector<std::vector<T>> matrix)`
  - `matrix` を $A$ 、多項式の変数を $x$ として、 $\det(xI-A)$ の係数列を返す（昇順、サイズ $N+1$ 、最高次係数は 1）。
  - 前提: `matrix` は $N\times N$ 行列、`T` は体である。

- `std::vector<T> determinant_of_linear_matrix_polynomial(std::vector<std::vector<T>> M0, std::vector<std::vector<T>> M1)`
  - `M0` を $M_0$ 、`M1` を $M_1$ 、多項式の変数を $x$ として、 $\det(M_0+xM_1)$ の係数列を返す（昇順、サイズ $N+1$ ）。
  - 前提: $M_0,\,M_1$ はともに $N\times N$ 行列、`T` は体。
  - 備考: 内部で列に $x$ を掛ける操作を行った回数だけ、最後に低次の係数を削って補正する。

## 計算量

- `hessenberg_reduction`: 時間 $O(N^3)$
- `characteristic_polynomial`: 時間 $O(N^3)$
- `determinant_of_linear_matrix_polynomial`: 時間 $O(N^3)$
