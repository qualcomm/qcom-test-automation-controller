#!/usr/bin/env python3

# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

import asyncio
import logging
import logging.handlers
from pathlib import Path

import yaml
import TACDev
from fastmcp import FastMCP, Context
from fastmcp.server.middleware import Middleware, MiddlewareContext, CallNext

# --------------------------------------------------------------------------- #
# Config
# --------------------------------------------------------------------------- #
_CONFIG_PATH = Path(__file__).with_name("config.yaml")

def _load_config() -> dict:
    with open(_CONFIG_PATH) as f:
        return yaml.safe_load(f)

cfg = _load_config()

# --------------------------------------------------------------------------- #
# Logging
# --------------------------------------------------------------------------- #
def _setup_logging() -> logging.Logger:
    log_cfg = cfg["logging"]
    logger = logging.getLogger("tacdev_mcp")
    logger.setLevel(log_cfg["level"].upper())

    handler = logging.handlers.RotatingFileHandler(
        log_cfg["file"],
        maxBytes=log_cfg["max_bytes"],
        backupCount=log_cfg["backup_count"],
    )
    handler.setFormatter(logging.Formatter("%(asctime)s %(levelname)s %(message)s"))
    logger.addHandler(handler)
    return logger

log = _setup_logging()

# --------------------------------------------------------------------------- #
# Device ownership registry
# --------------------------------------------------------------------------- #
_lock = asyncio.Lock()

device_ownership: dict[str, str] = {}
session_handles: dict[str, list[int]] = {}
handles: dict[int, tuple[TACDev.TACDevice, str, str]] = {}  # handle -> (device, port, session_id)
_next_handle = 1


def _register(device: TACDev.TACDevice, port: str, session_id: str) -> int:
    global _next_handle
    handle = _next_handle
    _next_handle += 1
    handles[handle] = (device, port, session_id)
    session_handles.setdefault(session_id, []).append(handle)
    return handle


def _get_device_owned(handle: int, session_id: str) -> TACDev.TACDevice:
    entry = handles.get(handle)
    if entry is None:
        raise RuntimeError(f"Invalid handle {handle}.")
    device, port, owner = entry
    if owner != session_id:
        raise RuntimeError(f"Handle {handle} is owned by a different session.")
    return device


def _release_all() -> None:
    log.info("Closing all open devices before shutdown...")
    for handle, entry in list(handles.items()):
        device, port, session_id = entry
        try:
            device.Close()
            log.info("session=%s handle=%d port=%s closed on shutdown", session_id, handle, port)
        except Exception as e:
            log.warning("session=%s handle=%d port=%s failed to close on shutdown: %s", session_id, handle, port, e)
    handles.clear()
    session_handles.clear()
    device_ownership.clear()
    log.info("All devices released.")


def _device_list() -> dict:
    count = TACDev.GetDeviceCount()
    available = []
    in_use = []
    for i in range(count):
        d = TACDev.GetDevice(i)
        if not d:
            continue
        port = d.PortName()
        entry = {"port": port, "description": d.Description(), "serial": d.SerialNumber()}
        if port in device_ownership:
            in_use.append({**entry, "owned_by": device_ownership[port]})
        else:
            available.append(entry)
    return {"available": available, "in_use": in_use}


# --------------------------------------------------------------------------- #
# Session lifecycle middleware
# --------------------------------------------------------------------------- #
class SessionMiddleware(Middleware):
    async def on_initialize(
        self,
        context: MiddlewareContext,
        call_next: CallNext,
    ):
        result = await call_next(context)
        ctx = context.fastmcp_context
        if ctx:
            log.info("session=%s connected", ctx.session_id)
        return result


# --------------------------------------------------------------------------- #
# MCP server
# --------------------------------------------------------------------------- #
mcp = FastMCP("TACDev-MCP", middleware=[SessionMiddleware()])


# --------------------------------------------------------------------------- #
# Tools: device list
# --------------------------------------------------------------------------- #
@mcp.tool()
def list_devices() -> dict:
    return _device_list()


# --------------------------------------------------------------------------- #
# Tools: diagnostics
# --------------------------------------------------------------------------- #
@mcp.tool()
def get_alpaca_version() -> dict:
    return {"alpaca_version": TACDev.AlpacaVersion()}


