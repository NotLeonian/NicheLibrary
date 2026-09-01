---
title: Rectangle Add Max Get
documentation_of: other/rectangle-add-max-get.hpp
---

## 概要

長方形の左端を $l$ 、下端を $d$ 、右端を $r$ 、上端を $u$ とおく。

- 2 次元平面上に重み付き半開長方形 $[l,r)\times[d,u)$ を追加する。
- 指定した範囲内の各点で、その点を含む長方形の重みを合計する。
- 指定した範囲における重みの総和の最大値と、その最大値を達成する辞書順最小または最大の整数格子点を求める。
- 指定した範囲における重みの総和の最大値と、その最大値を達成する領域の面積を求める。
- `RectangleAddMaxGet` は座標圧縮を行わないため、小さい整数座標の範囲に適している。
- `CompressedRectangleAddMaxGet` は座標圧縮を行うため、広い座標の範囲に適している。
- `RectangleAddMaxGet` は、y 座標の範囲が x 座標に依存する範囲にも対応する。

## 使い方

以降、`lower_y` が表す関数を $f$ 、`upper_y` が表す関数を $g$ とする。可変 y 範囲を受け取る API は、各整数 $x$ について、幅 $1$ の半開長方形 $[x,x+1)\times[f(x),g(x))$ を計算対象とする。

- `RectangleAddMaxGet<T, C>()`
  - 座標圧縮を行わない `RectangleAddMaxGet` を空の状態で構築する。
  - 前提: `T` は `bool` でない整数型である。
  - 前提: `C()` と `C(1)` はそれぞれ $0$ と $1$ を表す。`C` はコピーおよびムーブが可能であり、`+=`, `-=`, `==`, `<`, `>` を使用できる。
  - 前提: 計算中に生じる重みの加減算結果は `C` の範囲に収まる。

- `RectangleAddMaxGet<T, C>(int n)`
  - 座標圧縮を行わない `RectangleAddMaxGet` を構築する。
  - 前提: $n\ge 0$ 。
  - 備考: $n$ 個の長方形を格納できるように、`rectangles` の容量をあらかじめ確保する。

- `CompressedRectangleAddMaxGet<T, C>()`
  - 座標圧縮を行う `CompressedRectangleAddMaxGet` を空の状態で構築する。
  - 前提: `T` は `bool` でない整数型である。
  - 前提: `C()` と `C(1)` はそれぞれ $0$ と $1$ を表す。`C` はコピーおよびムーブが可能であり、`+=`, `-=`, `==`, `<`, `>` を使用できる。
  - 前提: 計算中に生じる重みの加減算結果は `C` の範囲に収まる。

- `CompressedRectangleAddMaxGet<T, C>(int n)`
  - 座標圧縮を行う `CompressedRectangleAddMaxGet` を構築する。
  - 前提: $n\ge 0$ 。
  - 備考: $n$ 個の長方形を格納できるように、`rectangles` の容量をあらかじめ確保する。

- `void add_rectangle(T l, T d, T r, T u, C w = 1)`
  - $[l,r)\times[d,u)$ に重み $w$ の長方形を追加する。
  - 前提: $l\le r,\,d\le u$ 。
  - 前提: $l<r,\,d<u$ かつ `C` が符号付き整数型の場合、`std::numeric_limits<C>::lowest()` は指定しない。
  - 備考: $l=r$ または $d=u$ の場合は追加しない。
  - 備考: $w$ は負でもよい。
  - 備考: 面積 0 の長方形は、無引数版 `calc_...()` の外接長方形にも含まれない。

- `std::vector<Rectangle> rectangles`
  - 追加された正の面積の長方形を持つ。

- `std::tuple<C, T, T> calc_max_lexicographically_minimum_point() const`
  - 追加した長方形全体の外接長方形内で、重みの総和の最大値と、その最大値を達成する辞書順最小の整数格子点を返す。
  - 返り値は $(\text{最大値},\,x\text{座標},\,y\text{座標})$ である。
  - 備考: 長方形が 0 個の場合は $(0,0,0)$ を返す。

- `std::tuple<C, T, T> calc_max_lexicographically_minimum_point(T l, T d, T r, T u) const`
  - $[l,r)\times[d,u)$ 内で、重みの総和の最大値と、その最大値を達成する辞書順最小の整数格子点を返す。
  - 返り値は $(\text{最大値},\,x\text{座標},\,y\text{座標})$ である。
  - 前提: $l<r,\,d<u$ 。
  - 備考: 追加した各長方形と $[l,r)\times[d,u)$ の共通部分だけを計算に含める。

- `template <class Lower, class Upper> std::tuple<C, T, T> calc_max_lexicographically_minimum_point(T l, T r, Lower lower_y, Upper upper_y) const`
  - $l\le x<r$ である各整数 $x$ について指定した可変 y 範囲内で、重みの総和の最大値と、その最大値を達成する辞書順最小の整数格子点を返す。
  - `RectangleAddMaxGet` のみで使える。
  - 返り値は $(\text{最大値},\,x\text{座標},\,y\text{座標})$ である。
  - 前提: $l<r$ 。
  - 前提: 各整数 $x$ で $f(x)\le g(x)$ 。
  - 前提: $l\le x<r$ を満たす整数 $x$ のうち、 $f(x)<g(x)$ となるものが存在する。
  - 前提: $f(x)$ と $g(x)$ の返り値は、値を変えずに `T` へ変換できる。

