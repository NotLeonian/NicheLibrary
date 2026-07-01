# CONTRIBUTING.md
NicheLibrary への貢献をご検討していただきありがとうございます。

このライブラリは、既存の有名な競技プログラミング用ライブラリで要件を満たせないものを補うためのライブラリです。  
このライブラリだけで汎用的な問題を解けるようなライブラリにすることは想定していません。

個人で管理しているため、方針と合わない変更、保守コストに見合わない変更、検証が難しい変更は採用しないことがあります。

このドキュメントは貢献者向けの概要です。  
コーディング規約は [STYLE_GUIDE.md](STYLE_GUIDE.md) に記載しています。

## 言語と文体
- ソースコード中のコメント、[docs/](docs/) および [.competitive-verifier/docs/](.competitive-verifier/docs/) 配下のドキュメントでは、原則として日本語の常体を使用してください。
- [README.md](README.md), [CONTRIBUTING.md](CONTRIBUTING.md), GitHub Pages 上のトップページなどのリポジトリについてのドキュメントでは、原則として日本語の敬体を使用してください。
- Issue および Pull Request の description は、日本語以外でも構いません。
- Markdown の記法、句読点、ファイルパスの書き方などの細かな規約は、[STYLE_GUIDE.md](STYLE_GUIDE.md) に従ってください。

## 受け付ける Issue, Pull Request
### 不具合の報告、修正
既存の実装、verify、ドキュメント、スクリプト、設定ファイルなどの不具合に関する Issue や Pull Request は歓迎します。

不具合を報告する場合は、可能な範囲で次の情報を記述してください。

- 再現条件
- 期待される挙動
- 実際の挙動
- 不具合が発生する入力、ソースコード、コマンドなどの例

不具合を修正する Pull Request では、必要に応じて verify の追加や更新も行ってください。

### 新規のデータ構造、アルゴリズムの追加の Pull Request
新規のデータ構造、アルゴリズムを追加する Pull Request は、次の条件を満たす場合に受け入れる可能性があります。

- [Luzhiled's Library](https://ei1333.github.io/library/), [Nyaan's Library](https://nyaannyaan.github.io/library/), [cp-library-cpp](https://suisen-cp.github.io/cp-library-cpp/) のいずれにも実装が存在しない。もしくは、それらによる実装がこのライブラリで求める要件を満たしていない。
- competitive-verifier で verify できる。
  - [Library Checker (Yosupo Judge)](https://judge.yosupo.jp/), [yukicoder](https://yukicoder.me/), [Aizu Online Judge (AOJ)](https://judge.u-aizu.ac.jp/onlinejudge/) のいずれかの問題を使用した verify を推奨します。
  - それらのオンラインジャッジに適する問題がない場合は、`// competitive-verifier: STANDALONE` による自己検証を追加してください。
  - AtCoder の問題は verify には使用しないでください。
- ヘッダファイル、verify ソースコード、ドキュメントなどが、このリポジトリの既存の構成と記法に沿っている。
- 今後の保守対象として、このリポジトリに含める必要性がある。

条件を満たしていても、管理方針、保守コスト、既存の実装との重複、レビュー可能性などを理由に採用しないことがあります。  
また、変更がこのリポジトリの既存の構成と記法に沿うように、メンテナーが変更を加える可能性があります。

## 基本的に受け付けない Issue, Pull Request
### 追加要望の Issue
追加要望の Issue には基本的に対応しません。

ただし、上記の既存ライブラリに要件を満たす実装がなく、かつ管理者がこのリポジトリで扱う価値があると判断したものは、将来的な候補として残すことがあります。
対応時期は未定です。

## 実装に関する注意
C++ の実装では、少なくとも次の点を満たしてください。

- g++ および clang++ の C++20 でコンパイルできる。
- [.competitive-verifier/config.toml](.competitive-verifier/config.toml) の `CXXFLAGS` で警告が出ない。
- `#include <bits/stdc++.h>` を使用しない。
- `using namespace std;` を使用しない。
- 使用する標準ライブラリのヘッダファイルのみを include し、推移的な include に頼った実装も避ける。
- GCC 拡張やコンパイラに依存した実装にしない。
- ヘッダファイルでは include guard を使用し、`#pragma once` を使用しない。
- 公開 API の名前、前提条件、計算量、例外的な挙動が既存の実装と同程度に分かるようにする。

より細かなコーディング規約は [STYLE_GUIDE.md](STYLE_GUIDE.md) を確認してください。

## verify とドキュメント
新規の実装を追加する場合は、対応する verify ソースコードを [verify/](verify/) に追加してください。
既存の実装を修正する場合も、変更内容に応じて verify を追加または更新してください。

ライブラリとして公開するヘッダファイルには、原則として対応するドキュメントを [docs/](docs/) に追加または更新してください。
ドキュメントは、対応するヘッダファイルのルート直下から見た階層と同じ階層に置いてください。

より細かなコーディング規約は [STYLE_GUIDE.md](STYLE_GUIDE.md) を確認してください。

## フォーマットとローカルでの確認
C++ のソースコードは、[.clang-format](.clang-format) に従い clang-format 20 でフォーマットしてください。
フォーマットされていない場合、CI の formatting check が通りません。

ローカルで verify とフォーマットの確認を行う場合は、次のコマンドを使用できます。

```sh
uv sync --group dev
bash .github/scripts/local_verify.sh --format
```

yukicoder の問題を verify する場合は、`YUKICODER_TOKEN` を環境変数に設定してください。
ローカル verify の詳細は [README.md](README.md) を参照してください。

## リポジトリの運用に関わるファイルの変更
[AGENTS.md](AGENTS.md), [STYLE_GUIDE.md](STYLE_GUIDE.md), [README.md](README.md), [CONTRIBUTING.md](CONTRIBUTING.md), [.github/workflows/verify.yml](.github/workflows/verify.yml), [.github/scripts/](.github/scripts/), [.competitive-verifier/config.toml](.competitive-verifier/config.toml), [pyproject.toml](pyproject.toml), [uv.lock](uv.lock), [.clang-format](.clang-format) などは、レビュー、CI、verify、ドキュメント生成、依存関係の前提に影響します。

これらを変更する Pull Request では、変更理由、影響範囲、ローカルで確認した内容を description に書いてください。
運用方針そのものを変える変更は、通常のヘッダ追加や不具合修正よりも慎重に扱います。

AI 支援ツールや自動修正ツールを使用する場合も、出力をそのまま提出せず、[STYLE_GUIDE.md](STYLE_GUIDE.md) のコーディング規約に照らして内容を確認してください。

## Pull Request 作成前の確認

- [ ] このリポジトリの方針に合う変更である。
- [ ] 必要な verify を追加または更新した。
- [ ] 必要なドキュメントを日本語で追加または更新した。
- [ ] clang-format 20 によるフォーマットを行った。
- [ ] g++ および clang++ の C++20 でコンパイルでき、警告が出ない。
- [ ] `bash .github/scripts/local_verify.sh --format` など、実行した確認内容を Pull Request の description に書いた。
- [ ] リポジトリの運用に関わるファイルを変更した場合は、変更理由と影響範囲を書いた。