@mcp.tool()
def get_tac_version() -> dict:
    return {"tac_version": TACDev.TACVersion()}


@mcp.tool()
def get_last_tac_error() -> dict:
    return {"last_error": TACDev.GetLastError()}


# --------------------------------------------------------------------------- #
# Tools: logging
# --------------------------------------------------------------------------- #
@mcp.tool()
def get_logging_state() -> dict:
    return {"logging_enabled": TACDev.GetLoggingState()}


@mcp.tool()
def set_logging_state(enabled: bool) -> dict:
    TACDev.SetLoggingState(enabled)
    return {"success": True}


# --------------------------------------------------------------------------- #
# Tools: device enumeration
# --------------------------------------------------------------------------- #
@mcp.tool()
def get_device_count() -> dict:
    return {"device_count": TACDev.GetDeviceCount()}


@mcp.tool()
def get_port_data(device_index: int) -> dict:
    device = TACDev.GetDevice(device_index)
    if device is None:
        raise RuntimeError(f"No device at index {device_index}.")
    return {"port_data": device.PortName(), "description": device.Description(), "serial_number": device.SerialNumber()}


# --------------------------------------------------------------------------- #
# Tools: handle management
# --------------------------------------------------------------------------- #
@mcp.tool()
async def open_handle_by_description(port_name: str, ctx: Context) -> dict:
    session_id = ctx.session_id
    async with _lock:
        if port_name in device_ownership:
            owner = device_ownership[port_name]
            log.warning("session=%s tried to open %s already owned by %s", session_id, port_name, owner)
            raise RuntimeError(
                f"Device '{port_name}' is already in use by session {owner}. "
                f"If that session crashed, restart the server (Ctrl+C) to release all devices."
            )
        count = TACDev.GetDeviceCount()
        for i in range(count):
            device = TACDev.GetDevice(i)
            if device and device.PortName() == port_name:
                if not device.Open():
                    raise RuntimeError(f"Failed to open device '{port_name}'.")
                handle = _register(device, port_name, session_id)
                device_ownership[port_name] = session_id
                log.info("session=%s opened %s handle=%d", session_id, port_name, handle)
                return {"handle": handle}
    raise RuntimeError(f"No device with port name '{port_name}'.")


@mcp.tool()
async def close_tac_handle(handle: int, ctx: Context) -> dict:
    session_id = ctx.session_id
    async with _lock:
        entry = handles.get(handle)
        if entry is None:
            raise RuntimeError(f"Invalid handle {handle}.")
        device, port, owner = entry
        if owner != session_id:
            raise RuntimeError(f"Handle {handle} is owned by a different session.")
        device.Close()
        del handles[handle]
        if handle in session_handles.get(session_id, []):
            session_handles[session_id].remove(handle)
        del device_ownership[port]
    log.info("session=%s closed handle=%d port=%s", session_id, handle, port)
    return {"success": True}


# --------------------------------------------------------------------------- #
# Tools: device info
# --------------------------------------------------------------------------- #
@mcp.tool()
async def get_name(handle: int, ctx: Context) -> dict:
    return {"name": _get_device_owned(handle, ctx.session_id).Get_Name()}


@mcp.tool()
async def get_firmware_version(handle: int, ctx: Context) -> dict:
    return {"firmware_version": _get_device_owned(handle, ctx.session_id).GetFirmwareVersion()}


@mcp.tool()
async def get_hardware(handle: int, ctx: Context) -> dict:
    return {"hardware": _get_device_owned(handle, ctx.session_id).GetHardware()}


@mcp.tool()
async def get_hardware_version(handle: int, ctx: Context) -> dict:
    return {"hardware_version": _get_device_owned(handle, ctx.session_id).Get_HardwareVersion()}


@mcp.tool()
async def get_uuid(handle: int, ctx: Context) -> dict:
    return {"uuid": _get_device_owned(handle, ctx.session_id).Get_UUID()}


# --------------------------------------------------------------------------- #
# Tools: external power
# --------------------------------------------------------------------------- #
@mcp.tool()
async def set_external_power_control(handle: int, state: bool, ctx: Context) -> dict:
    _get_device_owned(handle, ctx.session_id).SetExternalPowerControl(state)
    return {"success": True}


