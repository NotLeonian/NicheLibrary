---
title: Rectangle Add Max Get
documentation_of: other/rectangle-add-max-get.hpp
---

## 概要

- 2 次元平面上に重み付き半開長方形 $[l,r)\times[d,u)$ を追加する。
- 指定範囲内で、重み総和の最大値と、それを達成する辞書順最小/最大の整数格子点を求める。
- 指定範囲内で、重み総和の最大値と、最大値を達成する部分の面積の総和を求める。
- 無引数版の `calc_...()` は、追加された長方形全体の外接長方形を探索範囲にする。
- 4 引数版の `calc_...(l, d, r, u)` は、指定された範囲をそのまま探索範囲にする。
- 各 `calc_...` では、追加された長方形を探索範囲で切り詰めてから座標圧縮と平面走査を行う。

## 使い方

- `RectangleAddMaxGet<T, C>()`
  - 空で構築する。
  - 前提：`T` は `bool` でない整数型である。
  - 前提：`C` は重みの総和が収まる型である。

- `RectangleAddMaxGet<T, C>(int n)`
  - 長方形の個数 `n` を指定して構築する。
  - 前提：`n >= 0`。
  - 備考：内部の領域を `reserve` する。

- `void add_rectangle(T l, T d, T r, T u, C w = 1)`
  - $[l,r)\times[d,u)$ に重み `w` の長方形を追加する。
  - 前提：`l < r`, `d < u`。
  - 備考：`w` は負でもよい。

- `std::tuple<C, T, T> calc_max_lexicographically_minimum_point() const`
  - 追加された長方形全体の外接長方形で、重み総和の最大値と、それを達成する辞書順最小の整数格子点を返す。
  - 返り値は `(最大値, x 座標, y 座標)` である。
  - 備考：長方形が 0 個の場合は `(0, 0, 0)` を返す。

- `std::tuple<C, T, T> calc_max_lexicographically_minimum_point(T l, T d, T r, T u) const`
  - $[l,r)\times[d,u)$ 内の重み総和の最大値と、それを達成する辞書順最小の整数格子点を返す。
  - 返り値は `(最大値, x 座標, y 座標)` である。
  - 前提：`l < r`, `d < u`。
  - 備考：長方形は計算時に $[l,r)\times[d,u)$ で切り詰められる。

- `std::tuple<C, T, T> calc_max_lexicographically_maximum_point() const`
  - 追加された長方形全体の外接長方形で、重み総和の最大値と、それを達成する辞書順最大の整数格子点を返す。
  - 返り値は `(最大値, x 座標, y 座標)` である。
  - 備考：長方形が 0 個の場合は `(0, 0, 0)` を返す。

- `std::tuple<C, T, T> calc_max_lexicographically_maximum_point(T l, T d, T r, T u) const`
  - $[l,r)\times[d,u)$ 内の重み総和の最大値と、それを達成する辞書順最大の整数格子点を返す。
  - 返り値は `(最大値, x 座標, y 座標)` である。
  - 前提：`l < r`, `d < u`。
  - 備考：半開区間なので、最大側の代表点は整数格子点として返す。

- `template <class T2 = T> std::pair<C, T2> calc_max_area() const`
  - 追加された長方形全体の外接長方形で、重み総和の最大値と、最大値を達成する部分の面積の総和を返す。
  - 返り値は `(最大値, 面積)` である。
  - 前提：座標差と面積は `T2` に収まる。
  - 備考：長方形が 0 個の場合は `(0, 0)` を返す。

- `template <class T2 = T> std::pair<C, T2> calc_max_area(T l, T d, T r, T u) const`
  - $[l,r)\times[d,u)$ 内の重み総和の最大値と、最大値を達成する部分の面積の総和を返す。
  - 返り値は `(最大値, 面積)` である。
  - 前提：`l < r`, `d < u`。
  - 前提：座標差と面積は `T2` に収まる。
  - 備考：境界上の値の違いは面積に影響しない。

## 計算量

- `add_rectangle`: $O(1)$
- 各 `calc_...`: $O(N\log N)$
- 空間：$O(N)$
