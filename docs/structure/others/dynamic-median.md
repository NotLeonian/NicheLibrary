---
title: 値の追加・削除と中央値取得
documentation_of: structure/others/dynamic-median.hpp
---

## 概要

- 値を追加・削除しながら、中央値を求める。
- 下側中央値、上側中央値、両側の算術平均を選べる。
- `median()` は下側中央値を返す。
- `lower_values` と `upper_values` の $2$ つの多重集合で値を分けて持つ。

## 使い方

- `DynamicMedian<T>()`
  - 空で構築する。
  - 前提：`T` はコピー可能で、`std::multiset<T>` で扱える比較を持つ。
- `DynamicMedianMode`
  - `Lower` は昇順で $0$ 始まりの $(N - 1) / 2$ 番目の値を指定する。
  - `Upper` は昇順で $0$ 始まりの $N / 2$ 番目の値を指定する。
  - `Average` は `Lower` と `Upper` の算術平均を指定する。
  - 備考：要素数が奇数の場合、$3$ 種類は同じ値になる。
- `void add(T x)`
  - 値 $x$ を $1$ 個追加する。
- `bool erase(T x)`
  - 値 $x$ を $1$ 個削除する。
  - 返り値：削除できたなら `true`、存在しなければ `false`。
  - 備考：同じ値が複数ある場合はいずれか $1$ 個だけを削除する。
- `template <class Result = T> Result median(DynamicMedianMode mode = DynamicMedianMode::Lower) const`
  - `mode` で指定した中央値を返す。
  - 前提：要素数が $1$ 以上である。
  - 前提：`Average` を使う場合、`Result` へ変換した値の加算と $2$ での除算ができる。
  - 備考：`Result` が整数型の場合は、中央 $2$ 値の和を $2$ で割る整数除算の値を、和のオーバーフローを避けて返す。
  - 備考：平均を小数で返したい場合は `median<long double>(DynamicMedianMode::Average)` のように指定する。
- `std::multiset<T> lower_values`
  - 小さい側の値を持つ。
  - 備考：空でなければ最大値が下側中央値である。
- `std::multiset<T> upper_values`
  - 大きい側の値を持つ。
  - 備考：`lower_values` の各値は `upper_values` の各値以下である。

## 計算量

- 要素数を $N$ とする。
- `add`, `erase`: $O(\log N)$
- `median`: $O(1)$
- 空間：$O(N)$
