---
title: 行列の階数と動的な 1 行・1 列更新
documentation_of: math/matrix/dynamic-matrix-rank.hpp
---

## 概要

現在の行列を $A$ とし、その行数を $r$、列数を $c$ とおく。

- 体上の $r \times c$ 行列を前処理し、現在の階数を求める。
- $u$ を $r$ 列のベクトル、$v$ を $c$ 列のベクトルとして、$A + uv^\top$ の階数を求められる。
- さらに、1 行差し替え・1 列差し替え・外積 1 項更新を内部状態に反映できる。
- 現在の行列は左右の階数分解と片側逆元で保持する。

## 使い方

以降、`row_index` を $i$、`column_index` を $j$ とおいて説明することがある。
また、`column_vector` を $u$、`row_vector` を $v$ とおいて説明することがある。

- `DynamicMatrixRank()`
  - 空に構築する。後で `build(matrix)` を呼ぶ。
- `DynamicMatrixRank(const std::vector<std::vector<T>>& matrix)`
  - `matrix` で構築する。
  - 前提：`matrix` は長方形であり、`T` は除算ができる。
- `void build(const std::vector<std::vector<T>>& matrix)`
  - `matrix` を現在の行列として前処理し直す。
- `void build()`
  - 現在保持している行列から前処理し直す。
- `int rank() const`
  - 現在の行列の階数を返す。
- `std::vector<T> get_row(int row_index) const`
  - 現在の `row_index` 行目を返す。
  - 前提：$0\le i<r$。
- `std::vector<T> get_column(int column_index) const`
  - 現在の `column_index` 列目を返す。
  - 前提：$0\le j<c$。
- `std::vector<std::vector<T>> materialize_matrix() const`
  - 現在の行列を密行列として返す。
- `int rank_after_rank_one_update(const std::vector<T>& column_vector, const std::vector<T>& row_vector) const`
  - $A+uv^{\top}$ の階数を返す。
  - 前提：`column_vector` の長さは行数、`row_vector` の長さは列数。
  - 備考：内部状態は変更しない。
- `int rank_after_row_replacement(int row_index, const std::vector<T>& new_row) const`
  - `row_index` 行目を `new_row` に差し替えた行列の階数を返す。
  - 前提：$0\le i<r$、`new_row` の長さは列数。
  - 備考：内部状態は変更しない。
- `int rank_after_column_replacement(int column_index, const std::vector<T>& new_column) const`
  - `column_index` 列目を `new_column` に差し替えた行列の階数を返す。
  - 前提：$0\le j<c$、`new_column` の長さは行数。
  - 備考：内部状態は変更しない。
- `int apply_rank_one_update(const std::vector<T>& column_vector, const std::vector<T>& row_vector)`
  - $A+uv^{\top}$ に内部状態を更新し、その階数を返す。
- `int apply_row_replacement(int row_index, const std::vector<T>& new_row)`
  - `row_index` 行目を `new_row` に差し替え、変更後の階数を返す。
- `int apply_column_replacement(int column_index, const std::vector<T>& new_column)`
  - `column_index` 列目を `new_column` に差し替え、変更後の階数を返す。

## 計算量

$k$ は現在の行列 $A$ の階数とする。

- `build`：$O(rc\min(r, c) + k^2(r + c))$
- `rank`：$O(1)$
- `get_row`：$O(kc)$
- `get_column`：$O(kr)$
- `materialize_matrix`：$O(krc)$
- `rank_after_rank_one_update`：$O(k(r + c))$
- `rank_after_row_replacement`：$O(k(r + c))$
- `rank_after_column_replacement`：$O(k(r + c))$
- `apply_rank_one_update`：$O(k(r + c))$
- `apply_row_replacement`：$O(k(r + c))$
- `apply_column_replacement`：$O(k(r + c))$
