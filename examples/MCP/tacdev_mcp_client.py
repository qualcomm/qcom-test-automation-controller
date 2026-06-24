#!/usr/bin/env python3

# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

"""
MCP client for tacdev_mcp_server.py.

Launches the server as a subprocess and exposes a clean
TACDevClient class whose methods map 1-to-1 to every MCP tool.

Typical usage
-------------
    from tacdev_mcp_client import TACDevClient

    with TACDevClient() as tac:
        count = tac.get_device_count()
        handle = tac.open_handle_by_description("TAC-Lite")
        tac.power_on_button(handle)
        tac.close_tac_handle(handle)

Run as a script for a quick interactive demo:
    python tacdev_mcp_client.py [port_name]
"""

from __future__ import annotations

import asyncio
import os
import sys
from contextlib import asynccontextmanager
from pathlib import Path
from typing import Any

from mcp import ClientSession, StdioServerParameters
from mcp.client.stdio import stdio_client

# --------------------------------------------------------------------------- #
# Server launch parameters
# --------------------------------------------------------------------------- #
_SERVER_SCRIPT = str(Path(__file__).with_name("tacdev_mcp_server.py"))

_SERVER_PARAMS = StdioServerParameters(
    command=sys.executable,
    args=[_SERVER_SCRIPT],
    env={
        **os.environ,
        # Override library path here if needed, or set TACDEV_LIB_PATH in env.
        # "TACDEV_LIB_PATH": r"C:\path\to\TACDev.dll",
    },
)


# --------------------------------------------------------------------------- #
# Low-level async helper
# --------------------------------------------------------------------------- #
@asynccontextmanager
async def _session():
    """Async context manager that yields a live MCP ClientSession."""
    async with stdio_client(_SERVER_PARAMS) as (read, write):
        async with ClientSession(read, write) as session:
            await session.initialize()
            yield session


async def _call(session: ClientSession, tool: str, **kwargs: Any) -> Any:
    """Call one MCP tool and return its unwrapped result value."""
    result = await session.call_tool(tool, arguments=kwargs)
    # FastMCP returns a list of TextContent; pull out the parsed dict.
    if result.content:
        import json
        text = result.content[0].text
        try:
            return json.loads(text)
        except (ValueError, TypeError):
            return text
    return None


