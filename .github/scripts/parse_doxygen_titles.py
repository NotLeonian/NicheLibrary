#!/usr/bin/env python3
# GitHub Actions でも使用するため、shebang は uv としない
from __future__ import annotations

import argparse
import json
import re
from pathlib import Path
from typing import Any

TITLE_RE = re.compile(r"@(?:title|brief)\s+(.+)")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="verify_files.json に Doxygen 由来のタイトル情報を反映する。",
    )
    parser.add_argument("verify_files", type=Path)
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    data: dict[str, Any] = json.loads(args.verify_files.read_text(encoding="utf-8"))

    files = data.get("files")
    if not isinstance(files, dict):
        raise SystemExit("The `files` field in `verify_files.json` is not an object.")

    for path_text, value in files.items():
        if not isinstance(path_text, str) or not isinstance(value, dict):
            continue

        source_path = Path(path_text)
        if not source_path.exists():
            continue

        match = TITLE_RE.search(source_path.read_text(encoding="utf-8"))
        if match is None:
            continue

        document_attributes = value.setdefault("document_attributes", {})
        if not isinstance(document_attributes, dict):
            document_attributes = {}
            value["document_attributes"] = document_attributes

        document_attributes["document_title"] = match.group(1).strip()

    args.verify_files.write_text(
        json.dumps(data, ensure_ascii=False, separators=(",", ":")) + "\n",
        encoding="utf-8",
    )


if __name__ == "__main__":
    main()
