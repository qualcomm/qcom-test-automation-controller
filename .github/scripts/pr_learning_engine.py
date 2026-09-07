#!/usr/bin/env python3
# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause
"""
QTAC PR Review Learning Engine — SKILLS.md updater
"""

import os
import re
import sys
from datetime import datetime, timezone

try:
    import requests
except ImportError:
    print("[learn] ERROR: requests not installed. Run: pip install requests")
    sys.exit(1)

SKILLS_FILE = os.environ.get("SKILLS_FILE", "SKILLS.md")

SUPPRESS_START = "<!-- LEARNED-SUPPRESSIONS:START -->"
SUPPRESS_END = "<!-- LEARNED-SUPPRESSIONS:END -->"
AUTO_NOTE = "<!-- Entries are added automatically below this line. Do not hand-edit. -->"

# Cap the block so it can't grow without bound (keeps most recent).
MAX_SUPPRESSIONS = 200

DISMISS_KEYWORDS = [
    "false positive", "nolint", "wontfix", "by design", "intentional",
    "known issue", "pre-existing", "not applicable", "n/a", "ignore",
    "pre existing", "established pattern", "skip", "won't fix",
]

BOT_HEADER_MARKERS = ("QTAC Code Review", "QTAC PR REVIEW")


# ─────────────────────────────────────────────────────────────────────────────
# Comment classification
# ─────────────────────────────────────────────────────────────────────────────
def is_dismiss(text: str) -> bool:
    t = (text or "").lower()
    return any(kw in t for kw in DISMISS_KEYWORDS)


def is_bot_comment(comment: dict) -> bool:
    """True if this comment was posted by the review bot (not a human).

    A human "Quote reply" prepends the bot's text, so classifying by body
    content alone would misfile a human dismissal as a bot comment and drop it.
    Trust the API author first; only fall back to the header when it is on the
    first NON-QUOTED line.
    """
    user = comment.get("user") or {}
    login = (user.get("login") or "").lower()
    if user.get("type") == "Bot" or "github-actions" in login or "claude" in login:
        return True
    body = comment.get("body", "") or ""
    for line in body.splitlines():
        s = line.strip()
        if not s:
            continue
        if s.startswith(">"):          # quoted → human reply
            return False
        if s.startswith("## 🤖"):
            return True
        return any(m in s for m in BOT_HEADER_MARKERS)
    return False


def extract_rule_id(text: str) -> str:
    m = re.search(r"\b([A-Z]{2,4}-\d{2,3})\b", text or "")
    return m.group(1) if m else "UNKNOWN"


def extract_pattern(text: str, max_len: int = 100) -> str:
    """Short one-line description of the bot finding, for the suppression entry.

    Skips the bot's boilerplate (header line, the inline "AI review (verify):"
    prefix) so the entry captures the actual finding, not the banner.
    """
    content = []
    for line in (text or "").splitlines():
        s = line.strip()
        if not s or s.startswith("#"):
            continue
        if any(m in s for m in BOT_HEADER_MARKERS):
            continue
        content.append(s)
    joined = " ".join(content) if content else (text or "")
    joined = re.sub(r"🤖\s*AI review \(verify\):\s*", "", joined)
    clean = re.sub(r"[`*_#\[\]()]", "", joined)
    clean = re.sub(r"\s+", " ", clean).strip()
    first = re.split(r"[.!?]", clean)[0].strip() if clean else ""
    return first[:max_len]


# ─────────────────────────────────────────────────────────────────────────────
# GitHub API
# ─────────────────────────────────────────────────────────────────────────────
def get_pr_comments(token: str, api_url: str, repo: str, pr_number: int) -> list:
    """All PR comments — inline review comments + issue comments (paginated)."""
    headers = {"Authorization": f"token {token}",
               "Accept": "application/vnd.github.v3+json"}
    out = []
    for endpoint in (f"{api_url}/repos/{repo}/pulls/{pr_number}/comments",
                     f"{api_url}/repos/{repo}/issues/{pr_number}/comments"):
        page = 1
        while True:
            try:
                resp = requests.get(endpoint, headers=headers,
                                    params={"per_page": 100, "page": page}, timeout=30)
                if resp.status_code != 200:
                    break
                data = resp.json()
                if not data:
                    break
                out.extend(data)
                if len(data) < 100:
                    break
                page += 1
            except Exception as e:
                print(f"[learn] Warning: comment fetch failed: {e}")
                break
    return out


# ─────────────────────────────────────────────────────────────────────────────
# SKILLS.md block update  (pure — unit-testable)
# ─────────────────────────────────────────────────────────────────────────────
def _key(rule: str, pattern: str) -> tuple:
    return (rule.strip().lower(), pattern.strip().lower())


def _entry_line(rule: str, pattern: str, pr: int, date: str) -> str:
    return f"- [{rule}] {pattern} — dismissed in PR #{pr} ({date})"


