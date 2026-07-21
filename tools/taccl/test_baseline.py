#!/usr/bin/env python3
"""
taccl baseline test suite.

Runs the same set of commands against two builds of taccl.exe (one backed by
the Qt TACDev, one backed by the non-Qt TACDev) and compares:
  - exit codes
  - stdout (text or JSON)
  - stderr

Usage
-----
    python test_baseline.py --device COM3 --exe-a path\to\taccl_qt.exe --exe-b path\to\taccl_noqt.exe

Minimal run (single exe, capture only):
    python test_baseline.py --device COM3 --exe-a path\to\taccl.exe

The script writes a machine-readable JSON results file (taccl_baseline.json)
next to itself so diffs can be stored in git or compared offline.
"""

import argparse
import ctypes
import json
import os
import subprocess
import sys
import textwrap
import time
from dataclasses import dataclass, field, asdict
from pathlib import Path
from typing import Optional

# Suppress Windows crash dialogs (abort/retry/ignore) for child processes.
# SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX
if sys.platform == "win32":
    ctypes.windll.kernel32.SetErrorMode(0x0001 | 0x0002 | 0x8000)

# ---------------------------------------------------------------------------
# Data structures
# ---------------------------------------------------------------------------

@dataclass
class RunResult:
    exit_code: int
    stdout: str
    stderr: str
    elapsed_ms: float


@dataclass
class CaseResult:
    name: str
    args: list
    result_a: Optional[RunResult] = None
    result_b: Optional[RunResult] = None
    # Filled in during comparison
    exit_match: Optional[bool] = None
    stdout_match: Optional[bool] = None
    stderr_match: Optional[bool] = None
    passed: Optional[bool] = None
    notes: list = field(default_factory=list)


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

def run(exe: str, args: list, timeout: int = 10) -> RunResult:
    cmd = [exe] + [str(a) for a in args]
    # Run from the exe's directory so Windows DLL search finds sibling DLLs.
    cwd = str(Path(exe).resolve().parent)
    t0 = time.monotonic()
    try:
        proc = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            timeout=timeout,
            cwd=cwd,
        )
        elapsed = (time.monotonic() - t0) * 1000
        return RunResult(
            exit_code=proc.returncode,
            stdout=proc.stdout.strip(),
            stderr=proc.stderr.strip(),
            elapsed_ms=round(elapsed, 1),
        )
    except subprocess.TimeoutExpired:
        return RunResult(exit_code=-1, stdout="", stderr="TIMEOUT", elapsed_ms=timeout * 1000)
    except FileNotFoundError:
        return RunResult(exit_code=-1, stdout="", stderr=f"EXE NOT FOUND: {exe}", elapsed_ms=0)


def is_valid_json(s: str) -> bool:
    try:
        json.loads(s)
        return True
    except (json.JSONDecodeError, ValueError):
        return False


def compare(case: CaseResult) -> None:
    """Fill comparison fields. Called only when both A and B results exist."""
    a, b = case.result_a, case.result_b
    case.exit_match   = (a.exit_code == b.exit_code)
    case.stdout_match = (a.stdout    == b.stdout)
    case.stderr_match = (a.stderr    == b.stderr)
    case.passed       = case.exit_match and case.stdout_match

    if not case.exit_match:
        case.notes.append(
            f"exit: A={a.exit_code} B={b.exit_code}"
        )
    if not case.stdout_match:
        case.notes.append(
            f"stdout: A={repr(a.stdout)} B={repr(b.stdout)}"
        )
    if not case.stderr_match:
        case.notes.append(
            f"stderr: A={repr(a.stderr)} B={repr(b.stderr)}"
        )


# ---------------------------------------------------------------------------
# Test case definitions
# ---------------------------------------------------------------------------

