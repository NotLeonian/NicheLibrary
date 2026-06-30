# NicheLibrary
[![Actions Status](https://github.com/NotLeonian/NicheLibrary/actions/workflows/verify.yml/badge.svg?branch=main)](https://github.com/NotLeonian/NicheLibrary/actions/workflows/verify.yml?query=branch%3Amain)
[![GitHub Pages](https://img.shields.io/static/v1?label=GitHub+Pages&message=NicheLibrary+&color=brightgreen&logo=github)](https://notleonian.github.io/NicheLibrary/)

C++ library for competitive programming

競技プログラミング用の C++ ライブラリです。

## 方針
このライブラリは、名前通りニッチなもののみを実装しています。

- [Luzhiled's Library](https://ei1333.github.io/library/), [Nyaan's Library](https://nyaannyaan.github.io/library/), [cp-library-cpp](https://suisen-cp.github.io/cp-library-cpp/) のどれにも自分の求める要件を満たす実装が存在しないデータ構造、アルゴリズムのみを実装します。
- 原則として、実装するのは competitive-verifier で verify できるデータ構造、アルゴリズムのみとします。
  - 現在、competitive-verifier での verify に使用できるオンラインジャッジは [yukicoder](https://yukicoder.me/), [Library Checker (Yosupo Judge)](https://judge.yosupo.jp/), [Aizu Online Judge (AOJ)](https://judge.u-aizu.ac.jp/onlinejudge/) です。
  - また、オンラインジャッジ上の既存の問題を使用しない verify も適宜実装しています。

## ドキュメントの言語

- ドキュメントおよびソースコード中のコメントには原則日本語を使用します。
- Issue および Pull Request の description は日本語以外でも構いません。

## ローカルでの verify

ローカルでの verify は、以下のコマンドで行えます。

```sh
uv sync --group dev
bash .github/scripts/local_verify.sh
```

ローカル verify では、過去のローカル verify の結果を使用できます。  
キャッシュは `${XDG_CACHE_HOME:-$HOME/.cache}/niche-library/local-verify/` 配下に保存され、リポジトリのパスと competitive-verifier の設定、使用するスクリプトおよびコンパイラの fingerprint ごとに分離されます。  
[.competitive-verifier/config.toml](.competitive-verifier/config.toml)、[pyproject.toml](pyproject.toml)、[uv.lock](uv.lock)、[.competitive-verifier/config.toml](.competitive-verifier/config.toml) 上の `CXXFLAGS` などが変わった場合は、古い verify 結果は `prev-result` として使用されません。

GitHub Actions の verify は、GitHub Actions 専用のキャッシュに保存された結果を使用します。
ローカル verify のキャッシュは GitHub Actions では参照しません。

過去のローカル verify の結果を使わずにローカル verify を実行する場合は、`--no-prev-result` を指定します。
この場合も、今回の verify 結果は次回以降のローカル verify 用に保存されます。

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

yukicoder の問題を verify する場合は、`YUKICODER_TOKEN` を環境変数に設定する必要があります。  
以下のコマンドであれば、環境変数として `YUKICODER_TOKEN` をシェルに残さずに実行することができます。

```sh
bash -c '
set -euo pipefail

IFS= read -r -e -s -p "YUKICODER_TOKEN: " YUKICODER_TOKEN
printf "\n"

export YUKICODER_TOKEN
bash .github/scripts/local_verify.sh "$@"
' _
```

オプションを渡す場合は、末尾の `_` の後ろに追加します。  
以下は、オプションに `--no-prev-result` を指定して実行する例です。

```sh
bash -c '
set -euo pipefail

IFS= read -r -e -s -p "YUKICODER_TOKEN: " YUKICODER_TOKEN
printf "\n"

export YUKICODER_TOKEN
bash .github/scripts/local_verify.sh "$@"
' _ --no-prev-result
```

## Contributing

不具合の指摘または修正の Issue, Pull Request は歓迎します。  
新規のデータ構造、アルゴリズムの追加についても条件を満たす場合は受け入れる可能性がありますが、追加の要望の Issue への対応は基本的に行いません。

詳しくは [CONTRIBUTING.md](CONTRIBUTING.md) を参照してください。

## ライセンス

[CC0-1.0 license](LICENSE)
