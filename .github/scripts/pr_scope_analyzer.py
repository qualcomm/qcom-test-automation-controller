#!/usr/bin/env python3
# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause
"""
QTAC PR Scope Analyzer — Token-Budget Pre-Processor
"""

import os
import sys
import json
import re
import requests
from datetime import datetime

# ─────────────────────────────────────────────────────────────────────────────
# Token budget constants
# ─────────────────────────────────────────────────────────────────────────────
CHARS_PER_TOKEN = 3.5          # rough estimate for code
TARGET_TOKENS_PER_BATCH = int(os.environ.get("REVIEW_TOKENS_PER_BATCH", "100000"))
TARGET_CHARS_PER_BATCH = int(TARGET_TOKENS_PER_BATCH * CHARS_PER_TOKEN)
MAX_LINES_PER_FILE = int(os.environ.get("REVIEW_MAX_LINES_PER_FILE", "2000"))
MAX_CONTEXT_LINES = 3          # keep N lines of context around each change

MAX_BATCHES = int(os.environ.get("REVIEW_MAX_BATCHES", "5"))

# ─────────────────────────────────────────────────────────────────────────────
# File type definitions — priority 1 = always review first
# ─────────────────────────────────────────────────────────────────────────────
FILE_TYPES = {
    "cpp_header": {
        "extensions": [".h", ".hpp"],
        "priority": 1,
        "checklist": [
            "overriding functions (that redeclare a base-class virtual) must use `override` — do NOT flag first/base virtual declarations, which correctly have no `override`",
            "no `using namespace` in headers",
            "new public methods: should they be `const`?",
            "`#define` constants → replace with `constexpr`",
            "include guards or `#pragma once` present",
        ],
    },
    "cpp_source": {
        "extensions": [".cpp", ".cc", ".cxx"],
        "priority": 1,
        "checklist": [
            # Safety
            "raw `new` without smart pointer or `deleteLater()`",
            "`mutex.lock()` without `QMutexLocker` (not exception-safe)",
            "GUI ops (`show/hide/resize/update`) in worker threads",
            "accessing `_driveThread` without null check",
            "empty `catch` blocks that swallow exceptions",
            "static local variables in member functions (thread-unsafe)",
            "`new` inside loops without guaranteed cleanup",
            "old-style `SIGNAL()`/`SLOT()` macros → use pointer-to-member",
            # Security / correctness
            "hardcoded secrets/tokens in C++ literals (QString/char*/QByteArray) — must not be committed",
            "busy-wait polling loops (while-loop + sleep/msleep/usleep/Sleep) — prefer proper wait/condition/event",
            "new magic numbers in business logic (non-hardware constants) — require named constant + comment",
            # Potential bugs
            "integer overflow: arithmetic on `int` that could exceed INT_MAX (use qint64)",
            "uninitialized member variables (not set in constructor)",
            "use-after-free: accessing pointer after delete/deleteLater()",
            "missing `break` in switch case (unintentional fallthrough)",
            "signed/unsigned comparison: `int` vs `size_t`/`quint32`",
            "off-by-one: loop `<= size` when `< size` is correct, or vice versa",
            "float equality: `float == float` → use qFuzzyCompare()",
            "unchecked return value of error-signaling functions",
            # Dead code
            "unreachable code after `return`/`throw`",
            "always-true/false conditions: `if (x == x)`, `if (true)`",
            "`#if 0` disabled code blocks that should be removed",
            "variables declared but never read after assignment",
            # Optimization
            "QString/QByteArray construction inside loops (use reserve() outside)",
            "container.count()/.size() called in loop condition (cache before loop)",
            "unnecessary deep copy: pass QByteArray/QString by const ref not value",
            "QStringLiteral missing: use QStringLiteral(\"...\") for string literals",
            "QList/QVector appended in loop without reserve() pre-allocation",
        ],
    },
    "cmake": {
        "filenames": ["CMakeLists.txt"],
        "priority": 1,
        "checklist": [
            "target names: `Qt6::Core` not `Qt6Core`",
            "`find_package` has all needed components",
            "platform guards: `if(WIN32)`, `if(UNIX)`, ARM64 check",
            "new source files added to correct target",
            "no hardcoded absolute paths",
        ],
    },
    "thrift": {
        "extensions": [".thrift"],
        "priority": 1,
        "checklist": [
            "🔴 BREAKING: no field removal or type changes",
            "🔴 BREAKING: no service method renames",
            "field IDs unique and not reused",
            "new methods have handler implementations",
            "✅ SAFE: adding new optional fields with new IDs",
        ],
    },
    "python": {
        "extensions": [".py"],
        "priority": 2,
        "checklist": [
            "bare `except:` → use specific exception types",
            "hardcoded credentials/tokens/passwords",
            "hardcoded IP addresses/hostnames (should be configurable)",
            "`eval()` or `exec()` usage",
            "unused imports",
            "lines over 120 characters",
            "SKIP: files in `thrift/` subdirectory (auto-generated)",
        ],
    },
    "yaml_workflow": {
        "path_prefix": ".github/workflows/",
        "priority": 2,
        "checklist": [
            "permissions block is minimal",
            "secrets via `${{ secrets.NAME }}` not hardcoded",
            "third-party actions pinned (avoid `@main`; prefer full commit SHA where feasible)",
            "no `pull_request_target` with untrusted code",
        ],
    },
    "powershell": {
        "extensions": [".ps1"],
        "priority": 3,
        "checklist": [
            "`$ErrorActionPreference = 'Stop'` at top",
            "hardcoded paths that should be parameterized",
            "version numbers from CMakeLists.txt not hardcoded",
            "both x64 and ARM64 handled",
        ],
    },
    "shell": {
        "extensions": [".sh"],
        "priority": 3,
        "checklist": [
            "`set -e` and `set -u` at top",
            "variables properly quoted",
            "hardcoded absolute paths",
        ],
    },
    "yaml_config": {
        "extensions": [".yml", ".yaml"],
        "priority": 3,
        "checklist": [
            "valid YAML syntax",
            "no hardcoded IPs or secrets",
            "null values where real value expected",
        ],
    },
    "json": {
        "extensions": [".json"],
        "priority": 3,
        "checklist": [
            "valid JSON syntax",
            "no hardcoded secrets or IPs",
            "schema consistency with existing entries",
        ],
    },
    "csharp": {
        "extensions": [".cs", ".csproj"],
        "priority": 2,
        "checklist": [
            "DllImport calling convention and charset correct",
            "marshaling attributes match C++ types",
            "unsafe blocks justified",
        ],
    },
    "markdown": {
        "extensions": [".md"],
        "priority": 4,
        "checklist": [
            "code examples match current API",
            "no broken relative links",
            "version numbers up to date",
        ],
    },
}

