---
title: Rectangle Add Max Get
documentation_of: other/rectangle-add-max-get.hpp
---

## 概要

長方形の左端を $l$、下端を $d$、右端を $r$、上端を $u$ とおく。

- 2 次元平面上に重み付き半開長方形 $[l,r)\times[d,u)$ を追加する。
- 重み総和の最大値と、それを達成する辞書順最小/最大の整数格子点を求める。
- 重み総和の最大値と、最大値を達成する部分の面積の総和を求める。
- `RectangleAddMaxGet` は座標圧縮しない。小さい整数座標範囲向けである。
- `CompressedRectangleAddMaxGet` は座標圧縮する。広い座標範囲向けである。
- `RectangleAddMaxGet` は、y 座標の範囲が x 座標に依存する範囲にも対応する。

## 使い方

以降、`lower_y` が表す関数を $f$、`upper_y` が表す関数を $g$ とおいて説明することがある。

- `RectangleAddMaxGet<T, C>()`
  - 座標圧縮なし版を空で構築する。
  - 前提：`T` は `bool` でない整数型である。
  - 前提：`C` は `0`, `+=`, `-=`, `==`, 比較を持つ。

- `RectangleAddMaxGet<T, C>(int n)`
  - 座標圧縮なし版を構築する。
  - 前提：$n\ge 0$。
  - 備考：内部の領域を `reserve` する。

- `CompressedRectangleAddMaxGet<T, C>()`
  - 座標圧縮版を空で構築する。
  - 前提：`T` は `bool` でない整数型である。
  - 前提：`C` は `0`, `+=`, `-=`, `==`, 比較を持つ。

- `CompressedRectangleAddMaxGet<T, C>(int n)`
  - 座標圧縮版を構築する。
  - 前提：$n\ge 0$。
  - 備考：内部の領域を `reserve` する。

- `void add_rectangle(T l, T d, T r, T u, C w = 1)`
  - $[l,r)\times[d,u)$ に重み $w$ の長方形を追加する。
  - 前提：$l\le r,\,d\le u$。
  - 前提：$l<r,\,d<u$ かつ `C` が符号付き整数型の場合、`std::numeric_limits<C>::lowest()` は指定しない。
  - 備考：$l=r$ または $d=u$ の場合は追加しない。
  - 備考：$w$ は負でもよい。
  - 備考：面積 0 の長方形は、無引数版 `calc_...()` の外接長方形にも含まれない。

- `std::vector<Rectangle> rectangles`
  - 追加された正の面積の長方形を持つ。

- `std::tuple<C, T, T> calc_max_lexicographically_minimum_point() const`
  - 追加された長方形全体の外接長方形で、最大値と、それを達成する辞書順最小の整数格子点を返す。
  - 返り値は $(\text{最大値},\,x\text{座標},\,y\text{座標})$ である。
  - 備考：長方形が 0 個の場合は $(0,0,0)$ を返す。

- `std::tuple<C, T, T> calc_max_lexicographically_minimum_point(T l, T d, T r, T u) const`
  - $[l,r)\times[d,u)$ 内の最大値と、それを達成する辞書順最小の整数格子点を返す。
  - 返り値は $(\text{最大値},\,x\text{座標},\,y\text{座標})$ である。
  - 前提：$l<r,\,d<u$。
  - 備考：長方形は計算時に $[l,r)\times[d,u)$ で切り詰められる。

- `template <class Lower, class Upper> std::tuple<C, T, T> calc_max_lexicographically_minimum_point(T l, T r, Lower lower_y, Upper upper_y) const`
  - $x$ 座標について、範囲 $[l,r)$ かつ $[f(x),g(x))$ 内の最大値と、それを達成する辞書順最小の整数格子点を返す。
  - `RectangleAddMaxGet` のみで使える。
  - 返り値は $(\text{最大値},\,x\text{座標},\,y\text{座標})$ である。
  - 前提：$l<r$。
  - 前提：各 $x$ で $f(x)\le g(x)$。
  - 前提：$f(x)<g(x)$ となる $x$ が存在する。