def _parse_entry(line: str):
    """Return (rule, pattern) from an entry line, or None."""
    line = line.strip()
    if not line.startswith("- ["):
        return None
    try:
        rule = line[line.index("[") + 1:line.index("]")]
        after = line[line.index("]") + 1:]
        pattern = after.split(" — dismissed")[0].strip()
    except ValueError:
        return None
    return rule, pattern


def update_suppressions_block(text: str, new_entries: list):
    """Merge `new_entries` into the managed block in `text`.

    new_entries: list of dicts {rule, pattern, pr, date}.
    Returns (new_text, added_count). De-dupes by (rule, pattern); caps size.
    """
    if SUPPRESS_START not in text or SUPPRESS_END not in text:
        # Section missing — append a fresh one so the update can proceed.
        text = (text.rstrip() + "\n\n## 🧠 Learned Suppressions — DO NOT FLAG These\n\n"
                f"{SUPPRESS_START}\n{AUTO_NOTE}\n_(none yet)_\n{SUPPRESS_END}\n")

    pre, rest = text.split(SUPPRESS_START, 1)
    _block, post = rest.split(SUPPRESS_END, 1)

    entries, seen = [], set()
    for line in _block.splitlines():
        parsed = _parse_entry(line)
        if parsed:
            k = _key(*parsed)
            if k not in seen:
                seen.add(k)
                entries.append(line.strip())

    added = 0
    for e in new_entries:
        k = _key(e["rule"], e["pattern"])
        if k in seen:
            continue
        seen.add(k)
        entries.append(_entry_line(e["rule"], e["pattern"], e["pr"], e["date"]))
        added += 1

    if len(entries) > MAX_SUPPRESSIONS:
        entries = entries[-MAX_SUPPRESSIONS:]

    body = "\n".join(entries) if entries else "_(none yet)_"
    new_block = f"\n{AUTO_NOTE}\n{body}\n"
    return pre + SUPPRESS_START + new_block + SUPPRESS_END + post, added


# ─────────────────────────────────────────────────────────────────────────────
# Main
# ─────────────────────────────────────────────────────────────────────────────
def collect_dismissed(comments: list) -> list:
    """Return new suppression entries for bot findings a human dismissed."""
    bots = [c for c in comments if is_bot_comment(c)]
    humans = [c for c in comments if not is_bot_comment(c)]
    print(f"[learn] {len(bots)} bot comment(s), {len(humans)} human comment(s)")

    date = datetime.now(timezone.utc).strftime("%Y-%m-%d")
    pr_number = int(os.environ.get("PR_NUMBER", "0"))
    entries = []
    for b in bots:
        body = b.get("body", "")
        rule = extract_rule_id(body)
        pattern = extract_pattern(body)
        if not pattern:
            continue
        replies = [c for c in humans if c.get("in_reply_to_id") == b.get("id")]
        # Fallback for non-threaded issue comments: match by pattern text / rule id.
        if not replies and not b.get("in_reply_to_id"):
            for hc in humans:
                hcb = hc.get("body", "")
                if (len(pattern) >= 20 and pattern[:30] in hcb) or \
                   (rule != "UNKNOWN" and rule in hcb):
                    replies.append(hc)
        if any(is_dismiss(r.get("body", "")) for r in replies):
            print(f"[learn] Dismissed → suppress: [{rule}] {pattern[:50]}")
            entries.append({"rule": rule, "pattern": pattern,
                            "pr": pr_number, "date": date})
    return entries


def main():
    token = os.environ.get("GITHUB_TOKEN", "")
    api_url = os.environ.get("GITHUB_API_URL", "https://api.github.com")
    repo = os.environ.get("REPO_FULL_NAME", "")
    pr_number_str = os.environ.get("PR_NUMBER", "")

    if not all([token, repo, pr_number_str]):
        print("[learn] ERROR: Missing GITHUB_TOKEN, REPO_FULL_NAME, or PR_NUMBER")
        sys.exit(1)

    pr_number = int(pr_number_str)
    print(f"[learn] Processing closed PR #{pr_number}")

    comments = get_pr_comments(token, api_url, repo, pr_number)
    print(f"[learn] Fetched {len(comments)} comment(s)")

    new_entries = collect_dismissed(comments)
    if not new_entries:
        print("[learn] No dismissed findings to learn from — SKILLS.md unchanged")
        return 0

    if not os.path.exists(SKILLS_FILE):
        print(f"[learn] ERROR: {SKILLS_FILE} not found")
        return 1

    with open(SKILLS_FILE, "r", encoding="utf-8") as f:
        text = f.read()

    new_text, added = update_suppressions_block(text, new_entries)
    if added:
        with open(SKILLS_FILE, "w", encoding="utf-8") as f:
            f.write(new_text)
        print(f"[learn] Added {added} suppression(s) to {SKILLS_FILE}")
    else:
        print("[learn] All dismissed findings already present — SKILLS.md unchanged")
    return 0


if __name__ == "__main__":
    sys.exit(main())