def build_cases(device: str) -> list:
    """
    Returns a list of (name, args, expected_exit) tuples.
    expected_exit=None means "don't assert, just record".

    Sections:
      1. No-device (tool-level)   - don't need a real port
      2. Device enumeration       - needs the device present
      3. State queries            - read current state of every control
      4. Round-trip set/get       - set off, read, set on, read  (non-destructive)
      5. Set-only commands        - external-power, pin
      6. Error paths              - bad port, bad state value, missing args
      7. Output format            - --json and --quiet variants
    """
    D = device  # shorthand

    cases = []

    def add(name, args, expect=None):
        cases.append((name, args, expect))

    # -----------------------------------------------------------------------
    # 1. Tool-level (no device needed)
    # -----------------------------------------------------------------------
    add("version",              ["--version"],                               0)
    add("help",                 ["--help"],                                  0)
    add("help_battery",         ["battery", "--help"],                       0)
    add("help_usb0",            ["usb0", "--help"],                          0)
    add("help_pin",             ["pin", "--help"],                           0)
    add("no_subcommand",        [],                                          None)  # CLI11 returns non-zero

    # -----------------------------------------------------------------------
    # 2. Device enumeration / resolution errors
    # -----------------------------------------------------------------------
    add("bad_port_text",        ["battery", "--device=COM99"],               2)
    add("bad_port_json",        ["battery", "--device=COM99", "--json"],     2)
    add("missing_device",       ["battery"],                                 3)
    add("bad_state_value",      ["battery", f"--device={D}", "--state=bogus"], 105)
    add("pin_missing_pin",      ["pin", f"--device={D}", "--state=on"],      106)
    add("pin_missing_state",    ["pin", f"--device={D}", "--pin=7"],         106)
    add("ext_power_no_state",   ["external-power", f"--device={D}"],         106)

    # -----------------------------------------------------------------------
    # 3. State queries (read current state of every control)
    # -----------------------------------------------------------------------
    for ctrl in [
        "battery", "usb0",
        "usb1",               # may not be supported on all devices (exit 4 = TacCommandFail)
        "power-key",
        "volume-up", "volume-down",
        "disconnect-uim1", "disconnect-uim2",
        "disconnect-sdcard", "disconnect-headset",
        "primary-edl",
        "secondary-edl",      # may not be supported on all devices
        "force-pshold",
        "secondary-pm-resin", # may not be supported on all devices
        "eud",
    ]:
        expect = None if ctrl in ("usb1", "secondary-edl", "secondary-pm-resin") else 0
        add(f"query_{ctrl.replace('-', '_')}",
            [ctrl, f"--device={D}"],
            expect)
        add(f"query_{ctrl.replace('-', '_')}_json",
            [ctrl, f"--device={D}", "--json"],
            expect)
        add(f"query_{ctrl.replace('-', '_')}_quiet",
            [ctrl, f"--device={D}", "--quiet"],
            expect)

    # -----------------------------------------------------------------------
    # 4. Round-trip set/get  (battery: off → read → on → read)
    #    We pick battery because it's the safest to toggle briefly.
    #    The suite restores on at the end.
    # -----------------------------------------------------------------------
    add("battery_set_off",      ["battery", f"--device={D}", "--state=off"], 0)
    add("battery_read_after_off", ["battery", f"--device={D}"],              0)
    add("battery_set_on",       ["battery", f"--device={D}", "--state=on"],  0)
    add("battery_read_after_on",  ["battery", f"--device={D}"],              0)

    # USB0 round-trip
    add("usb0_set_off",         ["usb0", f"--device={D}", "--state=off"],    0)
    add("usb0_read_after_off",  ["usb0", f"--device={D}"],                   0)
    add("usb0_set_on",          ["usb0", f"--device={D}", "--state=on"],     0)
    add("usb0_read_after_on",   ["usb0", f"--device={D}"],                   0)

    # -----------------------------------------------------------------------
    # 5. Set-only commands
    # -----------------------------------------------------------------------
    add("external_power_on",    ["external-power", f"--device={D}", "--state=on"],  None)
    add("external_power_off",   ["external-power", f"--device={D}", "--state=off"], None)
    add("pin7_on",              ["pin", f"--device={D}", "--pin=7", "--state=on"],  0)
    add("pin7_off",             ["pin", f"--device={D}", "--pin=7", "--state=off"], 0)

    # -----------------------------------------------------------------------
    # 6. JSON output shape validation (structure, not just equality)
    # -----------------------------------------------------------------------
    add("battery_json_shape",   ["battery", f"--device={D}", "--json"],      0)
    add("bad_port_json_shape",  ["battery", "--device=COM99", "--json"],      2)

    # -----------------------------------------------------------------------
    # 7. TACCL_DEVICE env-var path (pass device via env, no --device flag)
    #    These are tagged specially so the runner sets the env var.
    # -----------------------------------------------------------------------
    add("env_device_query",     ["__ENV__", "battery"],                      0)

    # -----------------------------------------------------------------------
    # 8. Quick commands
    # -----------------------------------------------------------------------
    add("help_quick_commands",        ["quick-commands", "--help"],          0)
    add("quick_commands",             ["quick-commands", f"--device={D}"],   0)
    add("quick_commands_json",        ["quick-commands", f"--device={D}", "--json"], 0)
    add("quick_commands_quiet",       ["quick-commands", f"--device={D}", "--quiet"], 0)
    add("quick_commands_bad_port",    ["quick-commands", "--device=COM99"],   2)
    add("quick_commands_bad_port_json", ["quick-commands", "--device=COM99", "--json"], 2)
    add("quick_commands_missing_device", ["quick-commands"],                 3)

    return cases