# --------------------------------------------------------------------------- #
# Synchronous TACDevClient
# --------------------------------------------------------------------------- #
class TACDevClient:
    """
    Synchronous wrapper around the TACDev MCP server.

    Can be used as a context manager:
        with TACDevClient() as tac:
            ...

    Or manually:
        tac = TACDevClient()
        tac.connect()
        tac.disconnect()
    """

    def __init__(self) -> None:
        self._loop: asyncio.AbstractEventLoop | None = None
        self._session: ClientSession | None = None
        self._ctx = None

    # ── Lifecycle ────────────────────────────────────────────────────────────

    def connect(self) -> "TACDevClient":
        self._loop = asyncio.new_event_loop()
        self._ctx  = _session()
        self._session = self._loop.run_until_complete(self._ctx.__aenter__())
        return self

    def disconnect(self) -> None:
        if self._ctx and self._loop:
            self._loop.run_until_complete(self._ctx.__aexit__(None, None, None))
        if self._loop:
            self._loop.close()
        self._session = None
        self._loop    = None
        self._ctx     = None

    def __enter__(self) -> "TACDevClient":
        return self.connect()

    def __exit__(self, *_) -> None:
        self.disconnect()

    # ── Internal dispatch ────────────────────────────────────────────────────

    def _call(self, tool: str, **kwargs: Any) -> Any:
        if self._session is None or self._loop is None:
            raise RuntimeError("Not connected — call connect() or use as a context manager")
        return self._loop.run_until_complete(_call(self._session, tool, **kwargs))

    # ── Version / diagnostics ────────────────────────────────────────────────

    def get_alpaca_version(self) -> str:
        """Return the Alpaca firmware version string."""
        return self._call("get_alpaca_version")["alpaca_version"]

    def get_tac_version(self) -> str:
        """Return the TACDev library version string."""
        return self._call("get_tac_version")["tac_version"]

    def get_last_tac_error(self) -> str:
        """Return the last error message recorded by TACDev."""
        return self._call("get_last_tac_error")["last_error"]

    # ── Logging ──────────────────────────────────────────────────────────────

    def get_logging_state(self) -> bool:
        """Return whether TACDev logging is currently enabled."""
        return self._call("get_logging_state")["logging_enabled"]

    def set_logging_state(self, enabled: bool) -> None:
        """Enable or disable TACDev logging."""
        self._call("set_logging_state", enabled=enabled)

    # ── Device enumeration ───────────────────────────────────────────────────

    def get_device_count(self) -> int:
        """Return the number of connected TAC devices."""
        return self._call("get_device_count")["device_count"]

    def get_port_data(self, device_index: int) -> str:
        """Return the port description string for the device at device_index."""
        return self._call("get_port_data", device_index=device_index)["port_data"]

    def list_ports(self) -> list[str]:
        """Convenience: return port descriptions for all connected devices."""
        count = self.get_device_count()
        return [self.get_port_data(i) for i in range(count)]

    # ── Handle management ────────────────────────────────────────────────────

    def open_handle_by_description(self, port_name: str) -> int:
        """Open a TAC device by port description and return its integer handle."""
        return self._call("open_handle_by_description", port_name=port_name)["handle"]

    def close_tac_handle(self, handle: int) -> None:
        """Close a previously opened TAC device handle."""
        self._call("close_tac_handle", handle=handle)

    # ── Device info ──────────────────────────────────────────────────────────

    def get_name(self, handle: int) -> str:
        return self._call("get_name", handle=handle)["name"]

    def get_firmware_version(self, handle: int) -> str:
        return self._call("get_firmware_version", handle=handle)["firmware_version"]

    def get_hardware(self, handle: int) -> str:
        return self._call("get_hardware", handle=handle)["hardware"]

    def get_hardware_version(self, handle: int) -> str:
        return self._call("get_hardware_version", handle=handle)["hardware_version"]

    def get_uuid(self, handle: int) -> str:
        return self._call("get_uuid", handle=handle)["uuid"]

    def get_device_info(self, handle: int) -> dict:
        """Convenience: return all device identity fields in one call."""
        return {
            "name":             self.get_name(handle),
            "firmware_version": self.get_firmware_version(handle),
            "hardware":         self.get_hardware(handle),
            "hardware_version": self.get_hardware_version(handle),
            "uuid":             self.get_uuid(handle),
        }

    # ── External power ───────────────────────────────────────────────────────

    def set_external_power_control(self, handle: int, state: bool) -> None:
        self._call("set_external_power_control", handle=handle, state=state)

    # ── Dynamic commands ─────────────────────────────────────────────────────

    def list_commands(self, handle: int) -> list[str]:
        return self._call("list_commands", handle=handle)["commands"]

    def get_command(self, handle: int, command_index: int) -> str:
        return self._call("get_command", handle=handle, command_index=command_index)["command"]

    def list_quick_commands(self, handle: int) -> list[str]:
        return self._call("list_quick_commands", handle=handle)["quick_commands"]

    # ── Script variables ─────────────────────────────────────────────────────

    def list_script_variables(self, handle: int) -> list[str]:
        return self._call("list_script_variables", handle=handle)["script_variables"]

    def update_script_variable(self, handle: int, variable: str, value: str) -> None:
        self._call("update_script_variable", handle=handle, variable=variable, value=value)

    # ── Core command interface ────────────────────────────────────────────────

    def get_command_state(self, handle: int, command: str) -> bool:
        return self._call("get_command_state", handle=handle, command=command)["state"]

    def send_command(self, handle: int, command: str, state: bool) -> None:
        self._call("send_command", handle=handle, command=command, state=state)

    # ── Help / queue ─────────────────────────────────────────────────────────

    def get_help_text(self, handle: int) -> str:
        return self._call("get_help_text", handle=handle)["help_text"]

    def is_command_queue_clear(self, handle: int) -> bool:
        return self._call("is_command_queue_clear", handle=handle)["queue_clear"]

    # ── Raw pin control ───────────────────────────────────────────────────────

    def set_pin_state(self, handle: int, pin: int, state: bool) -> None:
        self._call("set_pin_state", handle=handle, pin=pin, state=state)

    # ── Battery ───────────────────────────────────────────────────────────────

    def set_battery_state(self, handle: int, state: bool) -> None:
        self._call("set_battery_state", handle=handle, state=state)

    def get_battery_state(self, handle: int) -> bool:
        return self._call("get_battery_state", handle=handle)["state"]

    # ── USB ───────────────────────────────────────────────────────────────────

    def set_usb0(self, handle: int, state: bool) -> None:
        self._call("set_usb0", handle=handle, state=state)

    def get_usb0_state(self, handle: int) -> bool:
        return self._call("get_usb0_state", handle=handle)["state"]

    def set_usb1(self, handle: int, state: bool) -> None:
        self._call("set_usb1", handle=handle, state=state)

    def get_usb1_state(self, handle: int) -> bool:
        return self._call("get_usb1_state", handle=handle)["state"]

    # ── Power key ─────────────────────────────────────────────────────────────

    def set_power_key(self, handle: int, state: bool) -> None:
        self._call("set_power_key", handle=handle, state=state)

    def get_power_key_state(self, handle: int) -> bool:
        return self._call("get_power_key_state", handle=handle)["state"]

    # ── Volume ────────────────────────────────────────────────────────────────

    def set_volume_up(self, handle: int, state: bool) -> None:
        self._call("set_volume_up", handle=handle, state=state)

    def get_volume_up_state(self, handle: int) -> bool:
        return self._call("get_volume_up_state", handle=handle)["state"]

    def set_volume_down(self, handle: int, state: bool) -> None:
        self._call("set_volume_down", handle=handle, state=state)

    def get_volume_down_state(self, handle: int) -> bool:
        return self._call("get_volume_down_state", handle=handle)["state"]

    # ── SIM / SD ──────────────────────────────────────────────────────────────

    def set_disconnect_uim1(self, handle: int, state: bool) -> None:
        self._call("set_disconnect_uim1", handle=handle, state=state)

    def get_disconnect_uim1_state(self, handle: int) -> bool:
        return self._call("get_disconnect_uim1_state", handle=handle)["state"]

    def set_disconnect_uim2(self, handle: int, state: bool) -> None:
        self._call("set_disconnect_uim2", handle=handle, state=state)

    def get_disconnect_uim2_state(self, handle: int) -> bool:
        return self._call("get_disconnect_uim2_state", handle=handle)["state"]

    def set_disconnect_sd_card(self, handle: int, state: bool) -> None:
        self._call("set_disconnect_sd_card", handle=handle, state=state)

    def get_disconnect_sd_card_state(self, handle: int) -> bool:
        return self._call("get_disconnect_sd_card_state", handle=handle)["state"]

    # ── EDL ───────────────────────────────────────────────────────────────────

    def set_primary_edl(self, handle: int, state: bool) -> None:
        self._call("set_primary_edl", handle=handle, state=state)

    def get_primary_edl_state(self, handle: int) -> bool:
        return self._call("get_primary_edl_state", handle=handle)["state"]

    def set_secondary_edl(self, handle: int, state: bool) -> None:
        self._call("set_secondary_edl", handle=handle, state=state)

    def get_secondary_edl_state(self, handle: int) -> bool:
        return self._call("get_secondary_edl_state", handle=handle)["state"]

    # ── PS_HOLD / RESIN_N ────────────────────────────────────────────────────

    def set_force_ps_hold_high(self, handle: int, state: bool) -> None:
        self._call("set_force_ps_hold_high", handle=handle, state=state)

    def get_force_ps_hold_high_state(self, handle: int) -> bool:
        return self._call("get_force_ps_hold_high_state", handle=handle)["state"]

    def set_secondary_pm_resin_n(self, handle: int, state: bool) -> None:
        self._call("set_secondary_pm_resin_n", handle=handle, state=state)

    def get_secondary_pm_resin_n_state(self, handle: int) -> bool:
        return self._call("get_secondary_pm_resin_n_state", handle=handle)["state"]

    # ── EUD ───────────────────────────────────────────────────────────────────

    def set_eud(self, handle: int, state: bool) -> None:
        self._call("set_eud", handle=handle, state=state)

    def get_eud_state(self, handle: int) -> bool:
        return self._call("get_eud_state", handle=handle)["state"]

    # ── Headset ───────────────────────────────────────────────────────────────

    def set_headset_disconnect(self, handle: int, state: bool) -> None:
        self._call("set_headset_disconnect", handle=handle, state=state)

    def get_headset_disconnect_state(self, handle: int) -> bool:
        return self._call("get_headset_disconnect_state", handle=handle)["state"]

    # ── Name / reset count ───────────────────────────────────────────────────

    def set_name(self, handle: int, new_name: str) -> None:
        self._call("set_name", handle=handle, new_name=new_name)

    def get_reset_count(self, handle: int) -> int:
        return self._call("get_reset_count", handle=handle)["reset_count"]

    def clear_reset_count(self, handle: int) -> None:
        self._call("clear_reset_count", handle=handle)

    # ── Button sequences ─────────────────────────────────────────────────────

    def power_on_button(self, handle: int) -> None:
        """Execute the power-on button sequence."""
        self._call("power_on_button", handle=handle)

    def power_off_button(self, handle: int) -> None:
        """Execute the power-off button sequence."""
        self._call("power_off_button", handle=handle)

    def boot_to_fastboot_button(self, handle: int) -> None:
        """Boot the device into Fastboot mode."""
        self._call("boot_to_fastboot_button", handle=handle)

    def boot_to_uefi_menu_button(self, handle: int) -> None:
        """Boot the device into the UEFI menu."""
        self._call("boot_to_uefi_menu_button", handle=handle)

    def boot_to_edl_button(self, handle: int) -> None:
        """Boot the device into primary EDL mode."""
        self._call("boot_to_edl_button", handle=handle)

    def boot_to_secondary_edl_button(self, handle: int) -> None:
        """Boot the device into secondary EDL mode."""
        self._call("boot_to_secondary_edl_button", handle=handle)


# --------------------------------------------------------------------------- #
# Quick demo / smoke-test (run as script)
# --------------------------------------------------------------------------- #
def _demo(port_name: str | None = None) -> None:
    with TACDevClient() as tac:
        print(f"  Alpaca version : {tac.get_alpaca_version()}")
        print(f"  TAC version    : {tac.get_tac_version()}")
        print(f"  Logging        : {tac.get_logging_state()}")

        ports = tac.list_ports()
        print(f"  Devices found  : {len(ports)}")
        for i, p in enumerate(ports):
            print(f"    [{i}] {p}")

        if not port_name and ports:
            port_name = ports[0]

        if port_name:
            print(f"\nOpening '{port_name}'...")
            handle = tac.open_handle_by_description(port_name)
            info   = tac.get_device_info(handle)
            for k, v in info.items():
                print(f"  {k:<20}: {v}")

            print(f"  Commands       : {tac.list_commands(handle)}")
            print(f"  Queue clear    : {tac.is_command_queue_clear(handle)}")

            tac.close_tac_handle(handle)
            print("Handle closed.")
        else:
            print("No device available — skipping handle demo.")


if __name__ == "__main__":
    _demo(sys.argv[1] if len(sys.argv) > 1 else None)
