#!/usr/bin/env python3
# GitHub Actions で使用することを前提とするため、shebang は uv としない

import re
import subprocess
import sys
import tomllib
from pathlib import Path
from typing import Any, cast

EXPECTED_GIT_URL = "https://github.com/NotLeonian/competitive-verifier"
SHA_RE = re.compile(r"^[0-9a-fA-F]{40}$")


def error(message: str) -> None:
    print(f"::error::{message}", file=sys.stderr)
    raise SystemExit(1)


def load_pyproject() -> dict[str, Any]:
    with Path("pyproject.toml").open("rb") as f:
        return tomllib.load(f)


def load_competitive_verifier_source() -> dict[str, Any]:
    data = load_pyproject()
    try:
        source = data["tool"]["uv"]["sources"]["competitive-verifier"]
    except KeyError:
        error("pyproject.toml に [tool.uv.sources].competitive-verifier がありません")

    if not isinstance(source, dict):
        error("[tool.uv.sources].competitive-verifier はテーブルで指定してください")

    return source


def normalize_git_url(source: dict[str, Any]) -> str:
    _git_url = source.get("git")
    if not isinstance(_git_url, str):
        error("competitive-verifier の URL は文字列で指定してください")

    git_url = cast(str, _git_url)

    url_without_suffix = git_url[:-4] if git_url.endswith(".git") else git_url
    if url_without_suffix != EXPECTED_GIT_URL:
        error(
            f"competitive-verifier の URL がこのスクリプトで指定されたものと異なります: {git_url!r}"
        )

    return EXPECTED_GIT_URL + ".git"


def read_source_ref(source: dict[str, Any]) -> tuple[str, str]:
    if "branch" in source:
        error(
            "competitive-verifier は branch ではなく、rev（commit SHA）または tag で固定してください"
        )

    has_rev = "rev" in source
    has_tag = "tag" in source

    if has_rev == has_tag:
        error(
            "competitive-verifier は rev または tag のどちらか一方のみによって固定してください"
        )

    if has_rev:
        rev = source["rev"]
        if not isinstance(rev, str) or not SHA_RE.fullmatch(rev):
            error(
                "competitive-verifier の rev は 40 文字の commit SHA で指定してください"
            )
        return "rev", rev.lower()

    tag = source["tag"]
    if not isinstance(tag, str) or tag == "":
        error("competitive-verifier の tag は空でない文字列で指定してください")
    if "\n" in tag or "\r" in tag:
        error("competitive-verifier の tag に改行を含めないでください")

    check = subprocess.run(
        ["git", "check-ref-format", f"refs/tags/{tag}"],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.PIPE,
        text=True,
    )
    if check.returncode != 0:
        error(f"competitive-verifier の tag 名が invalid です: {tag!r}")

    return "tag", tag


def resolve_tag_to_commit(git_url: str, tag: str) -> str:
    ref_name = f"refs/tags/{tag}"
    proc = subprocess.run(
        ["git", "ls-remote", "--tags", git_url, ref_name, f"{ref_name}^{{}}"],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
    )
    if proc.returncode != 0:
        error(f"competitive-verifier の tag を取得できません: {tag!r}")

    direct_sha: str | None = None
    peeled_sha: str | None = None

    for line in proc.stdout.splitlines():
        try:
            sha, name = line.split("\t", 1)
        except ValueError:
            continue
        if name == ref_name:
            direct_sha = sha
        elif name == f"{ref_name}^{{}}":
            peeled_sha = sha

    resolved = peeled_sha or direct_sha
    if resolved is None or not SHA_RE.fullmatch(resolved):
        error(f"competitive-verifier の tag を commit SHA に解決できません: {tag!r}")
        assert False  # 上の error で落ちる、型注釈のエラーを防ぐため

    return resolved.lower()


def main() -> None:
    source = load_competitive_verifier_source()
    git_url = normalize_git_url(source)
    source_kind, source_ref = read_source_ref(source)

    if source_kind == "rev":
        resolved_ref = source_ref
    else:
        resolved_ref = resolve_tag_to_commit(git_url, source_ref)

    print(f"source_kind={source_kind}")
    print(f"source_ref={source_ref}")
    print(f"resolved_ref={resolved_ref}")
    print(f"package=git+{git_url}@{resolved_ref}")


if __name__ == "__main__":
    main()
