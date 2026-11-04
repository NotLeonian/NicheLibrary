#!/usr/bin/env bash
set -euo pipefail

ROOT="$(git rev-parse --show-toplevel)"
cd "$ROOT"

if [[ -n "${CLANG_FORMAT_BIN:-}" ]]; then
  CF_CMD=("$CLANG_FORMAT_BIN")
else
  CF_CMD=(uv run --locked --group dev clang-format)
fi

"${CF_CMD[@]}" --version

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

"${CF_CMD[@]}" --dry-run --Werror -style="$STYLE" "${files[@]}"
