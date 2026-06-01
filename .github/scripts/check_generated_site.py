#!/usr/bin/env python3
# GitHub Actions で使用することを前提とするため、shebang は uv としない

import sys
from html.parser import HTMLParser
from pathlib import Path


BLOCKED_DOMAINS: tuple[bytes, ...] = (
    b"polyfill.io",
    b"bootcdn.net",
    b"bootcss.com",
    b"staticfile.net",
    b"staticfile.org",
    b"unionadjs.com",
    b"xhsbpza.com",
    b"union.macoms.la",
    b"newcrbpc.com",
    b"googie-anaiytics",
    b"kuurza.com",
)

CDN_DOMAINS_ALLOWED_ONLY_IN_VENDORED_MATHJAX: tuple[bytes, ...] = (
    b"cdn.jsdelivr.net",
    b"cdnjs.cloudflare.com",
)


def is_external_url(url: str) -> bool:
    value = url.strip().lower()
    return value.startswith(("http://", "https://", "//"))


class GeneratedHTMLParser(HTMLParser):
    def __init__(self) -> None:
        super().__init__(convert_charrefs=True)
        self.inline_script_count = 0
        self.external_script_srcs: list[str] = []
        self.external_stylesheet_hrefs: list[str] = []

    def handle_starttag(
        self,
        tag: str,
        attrs: list[tuple[str, str | None]],
    ) -> None:
        self.handle_tag(tag, attrs)

    def handle_startendtag(
        self,
        tag: str,
        attrs: list[tuple[str, str | None]],
    ) -> None:
        self.handle_tag(tag, attrs)

    def handle_tag(
        self,
        tag: str,
        attrs: list[tuple[str, str | None]],
    ) -> None:
        tag_name = tag.lower()
        attrs_by_name = {name.lower(): value for name, value in attrs}

        if tag_name == "script":
            src = attrs_by_name.get("src")

            if src is None:
                self.inline_script_count += 1
            elif is_external_url(src):
                self.external_script_srcs.append(src)

            return

        if tag_name != "link":
            return

        rel = attrs_by_name.get("rel")
        href = attrs_by_name.get("href")

        if rel is None or href is None:
            return

        rel_values = set(rel.lower().split())

        if "stylesheet" in rel_values and is_external_url(href):
            self.external_stylesheet_hrefs.append(href)


def is_vendored_mathjax_path(relative_path: Path) -> bool:
    return relative_path.as_posix().startswith("assets/vendor/mathjax/es5/")


def scan_blocked_domains(site_root: Path) -> list[str]:
    issues: list[str] = []

    for path in site_root.rglob("*"):
        if not path.is_file():
            continue

        relative_path = path.relative_to(site_root)
        content = path.read_bytes().lower()

        issues.extend(
            f"{relative_path}: contains blocked domain {domain.decode()}"
            for domain in BLOCKED_DOMAINS
            if domain in content
        )

        if not is_vendored_mathjax_path(relative_path):
            issues.extend(
                f"{relative_path}: contains unexpected CDN domain {domain.decode()}"
                for domain in CDN_DOMAINS_ALLOWED_ONLY_IN_VENDORED_MATHJAX
                if domain in content
            )

    return issues


def scan_html_external_assets(site_root: Path) -> list[str]:
    issues: list[str] = []

    for path in site_root.rglob("*.html"):
        relative_path = path.relative_to(site_root)
        parser = GeneratedHTMLParser()
        parser.feed(path.read_text(encoding="utf-8", errors="ignore"))

        if parser.inline_script_count > 0:
            issues.append(f"{relative_path}: contains inline <script>")

        issues.extend(
            f"{relative_path}: loads external script {src}"
            for src in parser.external_script_srcs
        )

        issues.extend(
            f"{relative_path}: loads external stylesheet {href}"
            for href in parser.external_stylesheet_hrefs
        )

    return issues


def main() -> int:
    if len(sys.argv) != 2:
        print("usage: check_generated_site.py <site-root>", file=sys.stderr)
        return 2

    site_root = Path(sys.argv[1])

    if not site_root.is_dir():
        print(f"{site_root} is not a directory", file=sys.stderr)
        return 2

    issues = [
        *scan_blocked_domains(site_root),
        *scan_html_external_assets(site_root),
    ]

    if issues:
        print("Generated site security check failed:", file=sys.stderr)
        for issue in issues:
            print(f"- {issue}", file=sys.stderr)
        return 1

    print("Generated site security check passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
