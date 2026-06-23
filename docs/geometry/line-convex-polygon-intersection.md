---
title: 凸多角形と直線の交点（$O(\log N)$）
documentation_of: geometry/line-convex-polygon-intersection.hpp
---

## 概要

凸多角形の頂点数を $N$ とおく。

- 反時計回りの面積正の狭義凸多角形と直線の共通部分を $O(\log N)$ で求める。
- 共通部分は空集合、1 点、線分のいずれかである。
- 返り値のサイズは $0, 1, 2$ のいずれかである。
- 共通部分が線分の場合は、その両端点を返す。
- 凸包が 1 点または 2 点に退化する場合は前提外であり、呼び出し側で処理する。
- 座標型が整数なら `LinePolygonIntersectionPoint<T>` を返す。
- 整数座標では内部に `__int128` を用いる。主な対象は $10^9$ 級制約であり、非常に大きい整数座標、特に $10^{18}$ 級では返り値生成時にオーバーフローする可能性がある。
- `std::complex` ベースの点型と `.x`, `.y` ベースの点型の両方を想定している。

## 使い方

- `line_polygon_intersection(polygon, line_a, line_b)`
  - 凸多角形 `polygon` と、`line_a` と `line_b` を通る直線の共通部分を返す。
  - 返り値の型は `LinePolygonIntersectionResult<Point>` である。
  - 前提: `polygon` を $P$、`line_a` を $a$、`line_b` を $b$ として、$\lvert P\rvert \ge 3,\;a\ne b$。
  - 前提: `polygon` は面積正の狭義凸多角形で、反時計回りで、3 頂点連続で一直線にならない。
    - 注意: `convex_hull` の結果を渡す場合、凸包が 1 点または 2 点になる退化ケースは本関数の前提外である。
    - 注意: 退化凸包は、呼び出し側で点または線分として処理する。
  - 備考: 返り値のサイズは $0, 1, 2$ のいずれかである。
  - 備考: 返り値のサイズが $2$ のとき、共通部分は 2 点または線分である。線分の場合はその両端点を返す。
  - 備考: 返る順序は保証しない。

- `LinePolygonIntersectionValue<Point>`
  - `line_polygon_intersection` の要素型である。
  - 備考: 座標型が整数なら `LinePolygonIntersectionPoint<T>`、そうでなければ `Point` になる。
  - 備考: 整数座標で 64bit 全域の厳密性は保証しない。非常に大きい整数座標、特に $10^{18}$ 級ではオーバーフローする可能性がある。

- `LinePolygonIntersectionResult<Point>`
  - `line_polygon_intersection` の返り値型である。

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
  - 備考: 非常に大きい整数座標では `x_numerator`, `y_numerator` の生成時にオーバーフローする可能性がある。

## 計算量

- 時間: $O(\log N)$
- 空間: $O(1)$
