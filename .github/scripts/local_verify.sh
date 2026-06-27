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
PYTHON_BIN="${PYTHON_BIN:-python3}"
LOCAL_CACHE_ROOT="${LOCAL_VERIFY_CACHE_DIR:-${XDG_CACHE_HOME:-$HOME/.cache}/niche-library/local-verify}"

compute_repo_cache_key() {
  "$PYTHON_BIN" - "$ROOT" <<'PY'
import hashlib
import pathlib
import sys

root = pathlib.Path(sys.argv[1]).resolve()
print(hashlib.sha256(str(root).encode("utf-8")).hexdigest()[:16])
PY
}

compute_local_verify_input_key() {
  "$PYTHON_BIN" - "$ROOT" "$COMPETITIVE_VERIFIER_CMD" <<'PY'
import hashlib
import os
import pathlib
import shutil
import subprocess
import sys

root = pathlib.Path(sys.argv[1]).resolve()
competitive_verifier_cmd = sys.argv[2]

h = hashlib.sha256()

def add_bytes(label: str, data: bytes) -> None:
    h.update(label.encode("utf-8"))
    h.update(b"\0")
    h.update(data)
    h.update(b"\0")

def add_text(label: str, value: str) -> None:
    add_bytes(label, value.encode("utf-8", "surrogateescape"))

def add_file(rel: str) -> None:
    path = root / rel
    if path.is_file():
        add_text(f"file:{rel}:exists", "1")
        add_bytes(f"file:{rel}:content", path.read_bytes())
    else:
        add_text(f"file:{rel}:exists", "0")

def add_command_version(command: str) -> None:
    resolved = shutil.which(command)
    add_text(f"tool:{command}:path", resolved or "")

    if not resolved:
        return

    try:
        stat = pathlib.Path(resolved).stat()
        add_text(f"tool:{command}:stat", f"{stat.st_size}:{stat.st_mtime_ns}")
    except OSError as e:
        add_text(f"tool:{command}:stat-error", repr(e))

    try:
        completed = subprocess.run(
            [resolved, "--version"],
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            timeout=3,
            check=False,
        )
    except Exception as e:
        add_text(f"tool:{command}:version-error", repr(e))
    else:
        add_text(f"tool:{command}:version-code", str(completed.returncode))
        add_text(f"tool:{command}:version-output", completed.stdout[:4096])

add_text("schema", "niche-library-local-verify-cache-v2")
add_text("competitive-verifier-cmd", competitive_verifier_cmd)

for rel in [
    ".competitive-verifier/config.toml",
    "pyproject.toml",
    "uv.lock",
    ".github/scripts/resolve_verify_files.sh",
    ".github/scripts/parse_doxygen_titles.py",
    ".github/scripts/run_verify_split.sh",
    ".github/scripts/local_verify.sh",
]:
    add_file(rel)

for name in [
    "CC",
    "CXX",
    "CPP",
    "CPPFLAGS",
    "CFLAGS",
    "CXXFLAGS",
    "LDFLAGS",
    "LIBRARY_PATH",
    "CPATH",
    "CMAKE_C_COMPILER",
    "CMAKE_CXX_COMPILER",
    "CMAKE_PREFIX_PATH",
]:
    add_text(f"env:{name}", os.environ.get(name, ""))

for command in [
    "cc",
    "c++",
    "gcc",
    "g++",
    "clang",
    "clang++",
]:
    add_command_version(command)

print(h.hexdigest()[:32])
PY
}

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
      LOCAL_CACHE_ROOT="$2"
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

if [[ -z "$LOCAL_CACHE_ROOT" ]]; then
  LOCAL_CACHE_ROOT="${XDG_CACHE_HOME:-$HOME/.cache}/niche-library/local-verify"
fi

repo_key="$("$PYTHON_BIN" - "$ROOT" <<'PY'
import hashlib
import pathlib
import sys
root = pathlib.Path(sys.argv[1]).resolve()
print(hashlib.sha256(str(root).encode("utf-8")).hexdigest()[:16])
PY
)"

LOCAL_REPO_CACHE_DIR="$LOCAL_CACHE_ROOT/$repo_key"

if [[ -z "${COMPETITIVE_VERIFIER_CMD:-}" ]]; then
  if command -v uv >/dev/null 2>&1; then
    export COMPETITIVE_VERIFIER_CMD="uv run --group dev competitive-verifier"
  else
    export COMPETITIVE_VERIFIER_CMD="competitive-verifier"
  fi
fi

REPO_CACHE_KEY="$(compute_repo_cache_key)"
LOCAL_REPO_CACHE_DIR="$LOCAL_CACHE_ROOT/$REPO_CACHE_KEY"

if [[ "$RUN_FORMAT" == "true" ]]; then
  .github/scripts/check_format.sh
fi

if [[ "$FULL_VERIFY" == "true" ]]; then
  USE_PREV_RESULT="false"
  rm -rf .competitive-verifier/cache
  rm -rf "$LOCAL_REPO_CACHE_DIR"
fi

prepare_self_ignored_dir() {
  local dir="$1"
  mkdir -p "$dir"

  if [[ ! -e "$dir/.gitignore" ]]; then
    printf '*\n' > "$dir/.gitignore"
  fi
}

prepare_self_ignored_dir ".competitive-verifier/bundled"
prepare_self_ignored_dir ".competitive-verifier/cache"

VERIFY_INPUT_CACHE_KEY="$(compute_local_verify_input_key)"
LOCAL_CACHE_DIR="$LOCAL_REPO_CACHE_DIR/$VERIFY_INPUT_CACHE_KEY"
LOCAL_PREV_RESULT="$LOCAL_CACHE_DIR/merged-result.json"

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

  if [[ ! -s "$output" ]]; then
    echo "The verify result was not generated: $output" >&2
    exit 1
  fi
done

read -r -a CV_CMD <<< "$COMPETITIVE_VERIFIER_CMD"

shopt -s nullglob
result_files=("$RESULT_ROOT"/Result-*/result.json)
shopt -u nullglob

if [[ "${#result_files[@]}" -eq 0 ]]; then
  echo "The verify result has not been generated." >&2
  exit 1
fi

if [[ "${#result_files[@]}" -ne "$SPLIT_SIZE" ]]; then
  echo "Unexpected number of verify results: expected=$SPLIT_SIZE actual=${#result_files[@]}" >&2
  printf 'found: %s\n' "${result_files[@]}" >&2
  exit 1
fi

MERGED_TMP="$TMP_DIR/merged-result.json"
"${CV_CMD[@]}" merge-result "${result_files[@]}" | tee "$MERGED_TMP"

if [[ "$SAVE_RESULT" == "true" ]]; then
  mkdir -p "$LOCAL_CACHE_DIR"
  cp "$MERGED_TMP" "$LOCAL_PREV_RESULT"
  cat > "$LOCAL_CACHE_DIR/fingerprint.txt" <<EOF
repository=$ROOT
repository_key=$REPO_CACHE_KEY
verify_input_key=$VERIFY_INPUT_CACHE_KEY
competitive_verifier_cmd=$COMPETITIVE_VERIFIER_CMD
EOF
fi

if [[ "$RUN_CHECK" == "true" ]]; then
  "${CV_CMD[@]}" check "${result_files[@]}"
fi
