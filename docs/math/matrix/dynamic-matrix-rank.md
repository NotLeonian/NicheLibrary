---
title: 1 行・1 列変更後の行列の階数
documentation_of: math/matrix/dynamic-matrix-rank.hpp
---

## 概要

* 体上の $r\times c$ 行列 `matrix` を前処理し、現在の階数と、`A + uv^\top` の階数を求める。
* 1 行差し替え・1 列差し替えを、それぞれ `e_i (b-a_i)^\top`, `(b-a_j)e_j^\top` として高速に判定する。
* 独立な行集合・列集合と、その交差小行列の逆行列を持つ。

## 使い方

* `DynamicMatrixRank(const std::vector<std::vector<T>>& matrix)`
  * `matrix` を与えて前処理する。
  * 前提：`matrix` は長方形であり、`T` は除算ができる（体）。
* `void build()`
  * 現在の `matrix` から内部状態を再構築する。
  * 備考：`matrix` を直接書き換えた後に呼ぶ。
* `int rank() const`
  * 現在保持している `matrix` の階数を返す。
* `int rank_after_rank_one_update(const std::vector<T>& column_vector, const std::vector<T>& row_vector) const`
  * 元の行列を `A` として、`A + column_vector * row_vector^\top` の階数を返す。
  * 前提：`column_vector` の長さは行数、`row_vector` の長さは列数。
* `int rank_after_row_replacement(int row_index, const std::vector<T>& new_row) const`
  * `row_index` 行目を `new_row` に差し替えた行列の階数を返す。
  * 前提：`0 <= row_index <` 行数、`new_row` の長さは列数。
  * 備考：内部状態は変更しない。
* `int rank_after_column_replacement(int column_index, const std::vector<T>& new_column) const`
  * `column_index` 列目を `new_column` に差し替えた行列の階数を返す。
  * 前提：`0 <= column_index <` 列数、`new_column` の長さは行数。
  * 備考：内部状態は変更しない。

## 計算量

* ここで $k$ は元の行列の階数である。
* `DynamicMatrixRank` / `build`：$O(rc\min(r,c) + k^2(r+c))$
* `rank`：$O(1)$
* `rank_after_rank_one_update`：$O(k(r+c))$
* `rank_after_row_replacement`：$O(kc)$
* `rank_after_column_replacement`：$O(kr)$