SKIP_EXTENSIONS = {
    ".tcnf", ".nsi",
    ".pptx", ".png", ".jpg", ".jpeg", ".ico", ".bmp",
    ".exe", ".dll", ".lib", ".a", ".so", ".pdb",
    ".tgz", ".zip", ".tar", ".gz",
}

SKIP_SUBSTRINGS = [
    "__Builds/", "__Installer/", "third-party/",
    "src/applications/tacservice/python/thrift/",
    "build_final_verify.log", "updatedevicelist_",
    "hex_test", "test_delayed_expansion",
    "test_e2e_detection", "test_skip_logic", "test_manifest",
    "test_fallback", "test_v6",
    "moc_", "ui_", "qrc_", ".pro.user",
]


# ─────────────────────────────────────────────────────────────────────────────
# Helpers
# ─────────────────────────────────────────────────────────────────────────────

def should_skip(path: str) -> bool:
    ext = os.path.splitext(path)[1].lower()
    if ext in SKIP_EXTENSIONS:
        return True
    return any(sub in path for sub in SKIP_SUBSTRINGS)


def categorize(path: str) -> str:
    filename = os.path.basename(path)
    ext = os.path.splitext(filename)[1].lower()
    for cat, info in FILE_TYPES.items():
        if filename in info.get("filenames", []):
            return cat
    for cat, info in FILE_TYPES.items():
        prefix = info.get("path_prefix", "")
        if prefix and path.startswith(prefix):
            return cat
    for cat, info in FILE_TYPES.items():
        if ext in info.get("extensions", []):
            return cat
    return "other"


def estimate_tokens(text: str) -> int:
    return int(len(text) / CHARS_PER_TOKEN)


# Categories whose files should be paired by class (header <-> source).
PAIRED_CATEGORIES = {"cpp_header", "cpp_source"}


def unit_key(item: dict) -> str:
    if item["category"] in PAIRED_CATEGORIES:
        path = item["path"]
        stem = os.path.splitext(os.path.basename(path))[0]
        return f"{os.path.dirname(path)}/{stem}".lower()
    return item["path"]