# ---------------------------------------------------------------------------
# Runner
# ---------------------------------------------------------------------------

def run_cases(exe: str, cases: list, device: str, delay: float = 0.5) -> list:
    results = []
    for name, args, _expect in cases:
        env = None
        actual_args = args

        if args and args[0] == "__ENV__":
            # Pass device via TACCL_DEVICE instead of --device flag
            env = {**os.environ, "TACCL_DEVICE": device}
            actual_args = args[1:]

        if env:
            t0 = time.monotonic()
            cwd = str(Path(exe).resolve().parent)
            try:
                proc = subprocess.run(
                    [exe] + actual_args,
                    capture_output=True,
                    text=True,
                    timeout=10,
                    env=env,
                    cwd=cwd,
                )
                elapsed = (time.monotonic() - t0) * 1000
                r = RunResult(
                    exit_code=proc.returncode,
                    stdout=proc.stdout.strip(),
                    stderr=proc.stderr.strip(),
                    elapsed_ms=round(elapsed, 1),
                )
            except subprocess.TimeoutExpired:
                r = RunResult(-1, "", "TIMEOUT", 10000)
            except FileNotFoundError:
                r = RunResult(-1, "", f"EXE NOT FOUND: {exe}", 0)
        else:
            r = run(exe, actual_args)

        results.append((name, actual_args, _expect, r))

        # Throttle cases that touch the real device to avoid serial port
        # contention between successive taccl processes.
        touches_device = any(device in a for a in actual_args) or env is not None
        if touches_device and delay > 0:
            time.sleep(delay)

    return results


# ---------------------------------------------------------------------------
# Reporting
# ---------------------------------------------------------------------------

def print_run_summary(label: str, results: list) -> None:
    print(f"\n{'='*70}")
    print(f"  {label}")
    print(f"{'='*70}")
    for name, args, expect, r in results:
        ok = "PASS" if (expect is None or r.exit_code == expect) else "FAIL"
        marker = "OK" if ok == "PASS" else "!!"
        print(f"  {marker} [{r.exit_code:>2}] {name:<40}  {' '.join(args)}")
        if ok == "FAIL":
            print(f"       expected exit={expect}  stdout={repr(r.stdout[:80])}")
        if r.stderr:
            print(f"       stderr: {repr(r.stderr[:80])}")


def print_diff_summary(cases: list) -> None:
    print(f"\n{'='*70}")
    print("  DIFF SUMMARY (A vs B)")
    print(f"{'='*70}")
    total = len(cases)
    matches = sum(1 for c in cases if c.passed)
    mismatches = [c for c in cases if c.passed is False]

    print(f"  Total cases : {total}")
    print(f"  Match       : {matches}")
    print(f"  Mismatch    : {len(mismatches)}")

    if mismatches:
        print()
        for c in mismatches:
            print(f"  !! {c.name}")
            for note in c.notes:
                print(f"      {note}")
    else:
        print("\n  All cases match between A and B.")


