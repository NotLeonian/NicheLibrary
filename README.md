# NicheLibrary
C++ library for competitive programming

競技プログラミング用の C++ ライブラリです。  
ドキュメント（自動生成）: https://notleonian.github.io/NicheLibrary/

## 方針

このライブラリでは、名前通り「ニッチ」なもののみを実装する方針をとっています。

- [Luzhiled's Library](https://ei1333.github.io/library/) / [Nyaan's Library](https://nyaannyaan.github.io/library/) / [cp-library-cpp](https://suisen-cp.github.io/cp-library-cpp/) のどれにも**自分の求める要件を満たす実装が存在しない**データ構造・アルゴリズムのみを実装します。
- 実装するのは competitive-verifier で **verify できる既存の問題がある**データ構造・アルゴリズムのみとします。
  - つまり、yukicoder / Library Checker / Aizu Online Judge のどれかに既存の問題があるものです。AOJ はテストケースが弱いと聞くので、なるべく yukicoder か Library Checker から選んでいます。

## ドキュメントの言語

- ドキュメントおよびソースコード中のコメントには原則**日本語**を使用します。
- Issue および Pull Request の description は日本語以外でも構いません。

## Contributing

不具合の指摘または修正の Issue / Pull Request は歓迎します。  
新規のデータ構造・アルゴリズムの追加についても条件を満たす場合は受け入れる可能性がありますが、方針上、追加の要望の Issue への対応は基本的に行いません。

詳しくは CONTRIBUTING.md を参照してください。

## ライセンス

CC0-1.0 license

詳しくは LICENSE を参照してください。