def compress_patch(patch: str, max_lines: int = MAX_LINES_PER_FILE) -> str:
    if not patch:
        return ""

    lines = patch.split("\n")
    if len(lines) <= max_lines:
        return patch

    important_indices = set()
    for i, line in enumerate(lines):
        if line.startswith("@@") or line.startswith("+") or line.startswith("-"):
            for j in range(
                max(0, i - MAX_CONTEXT_LINES),
                min(len(lines), i + MAX_CONTEXT_LINES + 1)
            ):
                important_indices.add(j)

    compressed = []
    last_included = -1
    for i, line in enumerate(lines):
        if i in important_indices:
            if last_included >= 0 and i > last_included + 1:
                skipped = i - last_included - 1
                compressed.append(f"... [{skipped} unchanged lines omitted] ...")
            compressed.append(line)
            last_included = i

    result = "\n".join(compressed)

    # Hard truncate if still too long
    if len(result.split("\n")) > max_lines:
        truncated_lines = result.split("\n")[:max_lines]
        truncated_lines.append(f"... [diff truncated at {max_lines} lines to stay within token budget] ...")
        result = "\n".join(truncated_lines)

    return result


# NOTE: Learned false positives are no longer injected here. 

def get_pr_info(token: str, api_url: str, repo: str, pr_number: int) -> dict:
    """Fetch PR title + description so the reviewer knows the author's intent."""
    headers = {
        "Authorization": f"token {token}",
        "Accept": "application/vnd.github.v3+json",
    }
    try:
        resp = requests.get(
            f"{api_url}/repos/{repo}/pulls/{pr_number}",
            headers=headers, timeout=30,
        )
        if resp.status_code == 200:
            return resp.json()
        print(f"[scope] Warning: PR info API {resp.status_code}")
    except Exception as e:
        print(f"[scope] Warning: PR info fetch failed: {e}")
    return {}


def format_pr_intent(pr_info: dict, max_body_chars: int = 1500) -> str:
    """Render the PR title + (truncated) description for the batch context."""
    title = (pr_info.get("title") or "").strip()
    body = (pr_info.get("body") or "").strip()
    # Drop the skip marker so it doesn't read as instruction noise.
    body = body.replace("[skip-review]", "").strip()
    if not title and not body:
        return ""
    lines = ["PR INTENT (author's stated goal — use for context, NOT as a checklist):"]
    if title:
        lines.append(f"  Title: {title}")
    if body:
        if len(body) > max_body_chars:
            body = body[:max_body_chars] + " …[description truncated]"
        indented = "\n".join(f"    {ln}" for ln in body.splitlines())
        lines.append("  Description:")
        lines.append(indented)
    return "\n".join(lines)


def get_pr_files(token: str, api_url: str, repo: str, pr_number: int) -> list:
    """Fetch all changed files for a PR (handles pagination)."""
    headers = {
        "Authorization": f"token {token}",
        "Accept": "application/vnd.github.v3+json",
    }
    url = f"{api_url}/repos/{repo}/pulls/{pr_number}/files"
    files = []
    page = 1
    while True:
        try:
            resp = requests.get(
                url, headers=headers,
                params={"per_page": 100, "page": page},
                timeout=30,
            )
            if resp.status_code != 200:
                print(f"[scope] Warning: GitHub API {resp.status_code}")
                break
            data = resp.json()
            if not data:
                break
            files.extend(data)
            if len(data) < 100:
                break
            page += 1
        except Exception as e:
            print(f"[scope] Warning: page {page} fetch failed: {e}")
            break
    return files


# ─────────────────────────────────────────────────────────────────────────────
# Main
# ─────────────────────────────────────────────────────────────────────────────

