---
title: $m$ で割った余りごとの $N$ 以下の素数の個数の列挙
documentation_of: math/multiplicative-function/prime-counting-modulo.hpp
---

## 概要

上限を $N$ 、法を $m$ とおく。

- $N$ 以下の素数を、 $m$ で割った余りごとに数える。
- Lucy DP のテーブルを余りごとに持つ。
- `prime_counting_modulo_mf_prefix_sum_table<T>` は、各剰余類に対応する Black Algorithm 用の `Fprime` を返す。
- $m$ が合成数の場合も同じ処理を行う。

## 使い方

添字 $k$ は余りを表すとする。

- `prime_counting_modulo_table(long long N, long long m)`
  - `std::pair<std::vector<long long>, std::vector<std::vector<long long>>>` を返す。
  - `first[i]` を $x$ とおく。`second[k][i]` は、 $x$ 以下の素数で $m$ で割った余りが $k$ であるものの個数である。
  - 前提: $N\ge 0,\;m>0$ 。
  - 備考: `second` の第 1 添字は余りである。

- `prime_counting_modulo(long long N, long long m)`
  - 長さ $m$ の `std::vector<long long>` を返す。
  - 返り値の $k$ 番目は、 $N$ 以下の素数で $m$ で割った余りが $k$ であるものの個数である。
  - 前提: $N\ge 0,\;m>0$ 。

- `prime_counting_modulo_mf_prefix_sum_table<T>(long long N, long long m)`
  - 長さ $m$ の `std::vector<std::vector<T>>` を返す。
  - 返り値の $k$ 番目は、 $m$ で割った余りが $k$ である素数だけを対象にする `Fprime` である。
  - 各要素は `long long` から `T` に変換される。
  - 前提: $N\ge 0,\;m>0$ 。
  - 備考: 複数の剰余類を同時に対象とする場合は、対応する `Fprime` をユーザー側で要素ごとに足す。
  - 備考: $N=0$ では各行は空である。

## 計算量

- 時間計算量: $O(m N^{3/4}/\log N)$
- 空間計算量: $O(m\sqrt{N})$
