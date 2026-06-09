# NicheLibrary
[![Actions Status](https://github.com/NotLeonian/NicheLibrary/actions/workflows/verify.yml/badge.svg?branch=main)](https://github.com/NotLeonian/NicheLibrary/actions/workflows/verify.yml?query=branch%3Amain)
[![GitHub Pages](https://img.shields.io/static/v1?label=GitHub+Pages&message=NicheLibrary+&color=brightgreen&logo=github)](https://notleonian.github.io/NicheLibrary/)

C++ library for competitive programming

## 方針
このライブラリは、名前通りニッチなもののみを実装しています。

- [Luzhiled's Library](https://ei1333.github.io/library/) / [Nyaan's Library](https://nyaannyaan.github.io/library/) / [cp-library-cpp](https://suisen-cp.github.io/cp-library-cpp/) のどれにも自分の求める要件を満たす実装が存在しないデータ構造・アルゴリズムのみを実装します。
- 原則として、実装するのは competitive-verifier で verify できる既存の問題があるデータ構造・アルゴリズムのみとします。
  - 現在、competitive-verifier で verify できるオンラインジャッジは yukicoder / Library Checker (Yosupo Judge) / Aizu Online Judge (AOJ) です。
  - AOJ はテストケースが弱いと聞くので、なるべく yukicoder か Library Checker から選んでいます。

## ドキュメントの言語

- ドキュメントおよびソースコード中のコメントには原則日本語を使用します。
- Issue および Pull Request の description は日本語以外でも構いません。

## Contributing

不具合の指摘または修正の Issue / Pull Request は歓迎します。  
新規のデータ構造・アルゴリズムの追加についても条件を満たす場合は受け入れる可能性がありますが、方針上、追加の要望の Issue への対応は基本的に行いません。

詳しくは CONTRIBUTING.md を参照してください。

## ライセンス

[CC0-1.0 license](LICENSE)