def validate_json_cases(results: list) -> None:
    """Extra check: cases ending in _json must produce valid JSON on stdout."""
    print(f"\n{'='*70}")
    print("  JSON OUTPUT VALIDATION")
    print(f"{'='*70}")
    for name, _args, _expect, r in results:
        if name.endswith("_json") or name.endswith("_shape"):
            if r.exit_code == 0 or name.endswith("_shape"):
                valid = is_valid_json(r.stdout) or is_valid_json(r.stderr)
                marker = "OK" if valid else "!!"
                # For error cases the JSON goes to stderr
                combined = r.stdout or r.stderr
                print(f"  {marker} {name:<45}  {repr(combined[:60])}")


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(
        description="taccl baseline test suite",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=textwrap.dedent("""
        Examples:
          # Single exe (capture only, no diff):
          python test_baseline.py --device COM3 --exe-a build\\Debug\\taccl.exe

          # Compare Qt vs non-Qt builds:
          python test_baseline.py --device COM3 \\
              --exe-a qt\\taccl.exe \\
              --exe-b noqt\\taccl.exe
        """),
    )
    parser.add_argument("--device", required=True,
                        help="COM port of the connected TAC device (e.g. COM3)")
    parser.add_argument("--exe-a", required=True, dest="exe_a",
                        help="Path to taccl.exe (build A, e.g. Qt-backed)")
    parser.add_argument("--exe-b", dest="exe_b",
                        help="Path to taccl.exe (build B, e.g. non-Qt). "
                             "If omitted only build A is run.")
    parser.add_argument("--output", default=None,
                        help="Path for JSON results file "
                             "(default: taccl_baseline_<label>.json next to this script)")
    parser.add_argument("--delay", type=float, default=0.5, metavar="SECS",
                        help="Seconds to wait between device-touching cases "
                             "(default: 0.5). Prevents serial port contention.")
    args = parser.parse_args()

    cases = build_cases(args.device)

    # -----------------------------------------------------------------------
    # Run A
    # -----------------------------------------------------------------------
    print(f"\nRunning build A: {args.exe_a}")
    results_a = run_cases(args.exe_a, cases, args.device, args.delay)
    print_run_summary(f"Build A — {args.exe_a}", results_a)
    validate_json_cases(results_a)

    # -----------------------------------------------------------------------
    # Run B (optional)
    # -----------------------------------------------------------------------
    results_b = None
    if args.exe_b:
        print(f"\nRunning build B: {args.exe_b}")
        results_b = run_cases(args.exe_b, cases, args.device, args.delay)
        print_run_summary(f"Build B — {args.exe_b}", results_b)
        validate_json_cases(results_b)

    # -----------------------------------------------------------------------
    # Diff
    # -----------------------------------------------------------------------
    case_objects = []
    if results_b:
        for (name, args_used, expect, ra), (_, _, _, rb) in zip(results_a, results_b):
            c = CaseResult(
                name=name,
                args=args_used,
                result_a=ra,
                result_b=rb,
            )
            compare(c)
            case_objects.append(c)
        print_diff_summary(case_objects)

    # -----------------------------------------------------------------------
    # Save JSON
    # -----------------------------------------------------------------------
    out_path = args.output
    if out_path is None:
        label = "a_only" if not args.exe_b else "a_vs_b"
        out_path = Path(__file__).parent / f"taccl_baseline_{label}.json"

    report = {
        "exe_a": args.exe_a,
        "exe_b": args.exe_b,
        "device": args.device,
        "results_a": [
            {"name": n, "args": a, "expected_exit": e, **asdict(r)}
            for n, a, e, r in results_a
        ],
    }
    if results_b:
        report["results_b"] = [
            {"name": n, "args": a, "expected_exit": e, **asdict(r)}
            for n, a, e, r in results_b
        ]
        report["diff"] = [asdict(c) for c in case_objects]

    with open(out_path, "w") as f:
        json.dump(report, f, indent=2)
    print(f"\nResults saved to: {out_path}")

    # Exit non-zero if any case_object failed the diff
    if case_objects and any(not c.passed for c in case_objects):
        sys.exit(1)


if __name__ == "__main__":
    main()
