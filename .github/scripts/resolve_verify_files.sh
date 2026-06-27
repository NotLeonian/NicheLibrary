#!/usr/bin/env bash
set -euo pipefail

ROOT="$(git rev-parse --show-toplevel)"
cd "$ROOT"

OUTPUT="${1:-verify_files.json}"
PYTHON_BIN="${PYTHON_BIN:-python3}"

if [[ -z "${COMPETITIVE_VERIFIER_CMD:-}" ]]; then
  if [[ "${GITHUB_ACTIONS:-}" == "true" ]]; then
    COMPETITIVE_VERIFIER_CMD="competitive-verifier"
  elif command -v uv >/dev/null 2>&1; then
    COMPETITIVE_VERIFIER_CMD="uv run --group dev competitive-verifier"
  else
    COMPETITIVE_VERIFIER_CMD="competitive-verifier"
  fi
fi
read -r -a CV_CMD <<< "$COMPETITIVE_VERIFIER_CMD"

"${CV_CMD[@]}" oj-resolve \
  --exclude .github/scripts \
  --config .competitive-verifier/config.toml \
  | tee "$OUTPUT"

"$PYTHON_BIN" .github/scripts/parse_doxygen_titles.py "$OUTPUT"
