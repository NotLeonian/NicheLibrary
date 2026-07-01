---
title: 列の隣り合わない $k$ 個の要素を選んだ総和の最大値または最小値の列挙
documentation_of: other/enumerate-maximum-independent-set-path-sums.hpp
---

## 概要

長さ $n$ の列 $a$ について、隣り合わない $k$ 個の添字を選んだときの $a_i$ の総和の最大値または最小値を、全ての $k$ に対して列挙する。

- パス上の重み付き独立集合について、独立集合の大きさごとの最大重みまたは最小重みを求めるものとみなせる。
- 返り値の長さは $\lceil n / 2 \rceil + 1$ である。
- 返り値の $k$ 番目 (0-based indexing) の要素は、`maximum` が `true` であれば $k$ 個選んだときの総和の最大値、`false` であれば $k$ 個選んだときの総和の最小値である。

## 使い方

`enumerate_maximum_independent_set_path_sums<T, bool maximum = true>(a)`, `enumerate_maximum_independent_set_path_sums<bool maximum, T>(a)`

- 比較ソートを用いる。
- 引数 $a$ は重みの列である。
- `maximum` は既定で `true` であり、`true` であれば最大値、`false` であれば最小値を返す。
- 返り値は、$k$ 個選ぶときの最大値または最小値を $k$ 番目 (0-based indexing) の要素に持つ列である。
- 前提: `T` は加算、減算、比較が正しく定義されている。
- 前提: アルゴリズム中に現れる加減算結果、および返り値の各要素が `T` の範囲に収まる。
- 前提: `std::is_unsigned_v<T>` が `false` である。

`enumerate_maximum_independent_set_path_sums_bucket_sort<T, bool maximum = true>(a)`, `enumerate_maximum_independent_set_path_sums_bucket_sort<bool maximum, T>(a)`

- バケットソートを用いる。
- 引数 $a$ は重みの列である。
- `maximum` は既定で `true` であり、`true` であれば最大値、`false` であれば最小値を返す。
- 返り値は、$k$ 個選ぶときの最大値または最小値を $k$ 番目 (0-based indexing) の要素に持つ列である。
- 前提: `T` は `bool` でない 32 bit 以下のプリミティブな符号付き整数型である。
- 前提: `std::is_unsigned_v<T>` が `false` である。
- 前提: 引数として与えられる列は非負整数列である。
- 前提: 引数として与えられる列の総和は `T` の範囲に収まる。

## 計算量

- `enumerate_maximum_independent_set_path_sums(a)`: 時間 $O(n \log n)$、空間 $O(n)$。
- `enumerate_maximum_independent_set_path_sums_bucket_sort(a)`: 列 $a$ の総和を $S$ として時間 $O(n + S)$、空間 $O(n + S)$。
