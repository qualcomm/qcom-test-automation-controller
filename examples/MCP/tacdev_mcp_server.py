#!/usr/bin/env python3

# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

"""
MCP server for TACDev.
Wraps the QTAC TACDev Python library and exposes every function as an MCP tool.

Install the TACDev library first — see README.md for setup instructions.

Usage:
    python tacdev_mcp_server.py
"""

import TACDev
from fastmcp import FastMCP

# --------------------------------------------------------------------------- #
# Handle registry
# Bridges MCP integer handles to TACDevice objects returned by the library.
# --------------------------------------------------------------------------- #
_handles: dict[int, TACDev.TACDevice] = {}
_next_handle: int = 1


def _register(device: TACDev.TACDevice) -> int:
    global _next_handle
    handle = _next_handle
    _next_handle += 1
    _handles[handle] = device
    return handle


def _get_device(handle: int) -> TACDev.TACDevice:
    device = _handles.get(handle)
    if device is None:
        raise RuntimeError(f"Invalid handle {handle}. Call open_handle_by_description first.")
    return device


# --------------------------------------------------------------------------- #
# MCP server
# --------------------------------------------------------------------------- #
mcp = FastMCP("TACDev")


# ── Diagnostics ──────────────────────────────────────────────────────────────

@mcp.tool()
def get_alpaca_version() -> dict:
    """Return the QTAC (Alpaca) version string."""
    return {"alpaca_version": TACDev.AlpacaVersion()}


@mcp.tool()
def get_tac_version() -> dict:
    """Return the TACDev library version string."""
    return {"tac_version": TACDev.TACVersion()}


# ── Logging ──────────────────────────────────────────────────────────────────

@mcp.tool()
def get_logging_state() -> dict:
    """Return whether TACDev logging is currently enabled."""
    return {"logging_enabled": TACDev.GetLoggingState()}


@mcp.tool()
def set_logging_state(enabled: bool) -> dict:
    """Enable or disable TACDev logging."""
    TACDev.SetLoggingState(enabled)
    return {"success": True}


# ── Device enumeration ───────────────────────────────────────────────────────

@mcp.tool()
def get_device_count() -> dict:
    """Return the number of connected TAC devices."""
    return {"device_count": TACDev.GetDeviceCount()}


@mcp.tool()
def get_port_data(device_index: int) -> dict:
    """Return the port description string for the device at device_index."""
    device = TACDev.GetDevice(device_index)
    if device is None:
        raise RuntimeError(f"No device found at index {device_index}.")
    return {"port_data": device.PortName(), "description": device.Description(), "serial_number": device.SerialNumber()}


# ── Handle management ────────────────────────────────────────────────────────

@mcp.tool()
def open_handle_by_description(port_name: str) -> dict:
    """Open a TAC device by its port name and return an integer handle."""
    count = TACDev.GetDeviceCount()
    for i in range(count):
        device = TACDev.GetDevice(i)
        if device and device.PortName() == port_name:
            if not device.Open():
                raise RuntimeError(f"Found device '{port_name}' but failed to open it.")
            return {"handle": _register(device)}
    raise RuntimeError(f"No device found with port name '{port_name}'. Use get_port_data to list available devices.")


@mcp.tool()
def close_tac_handle(handle: int) -> dict:
    """Close a previously opened TAC device handle."""
    device = _get_device(handle)
    device.Close()
    del _handles[handle]
    return {"success": True}


# ── Device info ──────────────────────────────────────────────────────────────

@mcp.tool()
def get_name(handle: int) -> dict:
    """Return the human-readable name of the TAC device."""
    return {"name": _get_device(handle).Get_Name()}


@mcp.tool()
def get_firmware_version(handle: int) -> dict:
    """Return the firmware version of the TAC device."""
    return {"firmware_version": _get_device(handle).GetFirmwareVersion()}