- `std::tuple<C, T, T> calc_max_lexicographically_maximum_point() const`
  - 追加された長方形全体の外接長方形で、最大値と、それを達成する辞書順最大の整数格子点を返す。
  - 返り値は $(\text{最大値},\,x\text{座標},\,y\text{座標})$ である。
  - 備考：長方形が 0 個の場合は $(0,0,0)$ を返す。

- `std::tuple<C, T, T> calc_max_lexicographically_maximum_point(T l, T d, T r, T u) const`
  - $[l,r)\times[d,u)$ 内の最大値と、それを達成する辞書順最大の整数格子点を返す。
  - 返り値は $(\text{最大値},\,x\text{座標},\,y\text{座標})$ である。
  - 前提：$l<r,\,d<u$。
  - 備考：半開区間なので、最大側の代表点は整数格子点として返す。

- `template <class Lower, class Upper> std::tuple<C, T, T> calc_max_lexicographically_maximum_point(T l, T r, Lower lower_y, Upper upper_y) const`
  - $x$ 座標について、範囲 $[l,r)$ かつ $[f(x),g(x))$ 内の最大値と、それを達成する辞書順最大の整数格子点を返す。
  - `RectangleAddMaxGet` のみで使える。
  - 返り値は $(\text{最大値},\,x\text{座標},\,y\text{座標})$ である。
  - 前提：$l<r$。
  - 前提：各 $x$ で $f(x)\le g(x)$。
  - 前提：$f(x)<g(x)$ となる $x$ が存在する。

- `template <class T2 = T> std::pair<C, T2> calc_max_area() const`
  - 追加された長方形全体の外接長方形で、最大値と、最大値を達成する部分の面積の総和を返す。
  - 返り値は $(\text{最大値},\,\text{面積})$ である。
  - 前提：座標差と面積は `T2` に収まる。
  - 備考：長方形が 0 個の場合は $(0,0)$ を返す。

- `template <class T2 = T> std::pair<C, T2> calc_max_area(T l, T d, T r, T u) const`
  - $[l,r)\times[d,u)$ 内の最大値と、最大値を達成する部分の面積の総和を返す。
  - 返り値は $(\text{最大値},\,\text{面積})$ である。
  - 前提：$l<r,\,d<u$。
  - 前提：座標差と面積は `T2` に収まる。
  - 備考：境界上の値の違いは面積に影響しない。

- `template <class T2 = T, class Lower, class Upper> std::pair<C, T2> calc_max_area(T l, T r, Lower lower_y, Upper upper_y) const`
  - $x$ 座標について、範囲 $[l,r)$ かつ $[f(x),g(x))$ 内の最大値と、最大値を達成する部分の面積の総和を返す。
  - `RectangleAddMaxGet` のみで使える。
  - 返り値は $(\text{最大値},\,\text{面積})$ である。
  - 前提：$l<r$。
  - 前提：各 $x$ で $f(x)\le g(x)$。
  - 前提：$f(x)<g(x)$ となる $x$ が存在する。
  - 前提：座標差と面積は `T2` に収まる。

## 計算量

長方形範囲の引数を $l,d,r,u$、追加済み長方形数を $N$ とおき、$W=r-l,\;H=u-d$ とおく。

また、可変 y 範囲について、`lower_y` が表す関数を $f$、`upper_y` が表す関数を $g$ とおく。
そして、$x$ を範囲 $[l,r)$ の座標として、$Y=\max_x g(x)-\min_x f(x)$ とおく。

- `add_rectangle`: $O(1)$
- `CompressedRectangleAddMaxGet` の各 `calc_...`: $O(N\log N)$
- `RectangleAddMaxGet` の長方形範囲の各 `calc_...`: $O(H+W+N\log H)$
- `RectangleAddMaxGet` の可変 y 範囲の各 `calc_...`: $O(Y+(W+N)\log Y)$
- `RectangleAddMaxGet` では、$W,\,H,\,Y$ は `int` に収まる必要がある。
