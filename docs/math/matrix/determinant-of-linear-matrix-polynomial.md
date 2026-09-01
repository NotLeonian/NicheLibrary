---
title: 1 次行列多項式の行列式（yukicoder No.1907 DETERMINATION）
documentation_of: math/matrix/determinant-of-linear-matrix-polynomial.hpp
---

## 概要

正方行列のサイズを $N$ 、入力の正方行列を $M_0,\,M_1$ 、変形後の $M_0$ を $A$ 、多項式の変数を $x$ とおく。

- 体上の $N\times N$ 行列 $M_0,\,M_1$ に対し、 $\det(M_0+xM_1)$ を次数 $N$ 以下の多項式として求める。
- 行基本変形と列基本変形を用いて $M_1$ を単位行列にし、 $\det(xI+A)$ を求める問題を特性多項式の計算に帰着させる。
- $M_1$ の列を処理するとき、掃き出しで使えるピボットがない場合は、まずその列の $M_1$ の成分を全て $0$ にする。次に、行列多項式の同じ列に $x$ を掛け、行列式に生じる因子 $x$ を記録する。

## 使い方

正方行列のサイズを $N$ 、多項式の変数を $x$ とおく。

- `void hessenberg_reduction(std::vector<std::vector<T>>& matrix)`
  - $N\times N$ 行列 `matrix` を上ヘッセンベルグ行列に相似変換する。
  - 前提: `T` は体である。

- `std::vector<T> characteristic_polynomial(std::vector<std::vector<T>> matrix)`
  - `matrix` を $A$ 、多項式の変数を $x$ として、 $\det(xI-A)$ の係数列を返す（昇順、サイズ $N+1$ 、最高次係数は 1）。
  - 前提: `matrix` は $N\times N$ 行列であり、`T` は体である。

- `std::vector<T> determinant_of_linear_matrix_polynomial(std::vector<std::vector<T>> M0, std::vector<std::vector<T>> M1)`
  - `M0` を $M_0$ 、`M1` を $M_1$ 、多項式の変数を $x$ として、 $\det(M_0+xM_1)$ の係数列を返す（昇順、サイズ $N+1$ ）。
  - 前提: $M_0,\,M_1$ はともに $N\times N$ 行列であり、`T` は体である。
  - 備考: 列に $x$ を掛けた回数だけ、得られた係数列の先頭から係数を削除し、記録した $x$ の因子を取り除く。

## 計算量

- `hessenberg_reduction`: 時間 $O(N^3)$
- `characteristic_polynomial`: 時間 $O(N^3)$
- `determinant_of_linear_matrix_polynomial`: 時間 $O(N^3)$
