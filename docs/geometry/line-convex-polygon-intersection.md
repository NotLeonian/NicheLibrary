---
title: 凸包と直線の交点（$O(\log N)$）
documentation_of: geometry/line-convex-polygon-intersection.hpp
---

## 概要

凸多角形の頂点数を $N$ とおく。

- 凸包と直線の共通部分を時間 $O(\log N)$ で求める。
- 凸包が空、1 点、線分、面積正の狭義凸多角形のどれであっても扱える。
- 共通部分は空集合、1 点、線分のいずれかである。
- 返り値のサイズは $0, 1, 2$ のいずれかである。
- 共通部分が線分の場合は、その両端点を返す。
- 座標型が整数ならば `LineConvexHullIntersectionPoint` を返す。
- 標準の 64 bit 以下の整数座標では、既定で `NicheLibrary::Int128` を内部計算に用いる。
- 内部計算の型は明示できる。
- `std::complex` ベースの点型と `.x`, `.y` ベースの点型の両方を想定している。

## 使い方

- `line_convex_hull_intersection<Point, Calc>(hull, line_a, line_b)`
  - 凸包 `hull` と、`line_a` と `line_b` を通る直線の共通部分を返す。
  - `Calc` は内部計算の型であり、省略できる。
  - `Calc` を省略した場合、標準の 64 bit 以下の整数座標では `NicheLibrary::Int128` を用いる。
  - 前提: `line_a` と `line_b` は異なる直線である。
  - 前提: `hull` のサイズが 2 ならば 2 点は相異なる。
  - 前提: `hull` のサイズが 3 以上ならば `hull` は反時計回りであり、連続する 3 頂点が一直線に並んでおらず、面積が正の狭義凸多角形である。
  - 前提: 整数座標では、外積、交点の有理表現の分子、分母が内部計算の型で表せる。
  - 備考: `hull` のサイズが 0 ならば空集合として扱う。
  - 備考: 返り値のサイズは $0, 1, 2$ のいずれかである。
  - 備考: 返る順序は保証しない。

- `line_polygon_intersection<Point, Calc>(polygon, line_a, line_b)`
  - `line_convex_hull_intersection<Point, Calc>(polygon, line_a, line_b)` と同じである。
  - 互換性のために残している。

- `LineConvexHullIntersectionValue<Point, Calc>`
  - `line_convex_hull_intersection<Point, Calc>` の要素型である。
  - 備考: 座標型が整数ならば `LinePolygonIntersectionPoint<Calc>` となり、そうでなければ `Point` になる。

- `LineConvexHullIntersectionResult<Point, Calc>`
  - `line_convex_hull_intersection<Point, Calc>` の返り値型である。

- `LinePolygonIntersectionValue<Point, Calc>`
  - `LineConvexHullIntersectionValue<Point, Calc>` と同じである。

- `LinePolygonIntersectionResult<Point, Calc>`
  - `LineConvexHullIntersectionResult<Point, Calc>` と同じである。

- `LinePolygonIntersectionPoint<T>::x_as<Real>()`
  - $x$ 座標を `Real` に変換して返す。

- `LinePolygonIntersectionPoint<T>::y_as<Real>()`
  - $y$ 座標を `Real` に変換して返す。

- `LinePolygonIntersectionPoint<T>::to_point<Point>()`
  - 交点を `Point` に変換して返す。
  - 前提: `denominator` を $q$ として、変換先の座標型が整数ならば $q=1$ である。
  - 備考: 浮動小数点の点型へは `long double` を経由して変換する。

- `LinePolygonIntersectionPoint<T>::x_numerator`, `y_numerator`, `denominator`
  - 交点の有理表現である。
  - `denominator` を $q$ として、$q>0$ になるように正規化している。
  - 前提: `x_numerator`, `y_numerator`, `denominator` が `T` で表せる。

## 計算量

`hull` のサイズ を $N$ とおく。

- 時間: $O(\log N)$
- 空間: $O(1)$
