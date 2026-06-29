# QTAC MCP Server — SSE

Persistent server supporting multiple concurrent clients. Each client session gets exclusive
ownership of a device.

Use this variant when multiple automation clients need to share a device pool simultaneously.
For single-client use see [`../stdio/`](../stdio/).

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
pip install -r examples/MCP/sse/requirements.txt
```

## Configuration

`config.yaml` — all fields optional, defaults shown:

| Parameter | Default | Description |
| :-- | :-- | :-- |
| `server.host` | `127.0.0.1` | Bind address |
| `server.port` | `8000` | Port |
| `logging.file` | `tacdev_mcp.log` | Log file path |
| `logging.level` | `INFO` | DEBUG / INFO / WARNING / ERROR |
| `logging.max_bytes` | `10485760` | Rotation size (10 MB) |
| `logging.backup_count` | `5` | Rotated files to keep |

## Running

The server must be started before any client connects:

```bash
python examples/MCP/sse/tacdev_mcp_server.py
```

### Option 1 — Python client

```bash
python examples/MCP/sse/tacdev_mcp_client.py           # first available device
python examples/MCP/sse/tacdev_mcp_client.py COM41     # specific port
```

### Option 2 — Claude CLI

Register once (server must already be running):

```bash
claude mcp add --transport sse TACDev-MCP http://127.0.0.1:8000/sse
```

Verify:

```bash
claude mcp list
```

Then ask Claude naturally — no special syntax needed:

```
List connected devices
Power on COM41
Boot COM3 to fastboot
```

> **Note:** The server must be running before starting a Claude session. Claude connects to it
> over SSE; it does not start the server automatically.

## Recovering a locked device

If a client crashes without closing its handle, the device will remain locked. Restart the server
to release all devices cleanly:

```
Ctrl+C
python examples/MCP/sse/tacdev_mcp_server.py
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
