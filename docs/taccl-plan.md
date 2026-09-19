# CLI Plan for TACDev Interface

## Context
Create a GNU-compliant command-line interface exposing all functionality from `interfaces/C++/TACDev/TACDev.h`. This enables shell scripting and automation without C++ code.

## CLI Design

### Executable Name
`taccl` (pronounced "tackle")

### GNU Conventions
- Short options: `-d`, `-s`, `-j`, `-q`, `-h`, `-v`
- Long options: `--device`, `--serial`, `--json`, `--quiet`, `--help`, `--version`
- Long option arguments: `--device=COM3` or `--device COM3` (both supported)
- `--` separates options from arguments
- Options can appear in any order before arguments
- `--help` and `--version` are mandatory

### Global Options
```
-d, --device=<port>     Specify device by port name
-s, --serial=<serial>   Specify device by serial number
-j, --json              Output in JSON format
-q, --quiet             Suppress non-essential output
-h, --help              Show help
    --version           Show version info
```

### Environment Variables
```
TACCL_DEVICE           Default device port (overridden by -d)
TACCL_SERIAL           Default device serial (overridden by -s)
TACCL_JSON             Set to "1" for JSON output by default
```

Note: `-d` and `-s` are mutually exclusive. One is required for device operations (unless set via environment).

---

## Command Structure

### 1. Device Discovery
```
taccl list                     List all connected TAC devices
taccl list -j                  JSON output
```

### 2. Version Information
```
taccl version                  Show QTAC and TAC library versions
```

### 3. Device Information
```
taccl info --device=<port>              Show all device info
taccl info --serial=<serial>            Same, using serial number

# Specific fields (flags, no arguments):
taccl info --device=<port> --name
taccl info --device=<port> --firmware
taccl info --device=<port> --hardware
taccl info --device=<port> --hardware-version
taccl info --device=<port> --uuid
taccl info --device=<port> --reset-count
```

### 4. Device Configuration
```
taccl set --device=<port> --name=<new-name>
taccl set --device=<port> --clear-reset-count
```

### 5. State Controls
Pattern: `taccl <control> --device=<port> [--state=on|off]` (omit --state to query)

```
# Power
taccl battery --device=<port>                 Query battery state
taccl battery --device=<port> --state=on      Enable battery
taccl battery --device=<port> --state=off     Disable battery
taccl external-power --device=<port> --state=on|off

# USB
taccl usb0 --device=<port> [--state=on|off]
taccl usb1 --device=<port> [--state=on|off]

# Keys
taccl power-key --device=<port> [--state=on|off]
taccl volume-up --device=<port> [--state=on|off]
taccl volume-down --device=<port> [--state=on|off]

# Disconnect controls
taccl disconnect-uim1 --device=<port> [--state=on|off]
taccl disconnect-uim2 --device=<port> [--state=on|off]
taccl disconnect-sdcard --device=<port> [--state=on|off]
taccl disconnect-headset --device=<port> [--state=on|off]

# EDL
taccl primary-edl --device=<port> [--state=on|off]
taccl secondary-edl --device=<port> [--state=on|off]

# Other
taccl force-pshold --device=<port> [--state=on|off]
taccl secondary-pm-resin --device=<port> [--state=on|off]
taccl eud --device=<port> [--state=on|off]

# Raw pin (no query)
taccl pin --device=<port> --pin=<number> --state=on|off
```

### 6. Boot Sequences
```
taccl boot --device=<port> --mode=power-on
taccl boot --device=<port> --mode=power-off
taccl boot --device=<port> --mode=fastboot
taccl boot --device=<port> --mode=uefi
taccl boot --device=<port> --mode=edl
taccl boot --device=<port> --mode=secondary-edl
```

### 7. Commands
```
taccl commands --device=<port>                       List available commands
taccl command --device=<port> --name=<cmd>           Query command state
taccl command --device=<port> --name=<cmd> --state=on|off   Set command state

taccl quick-commands --device=<port>                 List quick commands
```

### 8. Script Variables
```
taccl vars --device=<port>                           List all variables
taccl var --device=<port> --name=<var>               Get variable value
taccl var --device=<port> --name=<var> --value=<val> Set variable value
```

### 9. Utility
```
taccl help-text --device=<port>                      Get device help text
taccl queue-clear --device=<port>                    Check if command queue is clear
taccl logging [--state=on|off]                       Get/set logging state (omit --state to query)
```

---

## Output Formats

### Human-readable (default)
```
$ taccl list
PORT     SERIAL
COM3     ABC123
COM5     DEF456

$ taccl info --device=COM3
name: TAC-Device-1
firmware: 1.2.3
hardware: TAC-Lite
hardware-version: 2.0
uuid: 550e8400-e29b-41d4-a716-446655440000
reset-count: 5

$ taccl battery --device=COM3
on

$ taccl battery --device=COM3 --state=off
off
```

### JSON (--json flag)
```
$ taccl list --json
[{"port":"COM3","serial":"ABC123"},{"port":"COM5","serial":"DEF456"}]

$ taccl info --device=COM3 --json
{"name":"TAC-Device-1","firmware":"1.2.3","hardware":"TAC-Lite","hardwareVersion":"2.0","uuid":"550e8400-e29b-41d4-a716-446655440000","resetCount":5}

$ taccl battery --device=COM3 --json
{"state":true}
```

---

## Exit Codes
```
0   Success
1   General error
2   Device not found
3   Invalid arguments / usage error
4   TAC command failed
5   Initialization failed
```

---

## Error Handling
- Errors to stderr
- Exit with appropriate code
- JSON errors when `-j` flag is set

```
$ taccl info --device=COM99
taccl: device 'COM99' not found
$ echo $?
2

$ taccl battery --device=COM3 --state=invalid
taccl: invalid value 'invalid' for --state, expected 'on' or 'off'
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
3. `taccl --version` - verify version output
4. `taccl list` - enumerate devices
5. `taccl info --device=<port>` - query device info
6. `taccl battery --device=<port>` - query state
7. `taccl battery --device=<port> --state=on` - change state
8. `taccl list --json | jq .` - verify JSON output
9. `TACCL_DEVICE=<port> taccl battery` - verify env var fallback
