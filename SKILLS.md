# QTAC PR Review Agent — Skills & Codebase Context

> This file is read automatically by the Claude PR Review Agent.
> It provides codebase context, established patterns, and review guidelines.

---

## 🏗️ Project Overview

**QTAC** (Qualcomm Test Automation Controller) is a cross-platform C++/Qt application and service that controls hardware test automation boards (PSOC, FTDI, PIC32CX) via USB. It exposes a Thrift-based IPC service (`TacService`) and provides C++, C#, and Python client interfaces.

**Platforms:** Windows (x64, ARM64), Linux (x86_64, ARM64)  
**Build system:** CMake + Qt (aqtinstall)  
**Qt version:** 6.9.x / 6.10.x  
**Language standard:** C++17  
**Key dependencies:** Qt6, FTDI libftd2xx, Apache Thrift, .NET Framework 4.8 (C# interop)

---

## 📁 Repository Structure

```
src/
  libraries/
    qcommon-console/     # Core device library (AlpacaDevice, TACCommand, FTDIDevice, PSOCDevice, etc.)
    qcommon/             # Shared utilities (AboutDialog, etc.)
  applications/
    tacservice/          # Apache Thrift service (ThriftServer, TacServiceHandler)
      thrift/            # .thrift IDL files (TacService.thrift, Common.thrift)
      python/            # Python Thrift client (TacServiceClient.py, test_tac.py)
    device-catalog/      # Device catalog application
interfaces/
  C++/TACDev/            # C++ client library
  C#/TACDevInterop/      # C# interop (.csproj)
configurations/          # .tcnf device configuration files
packaging/
  windows/               # Windows installer scripts (build-installer.ps1, install.ps1)
  linux/                 # Linux packaging scripts
examples/
  C++/TACClient/         # C++ usage example
  MCP/sse/               # MCP SSE example
third-party/             # FTDI driver archives (not reviewed)
udev-rules/              # Linux udev rules
```

---

## 🔧 Core Architecture

### Device Abstraction Layer
- `_AlpacaDevice` — base class for all hardware devices (PSOC, FTDI, PIC32CX)
- `AlpacaDevice` — `QSharedPointer<_AlpacaDevice>` typedef (smart pointer pattern)
- `AlpacaDevices` — `QList<AlpacaDevice>` typedef
- Devices are managed via a static registry (`_AlpacaDevice::_alpacaDevices`)
- Thread safety via `QMutexLocker lock(&_mutex)`

### Drive Thread Pattern
- Each open device has a `_driveThread` (derived from `QThread`)
- Drive threads handle USB communication asynchronously
- `_driveThread->deleteLater()` is the correct cleanup pattern
- `_driveThread != Q_NULLPTR` is the null check pattern used throughout

### Thrift Service
- `ThriftServer` — Apache Thrift server (runs in its own thread)
- `TacServiceHandler` — implements the Thrift service interface
- IDL files in `src/applications/tacservice/thrift/`
- Generated Thrift code is in `src/applications/tacservice/python/thrift/`

### Configuration System
- `.tcnf` files — XML-based platform configuration files
- `PlatformConfiguration` — parses and holds device configuration
- `devicelist.json` — generated from `.tcnf` files by `UpdateDeviceList`

---

## ✅ Established Patterns — DO NOT FLAG These

The following patterns are **intentional and established** in this codebase. Do **NOT** flag them as issues in new code that follows these patterns:

### 1. Boolean Comparison Style
```cpp
// ESTABLISHED — do not flag
if (active() == true) { ... }
if (result == false) { ... }
if (_driveThread != Q_NULLPTR) { ... }
if (active() == false) { ... }
```
This is the deliberate style in this codebase. Do not suggest `if (active())` or `if (!result)`.

### 2. Q_NULLPTR Usage
```cpp
// ESTABLISHED — do not flag
return AlpacaDevice(Q_NULLPTR);
_driveThread = Q_NULLPTR;
if (_platformConfiguration != Q_NULLPTR) { ... }
```
`Q_NULLPTR` is the established null pointer idiom here, not `nullptr`.

### 3. Tab Indentation in C++ Files
The C++ source files use **tab characters** for indentation. This is the established style. Do not flag tab indentation.

### 4. Non-const Getter Methods
Many existing getter methods are not marked `const`:
```cpp
// ESTABLISHED — do not flag existing non-const getters
bool active();
HashType hash();
QByteArray description();
QByteArray serialNumber();
```
Only flag non-const getters in **new** methods added by the PR.

### 5. TODO Comments Without Ticket References
There are pre-existing TODO comments without ticket numbers:
```cpp
// TODO: Throw on else: command not found but check if there are python issues
// todo, write errors to log
```
Do not flag these pre-existing TODOs. Only flag NEW TODO comments added in the PR that lack ticket references.

### 6. Commented-Out Code (Pre-existing)
Some files have pre-existing commented-out code (e.g., `portName()` in AlpacaDevice.cpp). Do not flag pre-existing commented-out code. Only flag NEW commented-out code added in the PR.

### 7. `result == false` Pattern in sendCommand
```cpp
// ESTABLISHED — do not flag
else if (result == false) {
    result = quickCommand(command);
}
```

### 8. `std::as_const` Usage (Mixed)
Some iteration loops use `std::as_const`, some don't. Both patterns exist in the codebase. Only flag missing `std::as_const` if the PR introduces a new loop that modifies a container during iteration.

### 9. Variable Shadowing in findAlpacaDevice Methods
The `findAlpacaDeviceBySerialNumber` and `findAlpacaDeviceByDescription` methods have local variables that shadow parameters. This is pre-existing. Do not flag it.

### 10. `QByteArray`/`QString` Pass-by-Value (Some Existing Methods)
Some existing methods pass `QByteArray` or `QString` by value. Do not flag pre-existing signatures. Only flag NEW method signatures that pass large types by value unnecessarily.

### 11. Magic Numbers in Switch Statements
```cpp
// ESTABLISHED — do not flag
case 3: result = "LP038"; break;
case 4: result = "LP030"; break;
case 10000: result = "FTDI";
```
Hardware version numbers in switch statements are acceptable.

### 12. `delete` on Drive Thread (Specific Pattern)
```cpp
// ESTABLISHED — this specific pattern is intentional
if (_driveThread != Q_NULLPTR)
    _driveThread->deleteLater();
_driveThread = Q_NULLPTR;
```
This is the correct cleanup pattern for drive threads.

---

## 🧠 Learned Suppressions — DO NOT FLAG These

These entries are **auto-maintained** by the learning workflow
(`.github/workflows/pr-review-learn.yml`): when a reviewer dismisses a finding as
a false positive on a closed PR, it is added here (via a bot PR you approve) so
future reviews do not flag it again. Treat everything between the markers below
exactly like the "Established Patterns" section — **do not flag these patterns**.

<!-- LEARNED-SUPPRESSIONS:START -->
<!-- Entries are added automatically below this line. Do not hand-edit. -->
_(none yet)_
<!-- LEARNED-SUPPRESSIONS:END -->

---

## 🔍 What to Focus On — NEW Code Only

Review ONLY the lines changed in the PR. Focus on these categories for **new code**:

### 🔴 Critical — Must Fix
1. **Memory leaks**: Raw `new` stored in raw pointer without smart pointer or `deleteLater()`
2. **Thread safety**: GUI operations (`show()`, `hide()`, `resize()`, `update()`) called from worker threads
3. **Mutex safety**: Direct `mutex.lock()` without `QMutexLocker` in new code
4. **Null dereference**: Accessing `_driveThread` members without null check
5. **Hardcoded secrets**: API keys, passwords, tokens in source code
6. **Empty catch blocks**: `catch (...) {}` that silently swallows exceptions
7. **Static local variables**: Thread-unsafe static locals in new member functions
8. **`using namespace` in headers**: Pollutes global namespace
9. **Potential bugs — integer overflow**: Arithmetic on `int` that could overflow (use `qint64`/`quint64`)
10. **Potential bugs — uninitialized variables**: Member variables not initialized in constructor
11. **Potential bugs — use-after-free**: Accessing a pointer after `delete`/`deleteLater()`
12. **Potential bugs — missing break in switch**: Unintentional fallthrough in `switch` cases
13. **Potential bugs — signed/unsigned mismatch**: Comparing `int` with `size_t`/`quint32` (undefined behavior)
14. **Potential bugs — unchecked return values**: Ignoring return value of functions that signal errors

### 🟡 Warning — Should Fix
1. **New `new` in loops**: Memory allocation inside loops without guaranteed cleanup
2. **Old-style Qt connect**: `SIGNAL()`/`SLOT()` macros in new code (use pointer-to-member)
3. **Missing `override`**: New virtual function overrides without `override` keyword in headers
4. **CMakeLists.txt errors**: Wrong target names, missing `find_package`, incorrect platform guards
5. **Thrift IDL changes**: Breaking changes to `.thrift` files (removing fields, changing types)
6. **Build script errors**: Incorrect paths, missing error handling in `.ps1`/`.sh` scripts
7. **New magic numbers**: Unexplained numeric literals in new business logic
8. **Dead code — unreachable**: Code after `return`/`throw` that can never execute
9. **Dead code — always-true/false condition**: `if (true)`, `if (x == x)`, constant conditions
10. **Dead code — `#if 0` blocks**: Disabled code blocks that should be removed
11. **Dead code — unused variables**: Variables declared but never read after assignment
12. **Optimization — string in loop**: `QString`/`QByteArray` construction inside loops (use `reserve()`)
13. **Optimization — repeated container size**: `container.count()` or `.size()` in loop condition (cache it)
14. **Optimization — unnecessary copy**: `QByteArray`/`QString` copied when a reference would suffice
15. **Optimization — `QStringLiteral`**: String literals used with `QString` should use `QStringLiteral`
16. **Potential bugs — off-by-one**: Loop bounds `< size` vs `<= size`, array index `[n]` vs `[n-1]`
17. **Potential bugs — floating point equality**: `float == float` comparison (use `qFuzzyCompare`)
18. **Error handling**: New functions that can fail but don't return/throw on error

### 🔵 Info — Nice to Have
1. **New long functions**: Functions over 100 lines added in this PR
2. **New TODO without ticket**: New TODO/FIXME comments without a ticket reference
3. **New commented-out code**: Newly added commented-out code blocks
4. **Dead code — commented-out functions**: Large commented-out code blocks (use git history instead)
5. **Optimization — `reserve()` missing**: `QList`/`QVector`/`QByteArray` appended in loop without `reserve()`
6. **API documentation**: New public methods without a doc comment explaining parameters/return value
7. **Test coverage hint**: New public methods that have no corresponding test in `test_tac.py` or similar

---

## 🔀 Control Flow & Cross-Function Analysis

**Do not review each function in isolation — trace the call chain.** Many real
defects only exist in how functions interact; reading one function looks fine.
Before commenting, follow the flow across the functions **visible in this batch**:

### Trace these interprocedural patterns
1. **Null-return propagation.** If a called function can `return Q_NULLPTR;` (or
   `nullptr`) on any path, then **every caller must null-check before dereferencing**.
   Read the callee's body to learn its return contract; a caller doing
   `findX(...)->method()` with no check is a null-deref bug.
2. **Use-after-free / use-after-null across a helper.** If a helper does
   `delete X;` / `X->deleteLater();` and/or sets `X = Q_NULLPTR;`, then any caller
   that touches `X` **after** calling that helper is a use-after-free / null deref.
   Trace what each called function does to shared members.
3. **Ownership across a call.** Follow `new`/factory return values across function
   boundaries. If a factory returns an owning raw pointer (`return new Session();`)
   and the caller neither deletes it, transfers it to an owning container, nor wraps
   it in a smart pointer → memory leak. A `by-value` sink parameter does **not** take
   ownership of a heap object.
4. **Constant conditions from an earlier guard.** A condition that an earlier
   `return`/guard already made always-true or always-false is dead/buggy logic.

### Flow-dependent dead code
- **Newly added private methods never referenced** anywhere in the change (no caller
  in the diff) — flag as likely dead code (🔵), noting it may be called elsewhere.
- Code after `return`/`throw`/`break` in the same block — unreachable (🟡).

### ⚠️ Be honest about what you can see — do NOT guess
You are given only the **diff**, split into **batches**; you do not see whole files.
- If the definition of a called function is **not present in this batch**, you cannot
  confirm its return contract or side effects. Label such a finding
  **"⚠️ unverified — callee not in diff/batch"** instead of asserting it, and phrase it
  as a question ("does `findX()` return null here?") rather than a definitive bug.
- **Never invent line numbers, function bodies, or call sites you cannot see.**

### Established patterns still win
Flow suspicion never overrides the "Established Patterns" section. A
`if (_session != Q_NULLPTR)` check, the `== true` style, and the drive-thread
`deleteLater()` cleanup are correct — do not "flow-flag" them.

---

## 🚫 Files to Skip Entirely

Do NOT review these files/directories:
- `third-party/` — external FTDI driver archives
- `__Builds/` — build output
- `__Installer/` — installer output
- `src/applications/tacservice/python/thrift/` — auto-generated Thrift code
- `*.tcnf` — binary-like XML configuration files (review only if explicitly changed)
- `*.nsi` — NSIS installer scripts (only review if logic changes)
- `build_final_verify.log`, `*.txt` — log/test output files

---

## 🏷️ CMakeLists.txt Review Guidelines

When `CMakeLists.txt` files are changed:
- Verify `target_link_libraries` uses correct target names (e.g., `Qt6::Core`, not `Qt6Core`)
- Check `find_package` calls have correct component names
- Verify platform guards: `if(WIN32)`, `if(UNIX)`, `if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")`
- Check that new source files are added to the correct target
- Verify install rules use correct paths

---

## 🔌 Thrift Interface Review Guidelines

When `.thrift` files are changed:
- **Breaking changes** (flag as critical): removing fields, changing field types, renaming services
- **Safe changes**: adding new optional fields, adding new service methods
- Verify field IDs are unique and not reused
- Check that new service methods have corresponding handler implementations

---

## 🐍 Python Code Review Guidelines

For Python files in `src/applications/tacservice/python/`:
- Skip auto-generated files in `thrift/` subdirectory
- Flag bare `except:` clauses (use specific exception types)
- Flag hardcoded credentials or tokens
- Flag `eval()`/`exec()` usage
- The `TacServiceClient.py` and `test_tac.py` are hand-written — review these

---

## 📦 Packaging/Installer Review Guidelines

For `packaging/windows/*.ps1` and `packaging/linux/*.sh`:
- Check for hardcoded paths that should be parameterized
- Verify error handling (`$ErrorActionPreference = "Stop"` in PowerShell)
- Check that version numbers are read from CMakeLists.txt, not hardcoded
- Verify that installer scripts handle both x64 and ARM64 architectures

---

## 🔐 Security Checklist

Always check for:
- Hardcoded API keys, passwords, tokens, or secrets
- Hardcoded IP addresses or hostnames (should be configurable)
- Unsafe string formatting that could lead to injection
- File path traversal vulnerabilities in file operations

---

## 💬 Review Comment Format

Post a **summary comment** using `gh pr comment` with this structure:

```
## 🤖 QTAC Code Review

### Verdict: [✅ Approve | ⚠️ Changes Requested | 💬 Comment]

### Summary
[Brief description of what the PR does]

### Issues Found
[List issues by severity: 🔴 Critical, 🟡 Warning, 🔵 Info]

### ✅ Good Practices
[Acknowledge good things in the PR]

### 📋 Checklist
- [ ] No hardcoded secrets
- [ ] Thread safety maintained
- [ ] CMakeLists.txt correct
- [ ] Thrift compatibility preserved (if applicable)

> 🤖 _Generated by an AI review agent — findings may be incorrect or incomplete. Treat every comment as advisory and verify before acting; this is not a substitute for human review._
```

**Always include the AI-generated disclaimer above as the last line of every summary comment**, and prefix inline comments with `🤖 AI review (verify): ` — the review's correctness is not yet trusted, so every comment must be clearly labeled as machine-generated.

For **inline comments**, use `mcp__github_inline_comment__create_inline_comment` with `confirmed: true`.

---

## 🔕 Suppression Mechanism

Contributors can suppress false positives:
- **C++**: Add `// NOLINT(<rule>)` at end of line
- **Python**: Add `# noqa: <code>` at end of line  
- **PR-level**: Add `[skip-review]` to PR description to skip the entire review

---

*Last updated: 2026 | Maintained by the QTAC team*