# --------------------------------------------------------------------------- #
# Tools: dynamic commands
# --------------------------------------------------------------------------- #
@mcp.tool()
async def list_commands(handle: int, ctx: Context) -> dict:
    device = _get_device_owned(handle, ctx.session_id)
    return {"commands": [device.GetCommand(i) for i in range(device.GetCommandCount())]}


@mcp.tool()
async def get_command(handle: int, command_index: int, ctx: Context) -> dict:
    return {"command": _get_device_owned(handle, ctx.session_id).GetCommand(command_index)}


@mcp.tool()
async def list_quick_commands(handle: int, ctx: Context) -> dict:
    device = _get_device_owned(handle, ctx.session_id)
    return {"quick_commands": [device.GetQuickCommand(i) for i in range(device.GetQuickCommandCount())]}


# --------------------------------------------------------------------------- #
# Tools: script variables
# --------------------------------------------------------------------------- #
@mcp.tool()
async def list_script_variables(handle: int, ctx: Context) -> dict:
    device = _get_device_owned(handle, ctx.session_id)
    return {"script_variables": [device.GetScriptVariable(i) for i in range(device.GetScriptVariableCount())]}


@mcp.tool()
async def update_script_variable(handle: int, variable: str, value: str, ctx: Context) -> dict:
    _get_device_owned(handle, ctx.session_id).UpdateScriptVariableValue(variable, value)
    return {"success": True}


# --------------------------------------------------------------------------- #
# Tools: core command interface
# --------------------------------------------------------------------------- #
@mcp.tool()
async def get_command_state(handle: int, command: str, ctx: Context) -> dict:
    return {"command": command, "state": _get_device_owned(handle, ctx.session_id).GetCommandState(command)}


@mcp.tool()
async def send_command(handle: int, command: str, state: bool, ctx: Context) -> dict:
    _get_device_owned(handle, ctx.session_id).SendCommand(command, state)
    return {"success": True}


# --------------------------------------------------------------------------- #
# Tools: help / queue
# --------------------------------------------------------------------------- #
@mcp.tool()
async def get_help_text(handle: int, ctx: Context) -> dict:
    return {"help_text": _get_device_owned(handle, ctx.session_id).GetHelpText()}


@mcp.tool()
async def is_command_queue_clear(handle: int, ctx: Context) -> dict:
    return {"queue_clear": _get_device_owned(handle, ctx.session_id).IsCommandQueueClear()}


# --------------------------------------------------------------------------- #
# Tools: raw pin
# --------------------------------------------------------------------------- #
@mcp.tool()
async def set_pin_state(handle: int, pin: int, state: bool, ctx: Context) -> dict:
    _get_device_owned(handle, ctx.session_id).SetPin(pin, state)
    return {"success": True}


# --------------------------------------------------------------------------- #
# Tools: battery
# --------------------------------------------------------------------------- #
@mcp.tool()
async def set_battery_state(handle: int, state: bool, ctx: Context) -> dict:
    _get_device_owned(handle, ctx.session_id).SetBatteryState(state)
    return {"success": True}


@mcp.tool()
async def get_battery_state(handle: int, ctx: Context) -> dict:
    return {"state": _get_device_owned(handle, ctx.session_id).GetBatteryState()}


# --------------------------------------------------------------------------- #
# Tools: USB
# --------------------------------------------------------------------------- #
@mcp.tool()
async def set_usb0(handle: int, state: bool, ctx: Context) -> dict:
    _get_device_owned(handle, ctx.session_id).Usb0(state)
    return {"success": True}


@mcp.tool()
async def get_usb0_state(handle: int, ctx: Context) -> dict:
    return {"state": _get_device_owned(handle, ctx.session_id).GetUsb0State()}


@mcp.tool()
async def set_usb1(handle: int, state: bool, ctx: Context) -> dict:
    _get_device_owned(handle, ctx.session_id).Usb1(state)
    return {"success": True}


@mcp.tool()
async def get_usb1_state(handle: int, ctx: Context) -> dict:
    return {"state": _get_device_owned(handle, ctx.session_id).GetUsb1State()}


