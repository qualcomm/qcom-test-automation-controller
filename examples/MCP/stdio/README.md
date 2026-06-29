# QTAC MCP Server — stdio

Server is spawned automatically per-client. No standalone process to manage.
Use this variant for Claude CLI integration.

For multi-client shared access see [`../sse/`](../sse/).

## Prerequisites

- Python 3.10+ (architecture must match: x64 or ARM64)
- Project built via `build.bat` / `build.sh`

## Setup

```bat
setup.bat        # Windows (auto-detects x64/ARM64)
```
```bash
./setup.sh       # Linux
```

## Claude CLI

**Register the server** (run once from repo root):

```bash
claude mcp add TACDev-MCP -- python examples/MCP/stdio/tacdev_mcp_server.py
```

**Verify:**

```bash
claude mcp list
```

**Then just ask Claude naturally:**

```
List connected devices
Power on the device on COM41
Boot COM3 to fastboot
```

**Scope options:**

| Flag | Saved to | Visible to |
| :-- | :-- | :-- |
| *(default)* | `.claude/settings.local.json` | you, this project |
| `--scope project` | `.mcp.json` (repo root) | whole team |
| `--scope user` | `~/.claude/settings.json` | you, all projects |

## Client demo

```bash
python examples/MCP/stdio/tacdev_mcp_client.py           # first available device
python examples/MCP/stdio/tacdev_mcp_client.py COM41     # specific port
```

## Troubleshooting

| Error | Fix |
| :-- | :-- |
| `No module named 'TACDev'` | `pip install interfaces/Python` from repo root |
| `TACDev library not found` | Run `build.bat` / `build.sh` first |
| `Architecture mismatch` | Use Python matching your OS architecture |
| `No module named 'fastmcp'` | `pip install -r requirements.txt` (requires Python 3.10+) |
| `cryptography` build failure on ARM64 | Re-run `setup.bat` — downloads OpenSSL automatically. Manual fallback: install [Win64ARMOpenSSL-4_0_1.msi](https://slproweb.com/download/Win64ARMOpenSSL-4_0_1.msi) (full, not Light) and set `OPENSSL_DIR=C:\Program Files\OpenSSL-Win64-ARM` |
| `get_device_count` returns 0 | Check board is connected. Windows: run `FTDICheck.exe` from `__Builds\x64\Release\bin`. Linux: `sudo cp udev-rules/99-QTAC-USB.rules /etc/udev/rules.d/ && sudo udevadm control --reload` |