@mcp.tool()
def get_hardware(handle: int) -> dict:
    """Return the hardware description of the TAC device."""
    return {"hardware": _get_device(handle).GetHardware()}


@mcp.tool()
def get_hardware_version(handle: int) -> dict:
    """Return the hardware version of the TAC device."""
    return {"hardware_version": _get_device(handle).Get_HardwareVersion()}


@mcp.tool()
def get_uuid(handle: int) -> dict:
    """Return the UUID of the TAC device."""
    return {"uuid": _get_device(handle).Get_UUID()}


# ── External power ───────────────────────────────────────────────────────────

@mcp.tool()
def set_external_power_control(handle: int, state: bool) -> dict:
    """Enable or disable external power control on the TAC device."""
    _get_device(handle).SetExternalPowerControl(state)
    return {"success": True}


# ── Dynamic commands ─────────────────────────────────────────────────────────

@mcp.tool()
def list_commands(handle: int) -> dict:
    """Return all dynamic command names available on the device."""
    device = _get_device(handle)
    count  = device.GetCommandCount()
    return {"commands": [device.GetCommand(i) for i in range(count)]}


@mcp.tool()
def get_command(handle: int, command_index: int) -> dict:
    """Return the dynamic command name at command_index."""
    return {"command": _get_device(handle).GetCommand(command_index)}


# ── Quick commands ───────────────────────────────────────────────────────────

@mcp.tool()
def list_quick_commands(handle: int) -> dict:
    """Return all quick command names available on the device."""
    device = _get_device(handle)
    count  = device.GetQuickCommandCount()
    return {"quick_commands": [device.GetQuickCommand(i) for i in range(count)]}


# ── Script variables ─────────────────────────────────────────────────────────

@mcp.tool()
def list_script_variables(handle: int) -> dict:
    """Return all script variable names for the device."""
    device = _get_device(handle)
    count  = device.GetScriptVariableCount()
    return {"script_variables": [device.GetScriptVariable(i) for i in range(count)]}


@mcp.tool()
def update_script_variable(handle: int, variable: str, value: str) -> dict:
    """Set the value of a named script variable on the device."""
    _get_device(handle).UpdateScriptVariableValue(variable, value)
    return {"success": True}


# ── Core command interface ───────────────────────────────────────────────────

@mcp.tool()
def get_command_state(handle: int, command: str) -> dict:
    """Return the current boolean state of a named command on the device."""
    return {"command": command, "state": _get_device(handle).GetCommandState(command)}


@mcp.tool()
def send_command(handle: int, command: str, state: bool) -> dict:
    """Send a named command with a boolean state to the device."""
    _get_device(handle).SendCommand(command, state)
    return {"success": True}


# ── Help ─────────────────────────────────────────────────────────────────────

@mcp.tool()
def get_help_text(handle: int) -> dict:
    """Return the full help text listing all commands supported by the device."""
    return {"help_text": _get_device(handle).GetHelpText()}


# ── Raw pin control ──────────────────────────────────────────────────────────

@mcp.tool()
def set_pin_state(handle: int, pin: int, state: bool) -> dict:
    """Set the raw boolean state of a hardware pin on the device."""
    _get_device(handle).SetPin(pin, state)
    return {"success": True}


# ── Command queue ────────────────────────────────────────────────────────────

@mcp.tool()
def is_command_queue_clear(handle: int) -> dict:
    """Return True if the device's command queue is empty."""
    return {"queue_clear": _get_device(handle).IsCommandQueueClear()}


# ── Power / peripheral boolean controls ─────────────────────────────────────

@mcp.tool()
def set_battery_state(handle: int, state: bool) -> dict:
    """Set the battery connection state (True = connected)."""
    _get_device(handle).SetBatteryState(state)
    return {"success": True}


@mcp.tool()
def get_battery_state(handle: int) -> dict:
    """Return the current battery connection state."""
    return {"state": _get_device(handle).GetBatteryState()}


