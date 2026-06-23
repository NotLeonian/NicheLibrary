---
title: $A_i x\equiv B_i\pmod{M_i}$ の形の連立合同方程式
documentation_of: math/number-theory/generalized-garner.hpp
---

## 概要

未知数を $x$、各式の係数を $A_i$、右辺を $B_i$、法を $M_i$ とおく。

- $A_i x\equiv B_i\pmod{M_i}$ の形の連立合同方程式を解く。
- 各式は $A_i$ および $B_i$ を $M_i$ で割った余りとして扱う。
- $A_i$ と $M_i$ は互いに素でなくてよい。
- $A_i x\equiv B_i\pmod{M_i}$ の解を $x\equiv r\pmod m$ としたときの $(r,m)$ を返す（より厳密な要件は後述）。

## 使い方

- `generalized_garner<R1, R2>(a, b, m)`
  - `std::pair<R1, R2>` を返す。
  - `R2` を省略した場合は `R1` と同じ型になる。
  - `a[i]`, `b[i]`, `m[i]` は $A_i,\,B_i,\,M_i$ を表す。
  - 返り値の各成分を $r,m$ とおく。解が存在するなら、連立合同方程式の全ての解を表す $x\equiv r\pmod m$ の $(r,m)$ を返す。
  - 解が存在しないなら $(0,0)$ を返す。
  - 前提: $\lvert a\rvert=\lvert b\rvert=\lvert m\rvert,\;M_i>0$。
  - 前提: $A_i,\,B_i,\,M_i$ は内部の共通型へ変換できる。
  - 前提: 返り値は `R1`, `R2` に収まる。
  - 備考: 空の列、または制約を与えない式だけなら $(0,1)$ を返す。

## 計算量

- $N=\lvert a\rvert,\;V=\max_i M_i$ とする。
- 標準 64 ビット整数型では時間 $O(N\log V)$。
- `__int128` では時間 $O(N\log^2 V)$。
- 空間 $O(1)$。
