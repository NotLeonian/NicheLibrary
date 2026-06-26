#!/usr/bin/env bash
set -euo pipefail

ROOT="$(git rev-parse --show-toplevel)"
cd "$ROOT"

RUN_FORMAT="false"
RUN_CHECK="true"
SPLIT_SIZE="20"
TIMEOUT="1800"
USE_PREV_RESULT="true"
SAVE_RESULT="true"
FULL_VERIFY="false"
LOCAL_CACHE_DIR="${LOCAL_VERIFY_CACHE_DIR:-}"

prepare_self_ignored_dir() {
  local dir="$1"
  mkdir -p "$dir"

  if [[ ! -e "$dir/.gitignore" ]]; then
    printf '*\n' > "$dir/.gitignore"
  fi
}

prepare_self_ignored_dir ".competitive-verifier/bundled"
prepare_self_ignored_dir ".competitive-verifier/cache"

while [[ "$#" -gt 0 ]]; do
  case "$1" in
    --format)
      RUN_FORMAT="true"
      shift
      ;;
    --no-check)
      RUN_CHECK="false"
      shift
      ;;
    --split-size)
      SPLIT_SIZE="$2"
      shift 2
      ;;
    --timeout)
      TIMEOUT="$2"
      shift 2
      ;;
    --cache-dir)
      LOCAL_CACHE_DIR="$2"
      shift 2
      ;;
    --full)
      FULL_VERIFY="true"
      USE_PREV_RESULT="false"
      shift
      ;;
    --no-prev-result)
      USE_PREV_RESULT="false"
      shift
      ;;
    --no-save-result)
      SAVE_RESULT="false"
      shift
      ;;
    *)
      echo "unknown option: $1" >&2
      exit 2
      ;;
  esac
done

PYTHON_BIN="${PYTHON_BIN:-python3}"

if [[ -z "$LOCAL_CACHE_DIR" ]]; then
  cache_base="${XDG_CACHE_HOME:-$HOME/.cache}/niche-library/local-verify"
  repo_key="$("$PYTHON_BIN" - <<'PY'
import hashlib
import os

print(hashlib.sha256(os.getcwd().encode("utf-8")).hexdigest()[:16])
PY
)"
  LOCAL_CACHE_DIR="$cache_base/$repo_key"
fi

if [[ -z "${COMPETITIVE_VERIFIER_CMD:-}" ]]; then
  if command -v uv >/dev/null 2>&1; then
    export COMPETITIVE_VERIFIER_CMD="uv run --group dev competitive-verifier"
  else
    export COMPETITIVE_VERIFIER_CMD="competitive-verifier"
  fi
fi

if [[ "$RUN_FORMAT" == "true" ]]; then
  .github/scripts/check_format.sh
fi

LOCAL_PREV_RESULT="$LOCAL_CACHE_DIR/merged-result.json"

if [[ "$FULL_VERIFY" == "true" ]]; then
  rm -f "$LOCAL_PREV_RESULT"
  rm -rf .competitive-verifier/cache .verify-helper/cache
fi

TMP_DIR="$(mktemp -d "${TMPDIR:-/tmp}/niche-library-local-verify.XXXXXX")"
trap 'rm -rf "$TMP_DIR"' EXIT

VERIFY_FILES="$TMP_DIR/verify_files.json"
RESULT_ROOT="$TMP_DIR/results"

mkdir -p "$RESULT_ROOT"

.github/scripts/resolve_verify_files.sh "$VERIFY_FILES"

prev_result_args=()
if [[ "$USE_PREV_RESULT" == "true" && -f "$LOCAL_PREV_RESULT" ]]; then
  prev_result_args=(--prev-result "$LOCAL_PREV_RESULT")
fi

width="${#SPLIT_SIZE}"
if [[ "$width" -lt 2 ]]; then
  width="2"
fi

for i in $(seq 0 "$((SPLIT_SIZE - 1))"); do
  index="$(printf "%0${width}d" "$i")"
  output="$RESULT_ROOT/Result-$index/result.json"

  .github/scripts/run_verify_split.sh \
    --output "$output" \
    --verify-files "$VERIFY_FILES" \
    --split-size "$SPLIT_SIZE" \
    --split-index "$index" \
    --timeout "$TIMEOUT" \
    "${prev_result_args[@]}"
done

read -r -a CV_CMD <<< "$COMPETITIVE_VERIFIER_CMD"

shopt -s nullglob
result_files=("$RESULT_ROOT"/Result-*/result.json)
shopt -u nullglob

if [[ "${#result_files[@]}" -eq 0 ]]; then
  echo "The verify result has not been generated." >&2
  exit 1
fi

MERGED_TMP="$TMP_DIR/merged-result.json"
"${CV_CMD[@]}" merge-result "${result_files[@]}" | tee "$MERGED_TMP"

if [[ "$SAVE_RESULT" == "true" ]]; then
  mkdir -p "$LOCAL_CACHE_DIR"
  cp "$MERGED_TMP" "$LOCAL_PREV_RESULT"
  printf 'local\n' > "$LOCAL_CACHE_DIR/scope"
  printf '%s\n' "$ROOT" > "$LOCAL_CACHE_DIR/repository-root"
fi

if [[ "$RUN_CHECK" == "true" ]]; then
  "${CV_CMD[@]}" check "${result_files[@]}"
fi
