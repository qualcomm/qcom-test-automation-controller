# CLI Plan for TACDev Interface

## Context
Create a GNU-compliant command-line interface exposing all functionality from `interfaces/C++/TACDev/TACDev.h`. This enables shell scripting and automation without C++ code.

## Global Options

| Option | Description |
| :-- | :-- |
| `-d, --device=<port>` | Select device by port name |
| `-s, --serial=<serial>` | Select device by serial number |
| `-o, --output=<format>` | Output format: `text` (default) or `json` |
| `-h, --help` | Show help |
| `--version` | Show taccl and TAC library versions |

`-d` and `-s` are mutually exclusive. One is required for device operations (unless set via environment).

Env: `TACCL_DEVICE` (default for `-d`), `TACCL_SERIAL` (default for `-s`), `TACCL_OUTPUT=json` (default output format).

---

## Commands

### Device Discovery
```
taccl list
```
Backend: `GetDeviceCount` + `GetPortData`

### Version Information
```
taccl version
```
Backend: `GetTACVersion`

### Device Information
```
taccl info (-d|-s)                        Show all device info
taccl info (-d|-s) --name                 Show name only
taccl info (-d|-s) --firmware             Show firmware version only
taccl info (-d|-s) --hardware             Show hardware only
taccl info (-d|-s) --hardware-version     Show hardware version only
taccl info (-d|-s) --uuid                 Show UUID only
```
Backend: `GetName`, `GetFirmwareVersion`, `GetHardware`, `GetHardwareVersion`, `GetUUID`

### Device Configuration
```
taccl set (-d|-s) --name=<value>          Set device name
```
Backend: `SetName`

### Logging
```
taccl logging (-d|-s)                     Query logging state
taccl logging (-d|-s) --state=true|false  Set logging state
```
Backend: `GetLoggingState`, `SetLoggingState`

### Commands
```
taccl commands (-d|-s)                              List available stateful commands
taccl quick-commands (-d|-s)                        List available quick commands
taccl command (-d|-s) --name=<cmd>                  Query command state
taccl command (-d|-s) --name=<cmd> --state=true|false   Set command state
taccl command (-d|-s) --name=<cmd>                  Run quick command (no --state)
```
Backend (list): `GetCommandCount` + `GetCommand`, `GetQuickCommandCount` + `GetQuickCommand`
Backend (send/query): `SendCommand`, `GetCommandState`

Note: `taccl command` handles both stateful and quick commands. For quick commands, omit `--state`. The CLI enumerates both lists to determine whether `--state` is required.

### Script Variables
```
taccl vars (-d|-s)                                  List all variables
taccl var (-d|-s) --name=<var>                      Get variable value
taccl var (-d|-s) --name=<var> --value=<val>        Set variable value
```
Backend: `GetScriptVariableCount` + `GetScriptVariable`, `UpdateScriptVariableValue`

### Utility
```
taccl help-text (-d|-s)                             Print device help text
```
Backend: `GetHelpText`

---

## Boolean Values

Input accepts: `true`, `false`, `1`, `0`
Output is always: `true` or `false`

---

## Queue Management

Before every `CloseTACHandle`, the CLI automatically calls `IsCommandQueueClear` and waits for the queue to drain. This is not user-visible.

---

## Exit Codes

| Code | Meaning |
| :-- | :-- |
| 0 | Success |
| 1 | General error |
| 2 | Device not found |
| 3 | Invalid arguments / usage error |
| 4 | TAC command failed |
| 5 | Initialization failed |

Error messages sourced from `GetLastTACError`.

---

## Output Formats

### Human-readable (default)

```
$ taccl list
PORT     SERIAL
COM3     ABC123
COM5     DEF456

$ taccl info -d=COM3
name: TAC-Device-1
firmware: 1.2.3
hardware: TAC-Lite
hardware-version: 2.0
uuid: 550e8400-e29b-41d4-a716-446655440000

$ taccl command -d=COM3 --name=battery
true

$ taccl command -d=COM3 --name=battery --state=false
false
```

### JSON (`-o json`)

