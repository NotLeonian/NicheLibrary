# NicheLibrary

[![Actions Status](https://github.com/NotLeonian/NicheLibrary/actions/workflows/verify.yml/badge.svg?branch=main)](https://github.com/NotLeonian/NicheLibrary/actions/workflows/verify.yml?query=branch%3Amain)
[![GitHub Pages](https://img.shields.io/static/v1?label=GitHub+Pages&message=NicheLibrary+&color=brightgreen&logo=github)](https://notleonian.github.io/NicheLibrary/)

C++ library for competitive programming

競技プログラミング用の C++ ライブラリです。

## 方針

このライブラリは、その名のとおりニッチなデータ構造やアルゴリズムだけを実装しています。

- [Luzhiled's Library](https://ei1333.github.io/library/), [Nyaan's Library](https://nyaannyaan.github.io/library/), [cp-library-cpp](https://suisen-cp.github.io/cp-library-cpp/) のいずれにも、求める要件を満たす実装がないデータ構造やアルゴリズムだけを実装します。
- 原則として、competitive-verifier で verify できるデータ構造やアルゴリズムだけを実装します。
  - 現在、competitive-verifier での verify に使用できるオンラインジャッジは [yukicoder](https://yukicoder.me/), [Library Checker (Yosupo Judge)](https://judge.yosupo.jp/), [Aizu Online Judge (AOJ)](https://judge.u-aizu.ac.jp/onlinejudge/) です。
  - オンラインジャッジ上の既存の問題を使わない自己検証も、必要に応じて実装しています。

## ドキュメントの言語

- ドキュメントおよびソースコード中のコメントには原則日本語を使用します。
- Issue および Pull Request の description は日本語以外でも構いません。

## ローカルでの verify

ローカルでの verify は、以下のコマンドで行えます。

```sh
uv sync --group dev
bash .github/scripts/local_verify.sh
```

ローカルで verify するときは、前回までの結果を再利用できます。  
キャッシュは `${XDG_CACHE_HOME:-$HOME/.cache}/niche-library/local-verify/` 配下に保存され、リポジトリのパス、competitive-verifier の設定、使用するスクリプト、環境変数、コンパイラの情報から計算したフィンガープリントごとに分けられます。  
[.competitive-verifier/config.toml](.competitive-verifier/config.toml)、[pyproject.toml](pyproject.toml)、[uv.lock](uv.lock)、使用するスクリプト、`CXXFLAGS` などの環境変数、コンパイラのいずれかが変わった場合、以前の verify 結果は `prev-result` として再利用されません。

GitHub Actions での verify には専用のキャッシュを使用し、ローカル verify のキャッシュは参照しません。

過去のローカル verify の結果を使わずにローカル verify を実行する場合は、`--no-prev-result` を指定します。
この場合も、今回の verify 結果は次回以降に再利用できるよう保存されます。

```sh
bash .github/scripts/local_verify.sh --no-prev-result
```

キャッシュを削除して全ての verify を再実行する場合は、`--full` を指定します。

```sh
bash .github/scripts/local_verify.sh --full
```

clang-format の確認も同時に行う場合は、`--format` を指定します。

```sh
bash .github/scripts/local_verify.sh --format
```

yukicoder の問題で verify する場合は、`YUKICODER_TOKEN` を環境変数に設定する必要があります。  
次のように実行すると、`YUKICODER_TOKEN` を現在のシェルの環境変数に残さずに済みます。

```sh
bash -c '
set -euo pipefail

IFS= read -r -e -s -p "YUKICODER_TOKEN: " YUKICODER_TOKEN
printf "\n"

export YUKICODER_TOKEN
bash .github/scripts/local_verify.sh "$@"
' _
```

オプションを渡す場合は、末尾の `_` に続けて指定します。  
次に、`--no-prev-result` を指定する例を示します。

```sh
bash -c '
set -euo pipefail

IFS= read -r -e -s -p "YUKICODER_TOKEN: " YUKICODER_TOKEN
printf "\n"

export YUKICODER_TOKEN
bash .github/scripts/local_verify.sh "$@"
' _ --no-prev-result
```

## Python の検査

Python ソースコードのフォーマットと静的検査は、次のコマンドで実行できます。

```sh
uv run ruff format .
uv run ruff check .
git ls-files -z --cached --others --exclude-standard -- '*.py' |
  xargs -0 -r uv run pyright --project pyright-checks.json
git ls-files -z --cached --others --exclude-standard -- '*.py' |
  xargs -0 -r uv run mypy
```

## Contributing

不具合を報告または修正する Issue や Pull Request を歓迎します。  
新しいデータ構造やアルゴリズムを追加する Pull Request も、条件を満たす場合は受け入れることがあります。ただし、追加を求める Issue には基本的に対応しません。

詳しくは [CONTRIBUTING.md](CONTRIBUTING.md) を参照してください。

## ライセンス

[CC0-1.0 license](LICENSE)