def main():
    token = os.environ.get("GITHUB_TOKEN", "")
    api_url = os.environ.get("GITHUB_API_URL", "https://api.github.com")
    repo = os.environ.get("REPO_FULL_NAME", "")
    pr_number_str = os.environ.get("PR_NUMBER", "")

    if not all([token, repo, pr_number_str]):
        print("[scope] ERROR: Missing GITHUB_TOKEN, REPO_FULL_NAME, or PR_NUMBER")
        sys.exit(1)

    pr_number = int(pr_number_str)
    print(f"[scope] Analyzing PR #{pr_number} in {repo}")

    # ── Fetch PR files ────────────────────────────────────────────────────────
    pr_files = get_pr_files(token, api_url, repo, pr_number)
    print(f"[scope] {len(pr_files)} files changed")

    # ── Fetch PR title/description (author's intent) ──────────────────────────
    pr_info = get_pr_info(token, api_url, repo, pr_number)
    pr_intent_text = format_pr_intent(pr_info)
    if pr_intent_text:
        print(f"[scope] PR title: {pr_info.get('title', '')[:80]}")

    # ── Categorize and filter files ───────────────────────────────────────────
    reviewable = []
    skipped_count = 0
    total_additions = 0
    total_deletions = 0

    for f in pr_files:
        path = f.get("filename", "")
        status = f.get("status", "")
        additions = f.get("additions", 0)
        deletions = f.get("deletions", 0)
        patch = f.get("patch", "")

        if status == "removed":
            continue
        if should_skip(path):
            skipped_count += 1
            continue

        cat = categorize(path)
        if cat == "other":
            continue

        # Compress the patch to stay within per-file token budget
        compressed = compress_patch(patch, MAX_LINES_PER_FILE)

        reviewable.append({
            "path": path,
            "category": cat,
            "priority": FILE_TYPES[cat]["priority"],
            "checklist": FILE_TYPES[cat]["checklist"],
            "additions": additions,
            "deletions": deletions,
            "patch": compressed,
            "patch_chars": len(compressed),
        })

        total_additions += additions
        total_deletions += deletions

    print(f"[scope] {len(reviewable)} reviewable files, {skipped_count} skipped")

    def _unit_chars(unit):
        return sum(i["patch_chars"] + 500 for i in unit)  # 500 = per-file header overhead

    units: dict = {}
    for item in reviewable:
        units.setdefault(unit_key(item), []).append(item)

    # Bucket units by directory.
    dir_groups: dict = {}
    for unit in units.values():
        d = os.path.dirname(unit[0]["path"])
        dir_groups.setdefault(d, []).append(unit)

    # Sort directories: highest priority first, then largest total change first.
    ordered_dirs = sorted(
        dir_groups.values(),
        key=lambda g: (min(i["priority"] for u in g for i in u),
                       -sum(i["additions"] + i["deletions"] for u in g for i in u)),
    )

    batches = []
    current_batch = []
    current_chars = 0

    def _flush():
        nonlocal current_batch, current_chars
        if current_batch:
            batches.append(current_batch)
            current_batch = []
            current_chars = 0

    for dir_units in ordered_dirs:
        dir_chars = sum(_unit_chars(u) for u in dir_units)
        if dir_chars <= TARGET_CHARS_PER_BATCH:
            # Keep the whole directory together. Flush first if it won't fit.
            if current_chars + dir_chars > TARGET_CHARS_PER_BATCH:
                _flush()
            for unit in dir_units:
                current_batch.extend(unit)
            current_chars += dir_chars
        else:
            # Directory exceeds the budget — pack unit-by-unit within it.
            for unit in dir_units:
                uc = _unit_chars(unit)
                if current_chars + uc > TARGET_CHARS_PER_BATCH and current_batch:
                    _flush()
                current_batch.extend(unit)
                current_chars += uc

    _flush()

    if not batches:
        batches = [[]]  # empty PR — still run review

    dropped_files = []
    was_capped = False
    if len(batches) > MAX_BATCHES:
        was_capped = True
        dropped_batches = batches[MAX_BATCHES:]
        batches = batches[:MAX_BATCHES]
        dropped_files = [item["path"] for b in dropped_batches for item in b]
        print(f"[scope] ⚠️  Capped at {MAX_BATCHES} batches — "
              f"{len(dropped_files)} lower-priority file(s) will NOT be reviewed")

    is_large_pr = len(batches) > 1
    print(f"[scope] Split into {len(batches)} batch(es) "
          f"({'large PR' if is_large_pr else 'normal PR'})")

    # ── Build batch context files ─────────────────────────────────────────────
    batch_files = []

    for batch_idx, batch in enumerate(batches):
        batch_num = batch_idx + 1
        total_batches = len(batches)

        # Header
        header_lines = [
            "=" * 70,
            f"QTAC PR REVIEW CONTEXT — Batch {batch_num}/{total_batches}",
            f"PR #{pr_number} in {repo}",
            f"Total PR: +{total_additions}/-{total_deletions} lines across "
            f"{len(reviewable)} files ({skipped_count} skipped)",
            "=" * 70,
            "",
        ]

        if pr_intent_text:
            header_lines += ["─" * 70, pr_intent_text, "─" * 70, ""]

        if was_capped:
            header_lines += [
                f"🚫 REVIEW CAPPED — This PR is too large to review fully. Only the "
                f"{MAX_BATCHES} highest-priority batch(es) are reviewed.",
                f"   {len(dropped_files)} lower-priority file(s) were NOT reviewed. "
                f"State this clearly in your summary comment.",
                "",
            ]

        if is_large_pr:
            header_lines += [
                f"⚠️  LARGE PR — This is batch {batch_num} of {total_batches}.",
                f"   Review ONLY the {len(batch)} file(s) listed in this batch.",
                f"   Other files are reviewed in separate batches.",
                f"   Be CONCISE: flag Critical and Warning issues only.",
                f"   Skip Info-level suggestions to keep the review manageable.",
                "",
            ]

        by_category: dict = {}
        for item in batch:
            cat = item["category"]
            if cat not in by_category:
                by_category[cat] = []
            by_category[cat].append(item)

        file_sections = []
        for cat, items in by_category.items():
            checklist = FILE_TYPES[cat]["checklist"]
            section_lines = [
                "─" * 70,
                f"FILE TYPE: {cat.upper().replace('_', ' ')}",
                "Review checklist for this file type:",
            ]
            for check in checklist:
                section_lines.append(f"  ✓ {check}")
            section_lines.append("")

            for item in items:
                section_lines.append(
                    f"FILE: {item['path']}  "
                    f"(+{item['additions']}/-{item['deletions']} lines)"
                )
                if item["patch"]:
                    section_lines.append("DIFF:")
                    section_lines.append("```diff")
                    section_lines.append(item["patch"])
                    section_lines.append("```")
                else:
                    section_lines.append("(no diff available — file may be binary or new)")
                section_lines.append("")

            file_sections.append("\n".join(section_lines))

        # Footer instructions
        footer_lines = [
            "=" * 70,
            "REVIEW INSTRUCTIONS FOR THIS BATCH:",
            "1. Read SKILLS.md for established patterns NOT to flag.",
            "2. Apply ONLY the checklist items listed for each file type above.",
            "3. Review ONLY the diff lines shown (+ added, - removed).",
            "4. Do NOT flag patterns in the 'Established Patterns' or "
            "'Learned Suppressions' sections of SKILLS.md.",
        ]
        if is_large_pr:
            footer_lines.append(
                f"6. LARGE PR: Be concise. Only Critical/Warning issues. "
                f"Post batch {batch_num}/{total_batches} label in your summary."
            )
        footer_lines.append("=" * 70)

        # Assemble full batch context
        batch_content = (
            "\n".join(header_lines)
            + "\n"
            + "\n".join(file_sections)
            + "\n"
            + "\n".join(footer_lines)
        )

        # Estimate tokens for this batch
        batch_tokens = estimate_tokens(batch_content)
        print(f"[scope] Batch {batch_num}: {len(batch)} files, "
              f"~{batch_tokens:,} tokens ({len(batch_content):,} chars)")

        # Write batch file
        batch_filename = f"pr_batch_{batch_num}.txt"
        with open(batch_filename, "w", encoding="utf-8") as fh:
            fh.write(batch_content)
        batch_files.append(batch_filename)

    # ── Write machine-readable manifest ──────────────────────────────────────
    manifest = {
        "pr_number": pr_number,
        "repo": repo,
        "timestamp": datetime.utcnow().isoformat() + "Z",
        "is_large_pr": is_large_pr,
        "total_batches": len(batches),
        "was_capped": was_capped,
        "dropped_files": dropped_files,
        "dropped_file_count": len(dropped_files),
        "total_files_changed": len(pr_files),
        "reviewable_files": len(reviewable),
        "skipped_files": skipped_count,
        "total_additions": total_additions,
        "total_deletions": total_deletions,
        "batch_files": batch_files,
        "batches": [
            {
                "batch_num": i + 1,
                "file_count": len(b),
                "files": [x["path"] for x in b],
                "categories": list({x["category"] for x in b}),
            }
            for i, b in enumerate(batches)
        ],
    }

    with open("pr_batches.json", "w", encoding="utf-8") as fh:
        json.dump(manifest, fh, indent=2)

    # ── Write one-line meta for workflow ──────────────────────────────────────
    meta = (
        f"batches={len(batches)} "
        f"files={len(reviewable)} "
        f"large={'true' if is_large_pr else 'false'} "
        f"tokens_est={estimate_tokens(open(batch_files[0]).read()):,}"
    )
    with open("pr_scope_meta.txt", "w", encoding="utf-8") as fh:
        fh.write(meta)

    print(f"[scope] Done. {meta}")
    print(f"[scope] Batch files: {', '.join(batch_files)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())