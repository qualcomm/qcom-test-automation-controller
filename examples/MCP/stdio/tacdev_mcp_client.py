#!/usr/bin/env python3

# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

"""
MCP client for tacdev_mcp_server.py (stdio transport).

The server is spawned automatically as a subprocess — no separate process to
start before running this client.

Typical usage
-------------
    from tacdev_mcp_client import TACDevClient

    async def main():
        async with TACDevClient() as tac:
            devices = await tac.list_devices()
            handle = await tac.open_handle_by_description("COM3")
            await tac.power_on_button(handle)
            await tac.close_tac_handle(handle)

    asyncio.run(main())
"""

from __future__ import annotations

import asyncio
import json
import sys
from pathlib import Path
from typing import Any

from fastmcp import Client

_SERVER_PATH = Path(__file__).with_name("tacdev_mcp_server.py")


class TACDevClient:
    def __init__(self) -> None:
        self._client = Client({"mcpServers": {"tacdev": {"command": sys.executable, "args": [str(_SERVER_PATH)]}}})

    async def __aenter__(self) -> TACDevClient:
        await self._client.__aenter__()
        return self

    async def __aexit__(self, *args) -> None:
        await self._client.__aexit__(*args)

    async def _call(self, tool: str, **kwargs: Any) -> Any:
        result = await self._client.call_tool(tool, kwargs)
        if result.content:
            text = result.content[0].text
            try:
                return json.loads(text)
            except (ValueError, TypeError):
                return text
        return None

    async def list_devices(self) -> dict:
        return await self._call("list_devices")

    async def get_alpaca_version(self) -> str:
        return (await self._call("get_alpaca_version"))["alpaca_version"]

    async def get_tac_version(self) -> str:
        return (await self._call("get_tac_version"))["tac_version"]

    async def get_last_tac_error(self) -> str:
        return (await self._call("get_last_tac_error"))["last_error"]

    async def get_logging_state(self) -> bool:
        return (await self._call("get_logging_state"))["logging_enabled"]

    async def set_logging_state(self, enabled: bool) -> None:
        await self._call("set_logging_state", enabled=enabled)

    async def get_device_count(self) -> int:
        return (await self._call("get_device_count"))["device_count"]

    async def get_port_data(self, device_index: int) -> str:
        return (await self._call("get_port_data", device_index=device_index))["port_data"]

    async def list_ports(self) -> list[str]:
        count = await self.get_device_count()
        return [await self.get_port_data(i) for i in range(count)]

    async def open_handle_by_description(self, port_name: str) -> int:
        return (await self._call("open_handle_by_description", port_name=port_name))["handle"]

    async def close_tac_handle(self, handle: int) -> None:
        await self._call("close_tac_handle", handle=handle)

    async def get_name(self, handle: int) -> str:
        return (await self._call("get_name", handle=handle))["name"]

    async def get_firmware_version(self, handle: int) -> str:
        return (await self._call("get_firmware_version", handle=handle))["firmware_version"]

    async def get_hardware(self, handle: int) -> str:
        return (await self._call("get_hardware", handle=handle))["hardware"]

    async def get_hardware_version(self, handle: int) -> str:
        return (await self._call("get_hardware_version", handle=handle))["hardware_version"]

    async def get_uuid(self, handle: int) -> str:
        return (await self._call("get_uuid", handle=handle))["uuid"]

    async def get_device_info(self, handle: int) -> dict:
        return {
            "name":             await self.get_name(handle),
            "firmware_version": await self.get_firmware_version(handle),
            "hardware":         await self.get_hardware(handle),
            "hardware_version": await self.get_hardware_version(handle),
            "uuid":             await self.get_uuid(handle),
        }

    async def set_external_power_control(self, handle: int, state: bool) -> None:
        await self._call("set_external_power_control", handle=handle, state=state)

    async def list_commands(self, handle: int) -> list[str]:
        return (await self._call("list_commands", handle=handle))["commands"]

    async def get_command(self, handle: int, command_index: int) -> str:
        return (await self._call("get_command", handle=handle, command_index=command_index))["command"]

    async def list_quick_commands(self, handle: int) -> list[str]:
        return (await self._call("list_quick_commands", handle=handle))["quick_commands"]

    async def list_script_variables(self, handle: int) -> list[str]:
        return (await self._call("list_script_variables", handle=handle))["script_variables"]

    async def update_script_variable(self, handle: int, variable: str, value: str) -> None:
        await self._call("update_script_variable", handle=handle, variable=variable, value=value)

    async def get_command_state(self, handle: int, command: str) -> bool:
        return (await self._call("get_command_state", handle=handle, command=command))["state"]

    async def send_command(self, handle: int, command: str, state: bool) -> None:
        await self._call("send_command", handle=handle, command=command, state=state)

    async def get_help_text(self, handle: int) -> str:
        return (await self._call("get_help_text", handle=handle))["help_text"]

    async def is_command_queue_clear(self, handle: int) -> bool:
        return (await self._call("is_command_queue_clear", handle=handle))["queue_clear"]

    async def set_pin_state(self, handle: int, pin: int, state: bool) -> None:
        await self._call("set_pin_state", handle=handle, pin=pin, state=state)

    async def set_battery_state(self, handle: int, state: bool) -> None:
        await self._call("set_battery_state", handle=handle, state=state)

    async def get_battery_state(self, handle: int) -> bool:
        return (await self._call("get_battery_state", handle=handle))["state"]

    async def set_usb0(self, handle: int, state: bool) -> None:
        await self._call("set_usb0", handle=handle, state=state)

    async def get_usb0_state(self, handle: int) -> bool:
        return (await self._call("get_usb0_state", handle=handle))["state"]

    async def set_usb1(self, handle: int, state: bool) -> None:
        await self._call("set_usb1", handle=handle, state=state)

    async def get_usb1_state(self, handle: int) -> bool:
        return (await self._call("get_usb1_state", handle=handle))["state"]

    async def set_power_key(self, handle: int, state: bool) -> None:
        await self._call("set_power_key", handle=handle, state=state)

    async def get_power_key_state(self, handle: int) -> bool:
        return (await self._call("get_power_key_state", handle=handle))["state"]

    async def set_volume_up(self, handle: int, state: bool) -> None:
        await self._call("set_volume_up", handle=handle, state=state)

    async def get_volume_up_state(self, handle: int) -> bool:
        return (await self._call("get_volume_up_state", handle=handle))["state"]

    async def set_volume_down(self, handle: int, state: bool) -> None:
        await self._call("set_volume_down", handle=handle, state=state)

    async def get_volume_down_state(self, handle: int) -> bool:
        return (await self._call("get_volume_down_state", handle=handle))["state"]

    async def set_disconnect_uim1(self, handle: int, state: bool) -> None:
        await self._call("set_disconnect_uim1", handle=handle, state=state)

    async def get_disconnect_uim1_state(self, handle: int) -> bool:
        return (await self._call("get_disconnect_uim1_state", handle=handle))["state"]

    async def set_disconnect_uim2(self, handle: int, state: bool) -> None:
        await self._call("set_disconnect_uim2", handle=handle, state=state)

    async def get_disconnect_uim2_state(self, handle: int) -> bool:
        return (await self._call("get_disconnect_uim2_state", handle=handle))["state"]

    async def set_disconnect_sd_card(self, handle: int, state: bool) -> None:
        await self._call("set_disconnect_sd_card", handle=handle, state=state)

    async def get_disconnect_sd_card_state(self, handle: int) -> bool:
        return (await self._call("get_disconnect_sd_card_state", handle=handle))["state"]

    async def set_primary_edl(self, handle: int, state: bool) -> None:
        await self._call("set_primary_edl", handle=handle, state=state)

    async def get_primary_edl_state(self, handle: int) -> bool:
        return (await self._call("get_primary_edl_state", handle=handle))["state"]

    async def set_secondary_edl(self, handle: int, state: bool) -> None:
        await self._call("set_secondary_edl", handle=handle, state=state)

    async def get_secondary_edl_state(self, handle: int) -> bool:
        return (await self._call("get_secondary_edl_state", handle=handle))["state"]

    async def set_force_ps_hold_high(self, handle: int, state: bool) -> None:
        await self._call("set_force_ps_hold_high", handle=handle, state=state)

    async def get_force_ps_hold_high_state(self, handle: int) -> bool:
        return (await self._call("get_force_ps_hold_high_state", handle=handle))["state"]

    async def set_secondary_pm_resin_n(self, handle: int, state: bool) -> None:
        await self._call("set_secondary_pm_resin_n", handle=handle, state=state)

    async def get_secondary_pm_resin_n_state(self, handle: int) -> bool:
        return (await self._call("get_secondary_pm_resin_n_state", handle=handle))["state"]

    async def set_eud(self, handle: int, state: bool) -> None:
        await self._call("set_eud", handle=handle, state=state)

    async def get_eud_state(self, handle: int) -> bool:
        return (await self._call("get_eud_state", handle=handle))["state"]

    async def set_headset_disconnect(self, handle: int, state: bool) -> None:
        await self._call("set_headset_disconnect", handle=handle, state=state)

    async def get_headset_disconnect_state(self, handle: int) -> bool:
        return (await self._call("get_headset_disconnect_state", handle=handle))["state"]

    async def set_name(self, handle: int, new_name: str) -> None:
        await self._call("set_name", handle=handle, new_name=new_name)

    async def get_reset_count(self, handle: int) -> int:
        return (await self._call("get_reset_count", handle=handle))["reset_count"]

    async def clear_reset_count(self, handle: int) -> None:
        await self._call("clear_reset_count", handle=handle)

    async def power_on_button(self, handle: int) -> None:
        await self._call("power_on_button", handle=handle)

    async def power_off_button(self, handle: int) -> None:
        await self._call("power_off_button", handle=handle)

    async def boot_to_fastboot_button(self, handle: int) -> None:
        await self._call("boot_to_fastboot_button", handle=handle)

    async def boot_to_uefi_menu_button(self, handle: int) -> None:
        await self._call("boot_to_uefi_menu_button", handle=handle)

    async def boot_to_edl_button(self, handle: int) -> None:
        await self._call("boot_to_edl_button", handle=handle)

    async def boot_to_secondary_edl_button(self, handle: int) -> None:
        await self._call("boot_to_secondary_edl_button", handle=handle)