# --------------------------------------------------------------------------- #
# Tools: power key
# --------------------------------------------------------------------------- #
@mcp.tool()
async def set_power_key(handle: int, state: bool, ctx: Context) -> dict:
    _get_device_owned(handle, ctx.session_id).PowerKey(state)
    return {"success": True}


@mcp.tool()
async def get_power_key_state(handle: int, ctx: Context) -> dict:
    return {"state": _get_device_owned(handle, ctx.session_id).GetPowerKeyState()}


# --------------------------------------------------------------------------- #
# Tools: volume
# --------------------------------------------------------------------------- #
@mcp.tool()
async def set_volume_up(handle: int, state: bool, ctx: Context) -> dict:
    _get_device_owned(handle, ctx.session_id).VolumeUp(state)
    return {"success": True}


@mcp.tool()
async def get_volume_up_state(handle: int, ctx: Context) -> dict:
    return {"state": _get_device_owned(handle, ctx.session_id).GetVolumeUpState()}


@mcp.tool()
async def set_volume_down(handle: int, state: bool, ctx: Context) -> dict:
    _get_device_owned(handle, ctx.session_id).VolumeDown(state)
    return {"success": True}


@mcp.tool()
async def get_volume_down_state(handle: int, ctx: Context) -> dict:
    return {"state": _get_device_owned(handle, ctx.session_id).GetVolumeDownState()}


# --------------------------------------------------------------------------- #
# Tools: SIM / SD
# --------------------------------------------------------------------------- #
@mcp.tool()
async def set_disconnect_uim1(handle: int, state: bool, ctx: Context) -> dict:
    _get_device_owned(handle, ctx.session_id).DisconnectUIM1(state)
    return {"success": True}


@mcp.tool()
async def get_disconnect_uim1_state(handle: int, ctx: Context) -> dict:
    return {"state": _get_device_owned(handle, ctx.session_id).GetDisconnectUIM1State()}


@mcp.tool()
async def set_disconnect_uim2(handle: int, state: bool, ctx: Context) -> dict:
    _get_device_owned(handle, ctx.session_id).DisconnectUIM2(state)
    return {"success": True}


@mcp.tool()
async def get_disconnect_uim2_state(handle: int, ctx: Context) -> dict:
    return {"state": _get_device_owned(handle, ctx.session_id).GetDisconnectUIM2State()}


@mcp.tool()
async def set_disconnect_sd_card(handle: int, state: bool, ctx: Context) -> dict:
    _get_device_owned(handle, ctx.session_id).DisconnectSDCard(state)
    return {"success": True}


@mcp.tool()
async def get_disconnect_sd_card_state(handle: int, ctx: Context) -> dict:
    return {"state": _get_device_owned(handle, ctx.session_id).GetDisconnectSDCardState()}


# --------------------------------------------------------------------------- #
# Tools: EDL
# --------------------------------------------------------------------------- #
@mcp.tool()
async def set_primary_edl(handle: int, state: bool, ctx: Context) -> dict:
    _get_device_owned(handle, ctx.session_id).PrimaryEDL(state)
    return {"success": True}


@mcp.tool()
async def get_primary_edl_state(handle: int, ctx: Context) -> dict:
    return {"state": _get_device_owned(handle, ctx.session_id).GetPrimaryEDLState()}


@mcp.tool()
async def set_secondary_edl(handle: int, state: bool, ctx: Context) -> dict:
    _get_device_owned(handle, ctx.session_id).SecondaryEDL(state)
    return {"success": True}


@mcp.tool()
async def get_secondary_edl_state(handle: int, ctx: Context) -> dict:
    return {"state": _get_device_owned(handle, ctx.session_id).GetSecondaryEDLState()}


# --------------------------------------------------------------------------- #
# Tools: PS_HOLD / RESIN_N
# --------------------------------------------------------------------------- #
@mcp.tool()
async def set_force_ps_hold_high(handle: int, state: bool, ctx: Context) -> dict:
    _get_device_owned(handle, ctx.session_id).ForcePSHoldHigh(state)
    return {"success": True}


@mcp.tool()
async def get_force_ps_hold_high_state(handle: int, ctx: Context) -> dict:
    return {"state": _get_device_owned(handle, ctx.session_id).GetForcePSHoldHighState()}


