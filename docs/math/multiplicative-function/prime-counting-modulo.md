---
title: $m$ で割った余りごとの $N$ 以下の素数の個数の列挙
documentation_of: math/multiplicative-function/prime-counting-modulo.hpp
---

## 概要

- $N$ 以下の素数を、$m$ で割った余りごとに数える。
- Lucy DP のテーブルを余りごとに持つ。
- `prime_counting_modulo_mf_prefix_sum_table<T>` は、各余りについて Black Algorithm 用の `Fprime` を返す。
- $m$ が合成数でも特別扱いしない。

## 使い方

- `prime_counting_modulo_table(N, m)`
  - `pair<vector<long long>, vector<vector<long long>>>` を返す。
  - `first[i]` はテーブルの値 $x$ である。
  - `second[k][i]` は、$x$ 以下の素数で $m$ で割った余りが $k$ であるものの個数を返す。
  - 前提: $N\ge 0$, $m>0$。
  - 備考: `second` の 1 つ目の添字は余りである。

- `prime_counting_modulo(N, m)`
  - 長さ $m$ の `vector<long long>` を返す。
  - 返り値の $k$ 番目は、$N$ 以下の素数で $m$ で割った余りが $k$ であるものの個数である。
  - 前提: $N\ge 0$, $m>0$。

- `prime_counting_modulo_mf_prefix_sum_table<T>(N, m)`
  - 長さ $m$ の `vector<vector<T>>` を返す。
  - 返り値の $k$ 番目は、$m$ で割った余りが $k$ である素数だけを対象にする `Fprime` である。
  - 各要素は `long long` から `T` に変換される。
  - 前提: $N\ge 0$, $m>0$。
  - 備考: 複数の余りをまとめる場合は、返り値をユーザー側で足す。
  - 備考: $N=0$ では各行は空である。

## 計算量

- 時間: $O(m N^{3/4}/\log N)$
- 空間: $O(m\sqrt{N})$