@mcp.tool()
def set_usb0(handle: int, state: bool) -> dict:
    """Set the USB0 connection state (True = connected)."""
    _get_device(handle).Usb0(state)
    return {"success": True}


@mcp.tool()
def get_usb0_state(handle: int) -> dict:
    """Return the current USB0 connection state."""
    return {"state": _get_device(handle).GetUsb0State()}


@mcp.tool()
def set_usb1(handle: int, state: bool) -> dict:
    """Set the USB1 connection state (True = connected)."""
    _get_device(handle).Usb1(state)
    return {"success": True}


@mcp.tool()
def get_usb1_state(handle: int) -> dict:
    """Return the current USB1 connection state."""
    return {"state": _get_device(handle).GetUsb1State()}


@mcp.tool()
def set_power_key(handle: int, state: bool) -> dict:
    """Set the power key state (True = pressed)."""
    _get_device(handle).PowerKey(state)
    return {"success": True}


@mcp.tool()
def get_power_key_state(handle: int) -> dict:
    """Return the current power key state."""
    return {"state": _get_device(handle).GetPowerKeyState()}


@mcp.tool()
def set_volume_up(handle: int, state: bool) -> dict:
    """Set the volume-up button state (True = pressed)."""
    _get_device(handle).VolumeUp(state)
    return {"success": True}


@mcp.tool()
def get_volume_up_state(handle: int) -> dict:
    """Return the current volume-up button state."""
    return {"state": _get_device(handle).GetVolumeUpState()}


@mcp.tool()
def set_volume_down(handle: int, state: bool) -> dict:
    """Set the volume-down button state (True = pressed)."""
    _get_device(handle).VolumeDown(state)
    return {"success": True}


@mcp.tool()
def get_volume_down_state(handle: int) -> dict:
    """Return the current volume-down button state."""
    return {"state": _get_device(handle).GetVolumeDownState()}


@mcp.tool()
def set_disconnect_uim1(handle: int, state: bool) -> dict:
    """Set the UIM1 disconnect state (True = disconnected)."""
    _get_device(handle).DisconnectUIM1(state)
    return {"success": True}


@mcp.tool()
def get_disconnect_uim1_state(handle: int) -> dict:
    """Return the current UIM1 disconnect state."""
    return {"state": _get_device(handle).GetDisconnectUIM1State()}


@mcp.tool()
def set_disconnect_uim2(handle: int, state: bool) -> dict:
    """Set the UIM2 disconnect state (True = disconnected)."""
    _get_device(handle).DisconnectUIM2(state)
    return {"success": True}


@mcp.tool()
def get_disconnect_uim2_state(handle: int) -> dict:
    """Return the current UIM2 disconnect state."""
    return {"state": _get_device(handle).GetDisconnectUIM2State()}


@mcp.tool()
def set_disconnect_sd_card(handle: int, state: bool) -> dict:
    """Set the SD card disconnect state (True = disconnected)."""
    _get_device(handle).DisconnectSDCard(state)
    return {"success": True}


@mcp.tool()
def get_disconnect_sd_card_state(handle: int) -> dict:
    """Return the current SD card disconnect state."""
    return {"state": _get_device(handle).GetDisconnectSDCardState()}


@mcp.tool()
def set_primary_edl(handle: int, state: bool) -> dict:
    """Set the primary EDL (Emergency Download Mode) state."""
    _get_device(handle).PrimaryEDL(state)
    return {"success": True}


@mcp.tool()
def get_primary_edl_state(handle: int) -> dict:
    """Return the current primary EDL state."""
    return {"state": _get_device(handle).GetPrimaryEDLState()}


@mcp.tool()
def set_secondary_edl(handle: int, state: bool) -> dict:
    """Set the secondary EDL state."""
    _get_device(handle).SecondaryEDL(state)
    return {"success": True}


@mcp.tool()
def get_secondary_edl_state(handle: int) -> dict:
    """Return the current secondary EDL state."""
    return {"state": _get_device(handle).GetSecondaryEDLState()}


