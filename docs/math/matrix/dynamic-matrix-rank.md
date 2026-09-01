---
title: 行列の更新に伴う階数の計算
documentation_of: math/matrix/dynamic-matrix-rank.hpp
---

## 概要

現在の行列を $A$ とし、その行数を $r$ 、列数を $c$ 、階数を $k$ とおく。

- 体上の $r \times c$ 行列を前処理し、現在の階数を求める。
- $u$ をサイズ $r$ の列ベクトル、 $v$ をサイズ $c$ の列ベクトルとして、 $A + uv^{\top}$ の階数を求める。
- さらに、1 つの行または列を差し替えたり、1 つの外積を加えたりして、内部状態を更新できる。
- サイズが $r\times k$ の行列を $C$ とし、サイズが $k\times c$ の行列を $R$ とする。行列 $A$ の階数分解 $A=CR$ を保持する。
- $C$ の左逆行列と $R$ の右逆行列も保持する。

## 使い方

以降、`row_index` を $i$ 、`column_index` を $j$ とおいて説明することがある。
また、`column_vector` を $u$ 、`row_vector` を $v$ とおいて説明することがある。

- `DynamicMatrixRank()`
  - 0 行 0 列の行列として構築する。
- `DynamicMatrixRank(const std::vector<std::vector<T>>& matrix)`
  - `matrix` を現在の行列として構築する。
  - 前提: `matrix` は長方形であり、`T` は体をなす。
- `void build(const std::vector<std::vector<T>>& matrix)`
  - `matrix` を現在の行列とし、前処理をやり直す。
  - 前提: `matrix` は長方形であり、`T` は体をなす。
- `void build()`
  - 現在の行列を密行列として復元し、前処理をやり直す。
- `int rank() const`
  - 現在の行列の階数を返す。
- `std::vector<T> get_row(int row_index) const`
  - 現在の行列について、`row_index` で指定した行を返す。
  - 前提: $0\le i<r$ 。
- `std::vector<T> get_column(int column_index) const`
  - 現在の行列について、`column_index` で指定した列を返す。
  - 前提: $0\le j<c$ 。
- `std::vector<std::vector<T>> materialize_matrix() const`
  - 現在の行列を密行列として返す。
- `int rank_after_rank_one_update(const std::vector<T>& column_vector, const std::vector<T>& row_vector) const`
  - $A+uv^{\top}$ の階数を返す。
  - 前提: `column_vector` の長さは行数に等しく、`row_vector` の長さは列数に等しい。
  - 備考: 内部状態は変更しない。
- `int rank_after_row_replacement(int row_index, const std::vector<T>& new_row) const`
  - `row_index` 行目を `new_row` に差し替えた行列の階数を返す。
  - 前提: $0\le i<r$ 、`new_row` の長さは列数に等しい。
  - 備考: 内部状態は変更しない。
- `int rank_after_column_replacement(int column_index, const std::vector<T>& new_column) const`
  - `column_index` 列目を `new_column` に差し替えた行列の階数を返す。
  - 前提: $0\le j<c$ 、`new_column` の長さは行数に等しい。
  - 備考: 内部状態は変更しない。
- `int apply_rank_one_update(const std::vector<T>& column_vector, const std::vector<T>& row_vector)`
  - 内部状態が表す行列を $A+uv^{\top}$ に更新し、その階数を返す。
  - 前提: `column_vector` の長さは行数に等しく、`row_vector` の長さは列数に等しい。
- `int apply_row_replacement(int row_index, const std::vector<T>& new_row)`
  - `row_index` 行目を `new_row` に差し替え、変更後の階数を返す。
  - 前提: $0\le i<r$ 、`new_row` の長さは列数に等しい。
- `int apply_column_replacement(int column_index, const std::vector<T>& new_column)`
  - `column_index` 列目を `new_column` に差し替え、変更後の階数を返す。
  - 前提: $0\le j<c$ 、`new_column` の長さは行数に等しい。

## 計算量

現在の行列 $A$ の階数を $k$ とする。

- `build`: 時間 $O(r + rc\min(r, c) + k^2(r + c))$
- `rank`: 時間 $O(1)$
- `get_row`: 時間 $O((k + 1)c + 1)$
- `get_column`: 時間 $O((k + 1)r)$
- `materialize_matrix`: 時間 $O((k + 1)rc + r)$
- `rank_after_rank_one_update`: 時間 $O((k + 1)(r + c))$
- `rank_after_row_replacement`: 時間 $O((k + 1)(r + c))$
- `rank_after_column_replacement`: 時間 $O((k + 1)(r + c))$
- `apply_rank_one_update`: 時間 $O((k + 1)(r + c))$
- `apply_row_replacement`: 時間 $O((k + 1)(r + c))$
- `apply_column_replacement`: 時間 $O((k + 1)(r + c))$
