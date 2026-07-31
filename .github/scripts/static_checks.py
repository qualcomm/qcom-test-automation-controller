#!/usr/bin/env python3
# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause
"""
QTAC Self-Contained Static-Analysis PR Check

"""

import os
import re
import sys
import json
import shutil
import subprocess

try:
    import yaml
except ImportError:
    yaml = None

CONFIG_FILE = ".github/scripts/review_config.yml"

CPP_EXTS = {".cpp", ".cc", ".cxx", ".h", ".hpp"}
PY_EXTS = {".py"}

DEFAULT_SEVERITY_MAP = {
    "error": "critical", "warning": "warning",
    "performance": "info", "portability": "info", "style": "info",
    "high": "critical", "medium": "warning", "low": "info",
}
SEVERITY_ORDER = {"critical": 0, "warning": 1, "info": 2}
SEVERITY_ICON = {"critical": "🔴", "warning": "🟡", "info": "🔵"}


# ─────────────────────────────────────────────────────────────────────────────
# Config
# ─────────────────────────────────────────────────────────────────────────────
def load_config() -> dict:
    cfg = {}
    if yaml and os.path.exists(CONFIG_FILE):
        try:
            with open(CONFIG_FILE, "r", encoding="utf-8") as f:
                cfg = yaml.safe_load(f) or {}
        except Exception as e:
            print(f"[static] Warning: could not parse {CONFIG_FILE}: {e}")
    sa = cfg.get("static_analysis", {}) or {}
    return {
        "tools": {
            "cppcheck": sa.get("tools", {}).get("cppcheck", True),
            "clang_format": sa.get("tools", {}).get("clang_format", True),
            "flake8": sa.get("tools", {}).get("flake8", True),
            "bandit": sa.get("tools", {}).get("bandit", True),
        },
        "severity_map": {**DEFAULT_SEVERITY_MAP, **(sa.get("severity_map", {}) or {})},
        "max_issues_per_file": sa.get("max_issues_per_file", 8),
        "max_total_comments": sa.get("max_total_comments", 40),
        "skip_patterns": cfg.get("skip_patterns", []) or [],
    }


def should_skip(path: str, skip_patterns: list) -> bool:
    return any(p in path for p in skip_patterns)


# ─────────────────────────────────────────────────────────────────────────────
# Subprocess helper
# ─────────────────────────────────────────────────────────────────────────────
def _capture(cmd: list):
    """Run `cmd`, capturing output decoded as UTF-8.

    Windows runners default to cp1252, which raises UnicodeDecodeError on stray
    bytes in diff/tool output (crashing the reader thread and leaving stdout as
    None). Forcing utf-8 + errors='replace' makes this robust everywhere.
    """
    return subprocess.run(cmd, capture_output=True, encoding="utf-8",
                          errors="replace", check=False)


# ─────────────────────────────────────────────────────────────────────────────
# Changed-line detection
# ─────────────────────────────────────────────────────────────────────────────
def changed_lines(base: str, head: str) -> dict:
    """Return {path: set(added_line_numbers)} for the PR diff (base...head)."""
    result: dict = {}
    try:
        out = _capture(
            ["git", "diff", "--unified=0", "--no-color", f"{base}...{head}"]
        ).stdout or ""
    except Exception as e:
        print(f"[static] Warning: git diff failed: {e}")
        return result

    cur = None
    for line in out.splitlines():
        if line.startswith("+++ b/"):
            cur = line[6:]
            result.setdefault(cur, set())
        elif line.startswith("@@") and cur is not None:
            # @@ -a,b +c,d @@  → added lines start at c, run for d lines
            m = re.search(r"\+(\d+)(?:,(\d+))?", line)
            if m:
                start = int(m.group(1))
                count = int(m.group(2)) if m.group(2) is not None else 1
                for n in range(start, start + count):
                    result[cur].add(n)
    return result


def norm(path: str) -> str:
    return path.replace("\\", "/")


# ─────────────────────────────────────────────────────────────────────────────
# Tool runners — each returns a list of findings
#   finding = {"file","line","code","message","severity","tool"}
# ─────────────────────────────────────────────────────────────────────────────
def _have(tool: str) -> bool:
    return shutil.which(tool) is not None


