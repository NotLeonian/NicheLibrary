#!/usr/bin/env bash
set -euo pipefail

ROOT="$(git rev-parse --show-toplevel)"
cd "$ROOT"

OUTPUT=""
VERIFY_FILES="verify_files.json"
SPLIT_SIZE="20"
SPLIT_INDEX=""
TIMEOUT="1800"
PREV_RESULT=""
FULL_VERIFY="false"

while [[ "$#" -gt 0 ]]; do
  case "$1" in
    --output)
      OUTPUT="$2"
      shift 2
      ;;
    --verify-files)
      VERIFY_FILES="$2"
      shift 2
      ;;
    --split-size)
      SPLIT_SIZE="$2"
      shift 2
      ;;
    --split-index)
      SPLIT_INDEX="$2"
      shift 2
      ;;
    --timeout)
      TIMEOUT="$2"
      shift 2
      ;;
    --prev-result)
      PREV_RESULT="$2"
      shift 2
      ;;
    --full)
      FULL_VERIFY="true"
      shift
      ;;
    *)
      echo "unknown option: $1" >&2
      exit 2
      ;;
  esac
done

if [[ -z "$OUTPUT" ]]; then
  echo "`--output` is required." >&2
  exit 2
fi

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

if [[ "$FULL_VERIFY" == "true" ]]; then
  rm -rf .competitive-verifier/cache .verify-helper/cache
  PREV_RESULT=""
fi

mkdir -p "$(dirname "$OUTPUT")"

cmd=(
  "${CV_CMD[@]}"
  verify
  --check-error
  --timeout "$TIMEOUT"
  --output "$OUTPUT"
)

if [[ -n "$SPLIT_SIZE" ]]; then
  cmd+=(--split "$SPLIT_SIZE")
fi
if [[ -n "$SPLIT_INDEX" ]]; then
  cmd+=(--split-index "$SPLIT_INDEX")
fi
if [[ -n "$PREV_RESULT" ]]; then
  cmd+=(--prev-result "$PREV_RESULT")
fi

set +e
COMPETITIVE_VERIFY_FILES_PATH="$VERIFY_FILES" "${cmd[@]}"
status="$?"
set -e

echo "competitive-verifier verify status: $status"
exit 0