- `std::tuple<C, T, T> calc_max_lexicographically_maximum_point() const`
  - 追加した長方形全体の外接長方形内で、重みの総和の最大値と、その最大値を達成する辞書順最大の整数格子点を返す。
  - 返り値は $(\text{最大値},\,x\text{座標},\,y\text{座標})$ である。
  - 備考: 長方形が 0 個の場合は $(0,0,0)$ を返す。

- `std::tuple<C, T, T> calc_max_lexicographically_maximum_point(T l, T d, T r, T u) const`
  - $[l,r)\times[d,u)$ 内で、重みの総和の最大値と、その最大値を達成する辞書順最大の整数格子点を返す。
  - 返り値は $(\text{最大値},\,x\text{座標},\,y\text{座標})$ である。
  - 前提: $l<r,\,d<u$ 。
  - 備考: 右端と上端を含まないため、半開長方形全体で最大値を達成する場合の辞書順最大の整数格子点は $(r-1,u-1)$ である。

- `template <class Lower, class Upper> std::tuple<C, T, T> calc_max_lexicographically_maximum_point(T l, T r, Lower lower_y, Upper upper_y) const`
  - $l\le x<r$ である各整数 $x$ について指定した可変 y 範囲内で、重みの総和の最大値と、その最大値を達成する辞書順最大の整数格子点を返す。
  - `RectangleAddMaxGet` のみで使える。
  - 返り値は $(\text{最大値},\,x\text{座標},\,y\text{座標})$ である。
  - 前提: $l<r$ 。
  - 前提: 各整数 $x$ で $f(x)\le g(x)$ 。
  - 前提: $l\le x<r$ を満たす整数 $x$ のうち、 $f(x)<g(x)$ となるものが存在する。
  - 前提: $f(x)$ と $g(x)$ の返り値は、値を変えずに `T` へ変換できる。

- `template <class T2 = T> std::pair<C, T2> calc_max_area() const`
  - 追加した長方形全体の外接長方形内で、重みの総和の最大値と、その最大値を達成する領域の面積を返す。
  - 返り値は $(\text{最大値},\,\text{面積})$ である。
  - 前提: `T2()` は $0$ を表す。座標差を値を変えずに `T2` へ変換でき、`T2` で加算代入ができる。`CompressedRectangleAddMaxGet` では乗算もできる。
  - 前提: 座標差と面積は `T2` の範囲に収まる。
  - 備考: 長方形が 0 個の場合は $(0,0)$ を返す。

- `template <class T2 = T> std::pair<C, T2> calc_max_area(T l, T d, T r, T u) const`
  - $[l,r)\times[d,u)$ 内で、重みの総和の最大値と、その最大値を達成する領域の面積を返す。
  - 返り値は $(\text{最大値},\,\text{面積})$ である。
  - 前提: $l<r,\,d<u$ 。
  - 前提: `T2()` は $0$ を表す。座標差を値を変えずに `T2` へ変換でき、`T2` で加算代入ができる。`CompressedRectangleAddMaxGet` では乗算もできる。
  - 前提: 座標差と面積は `T2` の範囲に収まる。
  - 備考: 境界上で重みの総和が変わっても、境界の面積は $0$ なので返り値の面積には影響しない。

- `template <class T2 = T, class Lower, class Upper> std::pair<C, T2> calc_max_area(T l, T r, Lower lower_y, Upper upper_y) const`
  - $l\le x<r$ である各整数 $x$ について指定した可変 y 範囲内で、重みの総和の最大値と、その最大値を達成する領域の面積を返す。
  - `RectangleAddMaxGet` のみで使える。
  - 返り値は $(\text{最大値},\,\text{面積})$ である。
  - 前提: $l<r$ 。
  - 前提: 各整数 $x$ で $f(x)\le g(x)$ 。
  - 前提: $l\le x<r$ を満たす整数 $x$ のうち、 $f(x)<g(x)$ となるものが存在する。
  - 前提: $f(x)$ と $g(x)$ の返り値は、値を変えずに `T` へ変換できる。
  - 前提: `T2()` は $0$ を表す。座標差を値を変えずに `T2` へ変換でき、`T2` で加算代入ができる。
  - 前提: 座標差と面積は `T2` の範囲に収まる。

## 計算量

保持している正の面積の長方形の数を $N$ とする。計算対象が長方形の場合、その左端を $l$ 、下端を $d$ 、右端を $r$ 、上端を $u$ とし、幅を $W=r-l$ 、高さを $H=u-d$ とする。無引数の `calc_...()` では、追加した長方形全体の外接長方形を計算対象とする。

可変 y 範囲では $W=r-l$ とし、 $f(x)<g(x)$ となる整数 $x$ だけを対象として、y 座標全体の幅を $Y=\max_x g(x)-\min_x f(x)$ とする。`C`, `T2` の演算と $f, g$ の各評価には定数時間がかかるものとする。

- `add_rectangle`: 償却時間 $O(1)$
- 追加した長方形を保持する空間: $O(N)$
- `CompressedRectangleAddMaxGet` の各 `calc_...`: 時間 $O(N\log N)$ 、追加の空間 $O(N)$
- `RectangleAddMaxGet` で計算対象が長方形である各 `calc_...`: 時間 $O(H+W+N\log H)$ 、追加の空間 $O(H+W+N)$
- `RectangleAddMaxGet` で可変 y 範囲を指定する各 `calc_...`: 時間 $O(Y+(W+N)\log Y)$ 、追加の空間 $O(Y+W+N)$
- `RectangleAddMaxGet` では、計算対象が長方形の場合は $W, H$ が `int` の範囲に収まり、可変 y 範囲の場合は $W, Y$ が `int` の範囲に収まる必要がある。