def run_cppcheck(files: list, sev_map: dict) -> list:
    files = [f for f in files if os.path.splitext(f)[1].lower() in CPP_EXTS]
    if not files or not _have("cppcheck"):
        return []
    proc = _capture(
        ["cppcheck", "--enable=warning,performance,portability", "--quiet",
         "--template={file}:{line}:{severity}:{id}:{message}", *files]
    )
    findings = []
    for line in (proc.stderr or "").splitlines():
        parts = line.split(":", 4)
        if len(parts) < 5:
            continue
        fpath, lno, sev, cid, msg = parts
        if not lno.isdigit():
            continue
        findings.append({
            "file": norm(fpath), "line": int(lno),
            "code": cid, "message": msg.strip(),
            "severity": sev_map.get(sev.lower(), "warning"), "tool": "cppcheck",
        })
    return findings


def run_clang_format(files: list) -> list:
    tool = "clang-format" if _have("clang-format") else None
    files = [f for f in files if os.path.splitext(f)[1].lower() in CPP_EXTS]
    if not files or not tool:
        return []
    findings = []
    for f in files:
        proc = _capture([tool, "--dry-run", "--Werror", f])
        for line in (proc.stderr or "").splitlines():
            m = re.match(r"(.+?):(\d+):\d+:\s*(?:warning|error):\s*(.+)", line)
            if m:
                findings.append({
                    "file": norm(m.group(1)), "line": int(m.group(2)),
                    "code": "clang-format", "message": m.group(3).strip(),
                    "severity": "warning", "tool": "clang-format",
                })
    return findings


def run_flake8(files: list) -> list:
    files = [f for f in files if os.path.splitext(f)[1].lower() in PY_EXTS]
    if not files or not _have("flake8"):
        return []
    proc = _capture(
        ["flake8", "--max-line-length=120",
         "--format=%(path)s:%(row)d:%(col)d:%(code)s:%(text)s", *files]
    )
    findings = []
    for line in (proc.stdout or "").splitlines():
        parts = line.split(":", 4)
        if len(parts) < 5 or not parts[1].isdigit():
            continue
        fpath, lno, _col, code, msg = parts
        if code.startswith("E9") or code.startswith("F82"):
            sev = "critical"       # syntax errors / undefined names
        elif code.startswith("F"):
            sev = "warning"        # real pyflakes issues (unused import/var, etc.)
        else:
            sev = "info"           # style (E/W/C)
        findings.append({
            "file": norm(fpath), "line": int(lno),
            "code": code, "message": msg.strip(), "severity": sev, "tool": "flake8",
        })
    return findings


def run_bandit(files: list, sev_map: dict) -> list:
    files = [f for f in files if os.path.splitext(f)[1].lower() in PY_EXTS]
    if not files or not _have("bandit"):
        return []
    proc = _capture(["bandit", "-f", "json", "-q", *files])
    findings = []
    try:
        data = json.loads(proc.stdout or "{}")
    except json.JSONDecodeError:
        return []
    for r in data.get("results", []):
        findings.append({
            "file": norm(r.get("filename", "")), "line": r.get("line_number", 0),
            "code": r.get("test_id", "B?"), "message": r.get("issue_text", "").strip(),
            "severity": sev_map.get(str(r.get("issue_severity", "")).lower(), "warning"),
            "tool": "bandit",
        })
    return findings


# ─────────────────────────────────────────────────────────────────────────────
# Filter, format, post
# ─────────────────────────────────────────────────────────────────────────────
def filter_to_diff(findings: list, added: dict, skip_patterns: list) -> list:
    kept = []
    for f in findings:
        path = norm(f["file"])
        if should_skip(path, skip_patterns):
            continue
        added_lines = added.get(path)
        if added_lines is None:
            # match on basename-suffix in case tool emitted a relative path
            added_lines = next((v for k, v in added.items() if k.endswith(path)), None)
        if added_lines is None or f["line"] not in added_lines:
            continue
        kept.append(f)
    return kept


def cap(findings: list, per_file: int, total: int) -> tuple:
    findings.sort(key=lambda x: (SEVERITY_ORDER.get(x["severity"], 3), x["file"], x["line"]))
    seen_per_file: dict = {}
    kept, dropped = [], 0
    for f in findings:
        c = seen_per_file.get(f["file"], 0)
        if c >= per_file or len(kept) >= total:
            dropped += 1
            continue
        seen_per_file[f["file"]] = c + 1
        kept.append(f)
    return kept, dropped


