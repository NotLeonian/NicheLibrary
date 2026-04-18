---
title: 凸多角形と直線の交点（$O(\log N)$）
documentation_of: geometry/line-convex-polygon-intersection.hpp
---

## 概要

- 反時計回りの狭義凸多角形と直線の交点を $O(\log N)$ で求める。
- 交点数は $0, 1, 2$ のいずれかである。
- 座標型が整数なら厳密な有理点を返す。
- `std::complex` ベースの点型と `.x`, `.y` ベースの点型の両方を想定している。

## 使い方

- `line_polygon_intersection(polygon, line_a, line_b)`
  - 凸多角形 `polygon` と直線 `line_a` - `line_b` の交点を返す。
  - 返り値の型は `LinePolygonIntersectionResult<Point>` である。
  - 前提: `polygon.size() >= 3`, `line_a != line_b`。
  - 前提: `polygon` は反時計回りで, 3 頂点連続で一直線にならない。
  - 備考: 返り値のサイズは `0`, `1`, `2` のいずれかである。
  - 備考: 返る順序は保証しない。

- `LinePolygonIntersectionValue<Point>`
  - `line_polygon_intersection` の要素型である。
  - 備考: 座標型が整数なら `LinePolygonIntersectionPoint<T>`, そうでなければ `Point` になる。

- `LinePolygonIntersectionResult<Point>`
  - `line_polygon_intersection` の返り値型である。

- `LinePolygonIntersectionPoint<T>::x_as<Real>()`
  - $x$ 座標を `Real` に変換して返す。

- `LinePolygonIntersectionPoint<T>::y_as<Real>()`
  - $y$ 座標を `Real` に変換して返す。

- `LinePolygonIntersectionPoint<T>::to_point<Point>()`
  - 交点を `Point` に変換して返す。
  - 前提: 変換先の座標型が整数なら `denominator == 1`。
  - 備考: 浮動小数点の点型へは `long double` を経由して変換する。

- `LinePolygonIntersectionPoint<T>::x_numerator`, `y_numerator`, `denominator`
  - 交点の有理表現である。
  - 備考: `denominator > 0` になるように正規化している。

## 計算量

- 時間: $O(\log N)$
- 空間: $O(1)$