@mcp.tool()
def set_force_ps_hold_high(handle: int, state: bool) -> dict:
    """Force PS_HOLD high (True = forced high)."""
    _get_device(handle).ForcePSHoldHigh(state)
    return {"success": True}


@mcp.tool()
def get_force_ps_hold_high_state(handle: int) -> dict:
    """Return the current force PS_HOLD high state."""
    return {"state": _get_device(handle).GetForcePSHoldHighState()}


@mcp.tool()
def set_secondary_pm_resin_n(handle: int, state: bool) -> dict:
    """Set the secondary PM RESIN_N state."""
    _get_device(handle).SecondaryPM_RESIN_N(state)
    return {"success": True}


@mcp.tool()
def get_secondary_pm_resin_n_state(handle: int) -> dict:
    """Return the current secondary PM RESIN_N state."""
    return {"state": _get_device(handle).GetSecondaryPM_RESIN_NState()}


@mcp.tool()
def set_eud(handle: int, state: bool) -> dict:
    """Set the EUD (Enhanced USB Debugging) state."""
    _get_device(handle).Eud(state)
    return {"success": True}


@mcp.tool()
def get_eud_state(handle: int) -> dict:
    """Return the current EUD state."""
    return {"state": _get_device(handle).GetEUDState()}


@mcp.tool()
def set_headset_disconnect(handle: int, state: bool) -> dict:
    """Set the headset disconnect state (True = disconnected)."""
    _get_device(handle).HeadsetDisconnect(state)
    return {"success": True}


@mcp.tool()
def get_headset_disconnect_state(handle: int) -> dict:
    """Return the current headset disconnect state."""
    return {"state": _get_device(handle).GetHeadsetDisconnectState()}


# ── Name / reset count ───────────────────────────────────────────────────────

@mcp.tool()
def set_name(handle: int, new_name: str) -> dict:
    """Set a new human-readable name on the TAC device (persisted in firmware)."""
    _get_device(handle).SetName(new_name)
    return {"success": True}


@mcp.tool()
def get_reset_count(handle: int) -> dict:
    """Return the number of resets recorded by the TAC device."""
    return {"reset_count": _get_device(handle).GetResetCount()}


@mcp.tool()
def clear_reset_count(handle: int) -> dict:
    """Clear (zero) the reset counter on the TAC device."""
    _get_device(handle).ClearResetCount()
    return {"success": True}


# ── Button sequences ─────────────────────────────────────────────────────────

@mcp.tool()
def power_on_button(handle: int) -> dict:
    """Execute the power-on button sequence on the device."""
    _get_device(handle).PowerOnButton()
    return {"success": True}


@mcp.tool()
def power_off_button(handle: int) -> dict:
    """Execute the power-off button sequence on the device."""
    _get_device(handle).PowerOffButton()
    return {"success": True}


@mcp.tool()
def boot_to_fastboot_button(handle: int) -> dict:
    """Execute the button sequence to boot the device into Fastboot mode."""
    _get_device(handle).BootToFastBootButton()
    return {"success": True}


@mcp.tool()
def boot_to_uefi_menu_button(handle: int) -> dict:
    """Execute the button sequence to enter the UEFI menu."""
    _get_device(handle).BootToUEFIMenuButton()
    return {"success": True}


@mcp.tool()
def boot_to_edl_button(handle: int) -> dict:
    """Execute the button sequence to boot the device into primary EDL mode."""
    _get_device(handle).BootToEDLButton()
    return {"success": True}


@mcp.tool()
def boot_to_secondary_edl_button(handle: int) -> dict:
    """Execute the button sequence to boot the device into secondary EDL mode."""
    _get_device(handle).BootToSecondaryEDLButton()
    return {"success": True}


# --------------------------------------------------------------------------- #
# Entry point
# --------------------------------------------------------------------------- #
if __name__ == "__main__":
    mcp.run()
