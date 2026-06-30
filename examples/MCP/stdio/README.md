# QTAC MCP Server - stdio

Server is spawned automatically per-client process. No standalone server to manage.

Use this variant for Claude CLI integration or single-client automation.
For multi-client shared access see [`../sse/`](../sse/).

## Prerequisites

- Python 3.10+ (architecture must match: x64 or ARM64)
- Project built via `build.bat` / `build.sh`

## Setup

Installs the TACDev library and all dependencies:

```bat
setup.bat        # Windows (auto-detects x64/ARM64)
```
```bash
./setup.sh       # Linux
```

Or manually:

```bash
pip install interfaces/Python          # from repo root
pip install -r examples/MCP/stdio/requirements.txt
```

## Running

No separate server process needed - the server is spawned automatically.

### Option 1 - Python client

```bash
python examples/MCP/stdio/tacdev_mcp_client.py           # first available device
python examples/MCP/stdio/tacdev_mcp_client.py COM41     # specific port
```

### Option 2 - Claude CLI

Register once from repo root:

```bash
claude mcp add TACDev-MCP -- python examples/MCP/stdio/tacdev_mcp_server.py
```

Verify:

```bash
claude mcp list
```

Then ask Claude naturally - no special syntax needed:

```
List connected devices
Power on COM41
Boot COM3 to fastboot
```

> **Note:** Claude spawns the server automatically on startup. No manual server management needed.

**Scope options:**

| Flag | Saved to | Visible to |
| :-- | :-- | :-- |
| *(default)* | `.claude/settings.local.json` | you, this project |
| `--scope project` | `.mcp.json` (repo root) | whole team |
| `--scope user` | `~/.claude/settings.json` | you, all projects |

## Troubleshooting

| Error | Fix |
| :-- | :-- |
| `No module named 'TACDev'` | `pip install interfaces/Python` from repo root |
| `TACDev library not found` | Run `build.bat` / `build.sh` first |
| `Architecture mismatch` | Use Python matching your OS architecture |
| `No module named 'fastmcp'` | `pip install -r requirements.txt` (requires Python 3.10+) |
| `cryptography` build failure on ARM64 | Re-run `setup.bat` - downloads OpenSSL automatically. Manual fallback: install [Win64ARMOpenSSL-4_0_1.msi](https://slproweb.com/download/Win64ARMOpenSSL-4_0_1.msi) (full, not Light) and set `OPENSSL_DIR=C:\Program Files\OpenSSL-Win64-ARM` |
| `get_device_count` returns 0 | Check board is connected. Windows: run `FTDICheck.exe` from `__Builds\x64\Release\bin`. Linux: `sudo cp udev-rules/99-QTAC-USB.rules /etc/udev/rules.d/ && sudo udevadm control --reload` |
