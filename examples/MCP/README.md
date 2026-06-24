# QTAC MCP Server

An [MCP](https://modelcontextprotocol.io) server that exposes the full TACDev API as tools,
allowing AI assistants to control Qualcomm devices via a QTAC debug board.

## Prerequisites

- Python 3.8+ (64-bit, matching your build architecture: x64 or ARM64)
- Project built with `build.bat` / `build.sh` (the TACDev library is loaded from `__Builds`)
- MCP-capable host

## Setup

Run from the `examples/MCP` directory:

```cmd
setup.bat          (Windows x64, auto-detected)
setup.bat ARM64    (Windows ARM64)
```
```bash
./setup.sh         (Linux)
```

Each script builds the project, installs the TACDev Python library, and installs MCP dependencies.

For full setup guidance see [Bootcamp guide](../../docs/bootcamp/01-Bootcamp.md) and
[Python API reference](../../docs/bootcamp/02-Python-API.md).

## Usage

`tacdev_mcp_server.py`: starts the MCP server over stdio. Run from the repo root:

```bash
python examples/MCP/tacdev_mcp_server.py
```

`tacdev_mcp_client.py`: launches the server as a subprocess and runs a device demo.
Pass an optional port name to open a specific device:

```bash
python examples/MCP/tacdev_mcp_client.py           # lists devices, opens first found
python examples/MCP/tacdev_mcp_client.py TAC-Lite  # opens device by port name
```

## Available tools

| Category | Tools |
| :-- | :-- |
| Diagnostics | `get_alpaca_version`, `get_tac_version` |
| Logging | `get_logging_state`, `set_logging_state` |
| Device enumeration | `get_device_count`, `get_port_data` |
| Handle management | `open_handle_by_description`, `close_tac_handle` |
| Device info | `get_name`, `get_firmware_version`, `get_hardware`, `get_hardware_version`, `get_uuid` |
| External power | `set_external_power_control` |
| Dynamic commands | `list_commands`, `get_command` |
| Quick commands | `list_quick_commands` |
| Script variables | `list_script_variables`, `update_script_variable` |
| Command interface | `get_command_state`, `send_command` |
| Help | `get_help_text` |
| Raw pin | `set_pin_state` |
| Command queue | `is_command_queue_clear` |
| Battery | `set_battery_state`, `get_battery_state` |
| USB | `set_usb0`, `get_usb0_state`, `set_usb1`, `get_usb1_state` |
| Buttons | `set_power_key`, `get_power_key_state`, `set_volume_up`, `get_volume_up_state`, `set_volume_down`, `get_volume_down_state` |
| SIM / SD | `set_disconnect_uim1`, `get_disconnect_uim1_state`, `set_disconnect_uim2`, `get_disconnect_uim2_state`, `set_disconnect_sd_card`, `get_disconnect_sd_card_state` |
| EDL | `set_primary_edl`, `get_primary_edl_state`, `set_secondary_edl`, `get_secondary_edl_state` |
| PS_HOLD / RESIN | `set_force_ps_hold_high`, `get_force_ps_hold_high_state`, `set_secondary_pm_resin_n`, `get_secondary_pm_resin_n_state` |
| EUD | `set_eud`, `get_eud_state` |
| Headset | `set_headset_disconnect`, `get_headset_disconnect_state` |
| Device name / resets | `set_name`, `get_reset_count`, `clear_reset_count` |
| Button sequences | `power_on_button`, `power_off_button`, `boot_to_fastboot_button`, `boot_to_uefi_menu_button`, `boot_to_edl_button`, `boot_to_secondary_edl_button` |

## Troubleshooting

**`ModuleNotFoundError: No module named 'TACDev'`**
Run `pip install interfaces/Python` from the repo root, or use `setup.bat` / `setup.sh`.

**`TACDev library not found`**
Build the project first with `build.bat` / `build.sh` and run scripts from the repo root.

**`Architecture mismatch`**
Use a 64-bit Python build matching your target architecture (x64 or ARM64).

**`get_device_count` returns 0**
Check the debug board is connected. On Windows, look for Unknown Device in Device Manager and
run `FTDICheck.exe` from `__Builds\x64\Release\bin` to diagnose the FTDI connection.
On Linux, ensure udev rules are installed:
```bash
sudo cp udev-rules/99-QTAC-USB.rules /etc/udev/rules.d/
sudo udevadm control --reload
```

See [Bootcamp troubleshooting](../../docs/bootcamp/01-Bootcamp.md#troubleshooting) for more.
