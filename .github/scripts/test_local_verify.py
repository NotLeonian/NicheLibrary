from __future__ import annotations

import json
import os
import shutil
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path
from typing import Any

SCRIPTS = Path(__file__).resolve().parent


class LocalVerifyTest(unittest.TestCase):
    def test_untracked_sources_and_dependencies(self) -> None:
        with tempfile.TemporaryDirectory(prefix="niche-library-verify-test-") as temp:
            root = Path(temp) / "repo"
            root.mkdir()
            env = os.environ.copy()
            # 呼び出し元の Git の設定で別の作業ツリーを参照しないようにする。
            for name in ("GIT_DIR", "GIT_WORK_TREE", "GIT_INDEX_FILE"):
                env.pop(name, None)
            env["GITHUB_ACTIONS"] = "false"
            env["PYTHON_BIN"] = sys.executable
            subprocess.run(["git", "init", "--quiet", str(root)], check=True, env=env)

            scripts = root / ".github/scripts"
            scripts.mkdir(parents=True)
            for name in (
                "local_verify.sh",
                "resolve_verify_files.sh",
                "run_verify_split.sh",
                "parse_doxygen_titles.py",
            ):
                shutil.copy2(SCRIPTS / name, scripts / name)

            config = root / ".competitive-verifier/config.toml"
            config.parent.mkdir()
            config.write_text(
                '[[languages.cpp.environments]]\n'
                'CXX = "g++"\n'
                'CXXFLAGS = ["-std=c++20", "-Wall", "-Wextra"]\n',
                encoding="utf-8",
            )
            runner = Path(temp) / "verifier.py"
            runner.write_text(
                "import sys\n"
                "from competitive_verifier.app import main\n"
                "sys.exit(main())\n",
                encoding="utf-8",
            )
            env["COMPETITIVE_VERIFIER_CMD"] = f"{sys.executable} {runner}"

            # バージョンが同じでも、標準エラー出力が呼び出すたびに変わる場合を再現する。
            bin_dir = Path(temp) / "bin"
            bin_dir.mkdir()
            clang = bin_dir / "clang"
            clang.write_text(
                '#!/usr/bin/env bash\n'
                'printf "test clang version 1\\n"\n'
                'printf "diagnostic pid=%s\\n" "$$" >&2\n',
                encoding="utf-8",
            )
            clang.chmod(0o755)
            env["PATH"] = f"{bin_dir}{os.pathsep}{env['PATH']}"

            source = root / "verify/standalone-cache.test.cpp"
            source.parent.mkdir()
            source.write_text(
                '// competitive-verifier: STANDALONE\n\n'
                '#include <fstream>\n\n'
                '#include "../value.hpp"\n\n'
                'int main() {\n'
                '    std::ofstream("runs.txt", std::ios::app) << "run\\n";\n'
                '    if (value != 0) {\n'
                '        return 1;\n'
                '    }\n\n'
                '    return 0;\n'
                '}\n',
                encoding="utf-8",
            )
            header = root / "value.hpp"
            header_content = (
                "#ifndef VALUE_HPP\n#define VALUE_HPP\n\n"
                "constexpr int value = 0;\n\n#endif\n"
            )
            header.write_text(header_content, encoding="utf-8")
            (root / ".gitignore").write_text("ignored/\nruns.txt\n", encoding="utf-8")
            ignored = root / "ignored/standalone-invalid.test.cpp"
            ignored.parent.mkdir()
            ignored.write_text(
                "// competitive-verifier: STANDALONE\n#error ignored file\n",
                encoding="utf-8",
            )

            cache = Path(temp) / "cache"
            last_output = ""

            def run_local(*args: str, success: bool = True) -> list[dict[str, Any]]:
                nonlocal last_output
                proc = subprocess.run(
                    [
                        "bash",
                        str(scripts / "local_verify.sh"),
                        "--cache-dir",
                        str(cache),
                        "--split-size",
                        "1",
                        *args,
                    ],
                    cwd=root,
                    env=env,
                    capture_output=True,
                    text=True,
                    check=False,
                    timeout=120,
                )
                last_output = proc.stdout + proc.stderr
                self.assertEqual(proc.returncode == 0, success, last_output)
                self.assertFalse((root / ".git/index").exists())
                return [
                    json.loads(line)
                    for line in proc.stdout.splitlines()
                    if line.startswith("{")
                ]

            def assert_run_count(expected: int) -> None:
                path = root / "runs.txt"
                actual = len(path.read_text().splitlines()) if path.exists() else 0
                cached_results = sorted(
                    str(p.relative_to(cache))
                    for p in cache.glob("*/*/merged-result.json")
                )
                self.assertEqual(
                    actual,
                    expected,
                    last_output + f"\nCached results: {cached_results}\n",
                )

            resolved = run_local()[0]["files"]
            self.assertIn(source.relative_to(root).as_posix(), resolved)
            self.assertIn(
                header.name,
                resolved[source.relative_to(root).as_posix()]["dependencies"],
            )
            self.assertNotIn(ignored.relative_to(root).as_posix(), resolved)
            assert_run_count(1)

            run_local()
            assert_run_count(1)

            stat = header.stat()
            os.utime(header, ns=(stat.st_atime_ns, stat.st_mtime_ns + 10_000_000_000))
            run_local()
            assert_run_count(1)

            # 更新時刻が同じでも、依存先の内容が変わった場合は再実行する。
            stat = header.stat()
            header.write_text(header_content.replace("= 0", "= 1"), encoding="utf-8")
            os.utime(header, ns=(stat.st_atime_ns, stat.st_mtime_ns))
            run_local(success=False)
            assert_run_count(2)
            run_local(success=False)
            assert_run_count(3)

            header.write_text(header_content, encoding="utf-8")
            run_local()
            assert_run_count(4)

            stat = source.stat()
            source.write_text(source.read_text() + "\n", encoding="utf-8")
            os.utime(source, ns=(stat.st_atime_ns, stat.st_mtime_ns))
            previous = next(cache.glob("*/*/merged-result.json"))
            saved_result = previous.read_bytes()
            run_local("--no-save-result")
            assert_run_count(5)
            self.assertEqual(previous.read_bytes(), saved_result)
            run_local()
            assert_run_count(6)
            run_local("--no-prev-result")
            assert_run_count(7)
            run_local("--full")
            assert_run_count(8)

            # ファイルのサイズと更新時刻が同じでも、バージョンが変われば再実行する。
            stat = clang.stat()
            clang.write_text(
                clang.read_text().replace("version 1", "version 2"), encoding="utf-8"
            )
            os.utime(clang, ns=(stat.st_atime_ns, stat.st_mtime_ns))
            run_local()
            assert_run_count(9)
            run_local()
            assert_run_count(9)

            # CI と共通の resolver を直接呼ぶ場合は、未追跡のファイルを含めない。
            resolved_path = Path(temp) / "verify_files.json"
            subprocess.run(
                ["bash", str(scripts / "resolve_verify_files.sh"), str(resolved_path)],
                cwd=root,
                env=env | {"GITHUB_ACTIONS": "true"},
                capture_output=True,
                check=True,
                timeout=120,
            )
            self.assertEqual(json.loads(resolved_path.read_text())["files"], {})


if __name__ == "__main__":
    unittest.main()
