#!/usr/bin/env bash
set -euo pipefail

ROOT="$(git rev-parse --show-toplevel)"
cd "$ROOT"

CF_BIN="${CLANG_FORMAT_BIN:-}"
if [[ -z "$CF_BIN" ]]; then
  if command -v clang-format-20 >/dev/null 2>&1; then
    CF_BIN="clang-format-20"
  elif command -v clang-format >/dev/null 2>&1; then
    CF_BIN="clang-format"
  else
    echo "Either clang-format-20 or clang-format is required." >&2
    exit 1
  fi
fi

"$CF_BIN" --version

STYLE="file"
if [[ ! -f .clang-format ]]; then
  STYLE="LLVM"
fi

files=()
while IFS= read -r -d '' file; do
  files+=("$file")
done < <(
  git ls-files -z \
    '*.c' '*.C' '*.cpp' '*.cc' '*.cxx' '*.c++' \
    '*.h' '*.H' '*.hpp' '*.hh' '*.hxx' '*.h++' \
    '*.ino' '*.pde' '*.cu' '*.proto'
)

if [[ "${#files[@]}" -eq 0 ]]; then
  exit 0
fi

"$CF_BIN" --dry-run --Werror -style="$STYLE" "${files[@]}"
