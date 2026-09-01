---
title: 凸包と直線の共通部分（ $O(\log N)$ ）
documentation_of: geometry/line-convex-polygon-intersection.hpp
---

## 概要

凸包を表す頂点列を `hull` とし、その要素数を $N$ とおく。

- 凸包と直線の共通部分を時間 $O(\log N)$ で求める。
- `hull` が空、1 点、線分、面積が正の狭義凸多角形のどれを表していても扱える。
- 共通部分は空集合、1 点、線分のいずれかである。
- 返り値のサイズは $0, 1, 2$ のいずれかである。
- 共通部分が線分の場合は、その両端点を返す。
- 座標型が整数ならば、内部計算に用いる型を `T` とすると、返り値の各要素は `LineConvexHullIntersectionPoint<T>` になる。
- 標準の 64 bit 以下の符号付き整数型を座標型にすると、既定で `NicheLibrary::Int128` を内部計算に用いる。
- 内部計算の型は明示できる。
- `std::complex` ベースの点型と `.x`, `.y` ベースの点型の両方を想定している。

## 使い方

- `line_convex_hull_intersection<Point, Calc>(hull, line_a, line_b)`
  - 凸包 `hull` と、`line_a` と `line_b` を通る直線の共通部分を返す。
  - `Calc` は内部計算の型であり、省略できる。
  - `Calc` を省略した場合、標準の 64 bit 以下の符号付き整数型を座標型にすると `NicheLibrary::Int128` を用いる。
  - 前提: 以下の幾何条件は、各座標を `Calc` に変換した後の値について満たす。
  - 前提: `line_a` と `line_b` は異なる点である。
  - 前提: `hull` のサイズが 2 ならば 2 点は相異なる。
  - 前提: `hull` のサイズが 3 以上ならば `hull` は反時計回りであり、連続する 3 頂点が一直線に並んでおらず、面積が正の狭義凸多角形である。
  - 前提: 整数座標では、座標型と `Calc` は符号付き整数型であり、すべての中間値を `Calc` で表せる。
  - 備考: `hull` のサイズが 0 ならば空集合として扱う。
  - 備考: 返り値のサイズは $0, 1, 2$ のいずれかである。
  - 備考: 返り値の要素の順序は保証しない。
  - 備考: `Calc` が整数型でない場合、符号を調べる値の絶対値が $10^{-12}$ 以下ならば $0$ とみなす。

- `line_polygon_intersection<Point, Calc>(polygon, line_a, line_b)`
  - `line_convex_hull_intersection<Point, Calc>(polygon, line_a, line_b)` と同じである。
  - 互換性のために残している。

- `LineConvexHullIntersectionValue<Point, Calc>`
  - `line_convex_hull_intersection<Point, Calc>` の要素型である。
  - 備考: 内部計算に用いる型を `T` とする。座標型が整数ならば `LineConvexHullIntersectionPoint<T>` となり、そうでなければ `Point` になる。

- `LineConvexHullIntersectionResult<Point, Calc>`
  - `line_convex_hull_intersection<Point, Calc>` の返り値型である。

- `LinePolygonIntersectionValue<Point, Calc>`
  - `LineConvexHullIntersectionValue<Point, Calc>` と同じである。

- `LinePolygonIntersectionResult<Point, Calc>`
  - `LineConvexHullIntersectionResult<Point, Calc>` と同じである。

- `LineConvexHullIntersectionPoint<T>::x_as<Real>()`
  - $x$ 座標を `Real` に変換して返す。

- `LineConvexHullIntersectionPoint<T>::y_as<Real>()`
  - $y$ 座標を `Real` に変換して返す。

- `LineConvexHullIntersectionPoint<T>::to_point<Point>()`
  - 交点を `Point` に変換して返す。
  - 前提: `denominator` を $q$ として、変換先の座標型が整数ならば $q=1$ である。
  - 備考: 浮動小数点の点型へは `long double` を経由して変換する。

- `LineConvexHullIntersectionPoint<T>::x_numerator`, `y_numerator`, `denominator`
  - `x_numerator`, `y_numerator`, `denominator` をそれぞれ $p_x, p_y, q$ とおく。交点の座標は $(p_x/q, p_y/q)$ である。
  - $q>0$ になるように正規化している。

- `LinePolygonIntersectionPoint<T>`
  - `LineConvexHullIntersectionPoint<T>` と同じである。
  - 互換性のために残している。

## 計算量

`hull` のサイズを $N$ とおく。

- 時間計算量: $O(\log N)$
- 空間計算量: $O(1)$