@mcp.tool()
async def set_secondary_pm_resin_n(handle: int, state: bool, ctx: Context) -> dict:
    _get_device_owned(handle, ctx.session_id).SecondaryPM_RESIN_N(state)
    return {"success": True}


@mcp.tool()
async def get_secondary_pm_resin_n_state(handle: int, ctx: Context) -> dict:
    return {"state": _get_device_owned(handle, ctx.session_id).GetSecondaryPM_RESIN_NState()}


# --------------------------------------------------------------------------- #
# Tools: EUD
# --------------------------------------------------------------------------- #
@mcp.tool()
async def set_eud(handle: int, state: bool, ctx: Context) -> dict:
    _get_device_owned(handle, ctx.session_id).Eud(state)
    return {"success": True}


@mcp.tool()
async def get_eud_state(handle: int, ctx: Context) -> dict:
    return {"state": _get_device_owned(handle, ctx.session_id).GetEUDState()}


# --------------------------------------------------------------------------- #
# Tools: headset
# --------------------------------------------------------------------------- #
@mcp.tool()
async def set_headset_disconnect(handle: int, state: bool, ctx: Context) -> dict:
    _get_device_owned(handle, ctx.session_id).HeadsetDisconnect(state)
    return {"success": True}


@mcp.tool()
async def get_headset_disconnect_state(handle: int, ctx: Context) -> dict:
    return {"state": _get_device_owned(handle, ctx.session_id).GetHeadsetDisconnectState()}


# --------------------------------------------------------------------------- #
# Tools: name / reset count
# --------------------------------------------------------------------------- #
@mcp.tool()
async def set_name(handle: int, new_name: str, ctx: Context) -> dict:
    _get_device_owned(handle, ctx.session_id).SetName(new_name)
    return {"success": True}


@mcp.tool()
async def get_reset_count(handle: int, ctx: Context) -> dict:
    return {"reset_count": _get_device_owned(handle, ctx.session_id).GetResetCount()}


@mcp.tool()
async def clear_reset_count(handle: int, ctx: Context) -> dict:
    _get_device_owned(handle, ctx.session_id).ClearResetCount()
    return {"success": True}


# --------------------------------------------------------------------------- #
# Tools: button sequences
# --------------------------------------------------------------------------- #
@mcp.tool()
async def power_on_button(handle: int, ctx: Context) -> dict:
    _get_device_owned(handle, ctx.session_id).PowerOnButton()
    return {"success": True}


@mcp.tool()
async def power_off_button(handle: int, ctx: Context) -> dict:
    _get_device_owned(handle, ctx.session_id).PowerOffButton()
    return {"success": True}


@mcp.tool()
async def boot_to_fastboot_button(handle: int, ctx: Context) -> dict:
    _get_device_owned(handle, ctx.session_id).BootToFastBootButton()
    return {"success": True}


@mcp.tool()
async def boot_to_uefi_menu_button(handle: int, ctx: Context) -> dict:
    _get_device_owned(handle, ctx.session_id).BootToUEFIMenuButton()
    return {"success": True}


@mcp.tool()
async def boot_to_edl_button(handle: int, ctx: Context) -> dict:
    _get_device_owned(handle, ctx.session_id).BootToEDLButton()
    return {"success": True}


@mcp.tool()
async def boot_to_secondary_edl_button(handle: int, ctx: Context) -> dict:
    _get_device_owned(handle, ctx.session_id).BootToSecondaryEDLButton()
    return {"success": True}


# --------------------------------------------------------------------------- #
# Entry point
# --------------------------------------------------------------------------- #
if __name__ == "__main__":
    server_cfg = cfg["server"]
    log.info("Starting TACDev MCP server on %s:%d", server_cfg["host"], server_cfg["port"])
    log.info("Tip: if a client crashes and leaves a device locked, press Ctrl+C to restart the server — all devices will be released cleanly on shutdown.")
    try:
        mcp.run(
            transport="sse",
            host=server_cfg["host"],
            port=server_cfg["port"],
        )
    except KeyboardInterrupt:
        pass
    finally:
        _release_all()
        log.info("Server stopped.")