async def TACExample(port_name: str | None = None) -> None:
    async with TACDevClient() as tac:
        print(f"  Alpaca version : {await tac.get_alpaca_version()}")
        print(f"  TAC version    : {await tac.get_tac_version()}")
        print(f"  Logging        : {await tac.get_logging_state()}")

        devices = await tac.list_devices()
        print(f"  Available      : {devices['available']}")

        if not port_name and devices["available"]:
            port_name = devices["available"][0]["port"]

        if port_name:
            print(f"\nOpening '{port_name}'...")
            handle = await tac.open_handle_by_description(port_name)

            info = await tac.get_device_info(handle)
            for k, v in info.items():
                print(f"  {k:<20}: {v}")

            print(f"\n  Queue clear    : {await tac.is_command_queue_clear(handle)}")

            print("\n--- Available commands ---")
            print(await tac.get_help_text(handle))

            commands = await tac.list_commands(handle)
            if commands:
                cmd_names = [c.split(";")[0].strip() for c in commands]
                print(f"\nCommands      : {cmd_names}")

            quick_commands = await tac.list_quick_commands(handle)
            if quick_commands:
                qc_names = [q.split(";")[1].strip() for q in quick_commands]
                print(f"Quick commands: {qc_names}")

            if commands:
                cmd = cmd_names[0]
                state_before = await tac.get_command_state(handle, cmd)
                print(f"\nCommand '{cmd}' state before toggle: {state_before}")
                await tac.send_command(handle, cmd, not state_before)
                await asyncio.sleep(2)
                state_after = await tac.get_command_state(handle, cmd)
                print(f"Command '{cmd}' state after  toggle: {state_after}")
                await tac.send_command(handle, cmd, state_before)
                await asyncio.sleep(2)
                print(f"Command '{cmd}' restored to        : {await tac.get_command_state(handle, cmd)}")

            await tac.close_tac_handle(handle)
            print("\nHandle closed.")
        else:
            print("No device available.")


if __name__ == "__main__":
    asyncio.run(TACExample(sys.argv[1] if len(sys.argv) > 1 else None))