Every response includes a mandatory `code` field (0 on success). Errors add an `error` field.

```
$ taccl -o=json list
{"code":0,"devices":[{"port":"COM3","serial":"ABC123"},{"port":"COM5","serial":"DEF456"}]}

$ taccl -d=COM3 -o=json info
{"code":0,"name":"TAC-Device-1","firmware":"1.2.3","hardware":"TAC-Lite","hardwareVersion":"2.0","uuid":"550e8400-e29b-41d4-a716-446655440000"}

$ taccl -d=COM3 -o=json commands
{"code":0,"commands":["battery","usb0","usb1","pkey","volup","voldn","pedl","sedl"]}

$ taccl -d=COM3 -o=json quick-commands
{"code":0,"quickCommands":["bootToEDL","bootToFastboot"]}

$ taccl -d=COM3 -o=json command --name=battery
{"code":0,"command":"battery","state":true}

$ taccl -d=COM3 -o=json command --name=battery --state=false
{"code":0,"command":"battery","state":false}

$ taccl -d=COM3 -o=json var --name=edl
{"code":0,"variable":"edl","value":"1200"}

$ taccl -d=COM3 -o=json var --name=edl --value=1200
{"code":0,"variable":"edl","value":"1200"}

$ taccl -d=COM99 -o=json info
{"code":2,"error":"device 'COM99' not found"}
```

---

## Error Handling

- Errors written to stderr
- Exit with appropriate code
- JSON errors follow the same schema when `-o json` is set

```
$ taccl info -d=COM99
taccl: device 'COM99' not found
$ echo $?
2

$ taccl command -d=COM3 --name=battery --state=invalid
taccl: invalid value 'invalid' for --state, expected 'true', 'false', '1', or '0'
$ echo $?
3
```

---

## Build Requirements

- **No Qt dependency** - standalone C++ with standard library only
- **Independent build** - separate CMake project, not part of main qtac-cli build
- **Links against TACDev** - uses the TACDev shared library (DLL/so)
- **Cross-platform** - Windows and Linux support

## Dependencies

| Library | Purpose | Integration | License |
|---------|---------|-------------|---------|
| [nlohmann/json](https://github.com/nlohmann/json) | JSON output (recommended) | Header-only, CMake FetchContent | MIT |
| [CLI11](https://github.com/CLIUtils/CLI11) | Argument parsing (recommended) | Header-only, CMake FetchContent | BSD-3-Clause |

### Why CLI11?
- Native subcommand support
- Auto-generates `--help` and `--version`
- GNU-compliant option syntax (`--option=value`)
- Header-only, easy to integrate
- Cross-platform (no Windows porting issues)

**Alternative:** getopt_long can be used instead if preferred (requires manual subcommand handling and Windows port).

## Files to Create

| File | Purpose |
|------|---------|
| `tools/taccl/CMakeLists.txt` | Standalone CMake project |
| `tools/taccl/main.cpp` | Entry point |
| `tools/taccl/CLI.h` | CLI11 setup and subcommand definitions |
| `tools/taccl/CLI.cpp` | Argument parsing implementation |
| `tools/taccl/Commands.h` | Subcommand handlers header |
| `tools/taccl/Commands.cpp` | Subcommand implementations |
| `tools/taccl/Output.h` | Human/JSON output formatting |
| `tools/taccl/Output.cpp` | Output implementation |

### Reference Files
- `interfaces/C++/TACDev/TACDev.h` - API to wrap
- `examples/C++/TACExample.cpp` - Standalone TACDev usage example (no Qt)

---

## Verification
1. Build standalone:
   ```
   cd tools/taccl
   cmake -B build
   cmake --build build
   ```
2. `taccl --help` - verify help text
3. `taccl version` - verify version output
4. `taccl list` - enumerate devices
5. `taccl info -d=<port>` - query device info
6. `taccl command -d=<port> --name=battery` - query state
7. `taccl command -d=<port> --name=battery --state=true` - change state
8. `taccl list -o=json | jq .` - verify JSON output
9. `TACCL_DEVICE=<port> taccl command --name=battery` - verify env var fallback