def emit_annotations(findings: list):
    for f in findings:
        level = {"critical": "error", "warning": "warning", "info": "notice"}.get(
            f["severity"], "warning")
        print(f"::{level} file={f['file']},line={f['line']}::"
              f"[{f['tool']}:{f['code']}] {f['message']}")


def build_comment(findings: list, dropped: int, runner: str) -> str:
    lines = [f"## 🔧 QTAC Static Analysis ({runner})", ""]
    if not findings:
        lines.append("✅ No static-analysis issues found on the changed lines.")
    else:
        counts = {s: sum(1 for f in findings if f["severity"] == s)
                  for s in ("critical", "warning", "info")}
        lines.append(f"Found **{len(findings)}** issue(s) on changed lines — "
                     f"{SEVERITY_ICON['critical']} {counts['critical']} · "
                     f"{SEVERITY_ICON['warning']} {counts['warning']} · "
                     f"{SEVERITY_ICON['info']} {counts['info']}")
        if dropped:
            lines.append(f"> {dropped} additional issue(s) omitted (per-file / total cap).")
        lines.append("")
        cur_file = None
        for f in findings:
            if f["file"] != cur_file:
                cur_file = f["file"]
                lines.append(f"\n**`{cur_file}`**")
            lines.append(f"- {SEVERITY_ICON[f['severity']]} L{f['line']} "
                         f"`{f['tool']}:{f['code']}` — {f['message']}")
    lines += ["", "> 🔧 _Automated static analysis (deterministic tools) — "
              "advisory. Suppress with the tool's inline directive "
              "(`// NOLINT`, `# noqa`)._"]
    return "\n".join(lines)


def post_comment(body: str):
    token = os.environ.get("GH_TOKEN") or os.environ.get("GITHUB_TOKEN")
    repo = os.environ.get("REPO_FULL_NAME")
    pr = os.environ.get("PR_NUMBER")
    if not (token and repo and pr and _have("gh")):
        print("[static] (no gh/token/PR — printing comment instead of posting)\n")
        print(body)
        return
    env = {**os.environ, "GH_TOKEN": token}
    subprocess.run(["gh", "pr", "comment", pr, "--repo", repo, "--body", body],
                   env=env, check=False)


def main():
    # Windows runners default stdout/stderr to cp1252; the emoji in our output
    # would raise UnicodeEncodeError. Force UTF-8 where supported.
    for stream in (sys.stdout, sys.stderr):
        try:
            stream.reconfigure(encoding="utf-8", errors="replace")
        except Exception:
            pass

    base = os.environ.get("BASE_SHA", "")
    head = os.environ.get("HEAD_SHA", "HEAD")
    runner = os.environ.get("RUNNER_OS", "ci")
    cfg = load_config()

    if not base:
        print("[static] ERROR: BASE_SHA not set")
        return 1

    added = changed_lines(base, head)
    changed_files = list(added.keys())
    print(f"[static] {len(changed_files)} changed file(s)")

    findings = []
    if cfg["tools"]["cppcheck"]:
        findings += run_cppcheck(changed_files, cfg["severity_map"])
    if cfg["tools"]["clang_format"]:
        findings += run_clang_format(changed_files)
    if cfg["tools"]["flake8"]:
        findings += run_flake8(changed_files)
    if cfg["tools"]["bandit"]:
        findings += run_bandit(changed_files, cfg["severity_map"])

    findings = filter_to_diff(findings, added, cfg["skip_patterns"])
    findings, dropped = cap(findings, cfg["max_issues_per_file"], cfg["max_total_comments"])
    print(f"[static] {len(findings)} finding(s) on changed lines ({dropped} capped)")

    emit_annotations(findings)
    post_comment(build_comment(findings, dropped, runner))

    # Write a machine-readable artifact for the run.
    with open(f"review_results_{runner}.json", "w", encoding="utf-8") as f:
        json.dump({"runner": runner, "findings": findings, "dropped": dropped}, f, indent=2)
    return 0


if __name__ == "__main__":
    sys.exit(main())