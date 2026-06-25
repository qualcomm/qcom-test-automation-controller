#!/usr/bin/env python3

# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

import logging
import logging.handlers
from pathlib import Path

import yaml
import TACDev
from fastmcp import FastMCP, Context
from fastmcp.server.middleware import Middleware, MiddlewareContext, CallNext

import mcp.types as mt

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
device_ownership: dict[str, str] = {}
session_handles: dict[str, list[int]] = {}
handles: dict[int, TACDev.TACDevice] = {}
_next_handle = 1


def _register(device: TACDev.TACDevice, session_id: str) -> int:
    global _next_handle
    handle = _next_handle
    _next_handle += 1
    handles[handle] = device
    session_handles.setdefault(session_id, []).append(handle)
    return handle


def _get_device(handle: int) -> TACDev.TACDevice:
    device = handles.get(handle)
    if device is None:
        raise RuntimeError(f"Invalid handle {handle}.")
    return device


def _release_session(session_id: str) -> None:
    for handle in session_handles.pop(session_id, []):
        device = handles.pop(handle, None)
        if device:
            port = next((p for p, s in device_ownership.items() if s == session_id), None)
            device.Close()
            if port:
                del device_ownership[port]
            log.info("session=%s handle=%d auto-closed on disconnect", session_id, handle)


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
# Session connect middleware
# --------------------------------------------------------------------------- #
class SessionMiddleware(Middleware):
    async def on_initialize(
        self,
        context: MiddlewareContext[mt.InitializeRequest],
        call_next: CallNext,
    ):
        result = await call_next(context)
        ctx = context.fastmcp_context
        if ctx:
            session_id = ctx.session_id
            log.info("session=%s connected", session_id)
            devices = _device_list()
            await ctx.send_notification(
                mt.ResourceListChangedNotification(
                    method="notifications/resources/list_changed"
                )
            )
            log.info("session=%s device list pushed: %s", session_id, devices)
        return result


# --------------------------------------------------------------------------- #
# MCP server
# --------------------------------------------------------------------------- #
mcp = FastMCP("TACDev", middleware=[SessionMiddleware()])


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
    if port_name in device_ownership:
        owner = device_ownership[port_name]
        log.warning("session=%s tried to open %s already owned by %s", session_id, port_name, owner)
        raise RuntimeError(f"Device '{port_name}' is already in use by session {owner}.")
    count = TACDev.GetDeviceCount()
    for i in range(count):
        device = TACDev.GetDevice(i)
        if device and device.PortName() == port_name:
            if not device.Open():
                raise RuntimeError(f"Failed to open device '{port_name}'.")
            handle = _register(device, session_id)
            device_ownership[port_name] = session_id
            log.info("session=%s opened %s handle=%d", session_id, port_name, handle)
            return {"handle": handle}
    raise RuntimeError(f"No device with port name '{port_name}'.")


@mcp.tool()
async def close_tac_handle(handle: int, ctx: Context) -> dict:
    session_id = ctx.session_id
    device = _get_device(handle)
    port = next((p for p, s in device_ownership.items() if s == session_id), None)
    device.Close()
    del handles[handle]
    if handle in session_handles.get(session_id, []):
        session_handles[session_id].remove(handle)
    if port:
        del device_ownership[port]
    log.info("session=%s closed handle=%d port=%s", session_id, handle, port)
    return {"success": True}


# --------------------------------------------------------------------------- #
# Tools: device info
# --------------------------------------------------------------------------- #
@mcp.tool()
def get_name(handle: int) -> dict:
    return {"name": _get_device(handle).Get_Name()}


@mcp.tool()
def get_firmware_version(handle: int) -> dict:
    return {"firmware_version": _get_device(handle).GetFirmwareVersion()}


@mcp.tool()
def get_hardware(handle: int) -> dict:
    return {"hardware": _get_device(handle).GetHardware()}


@mcp.tool()
def get_hardware_version(handle: int) -> dict:
    return {"hardware_version": _get_device(handle).Get_HardwareVersion()}


@mcp.tool()
def get_uuid(handle: int) -> dict:
    return {"uuid": _get_device(handle).Get_UUID()}


# --------------------------------------------------------------------------- #
# Tools: external power
# --------------------------------------------------------------------------- #
@mcp.tool()
def set_external_power_control(handle: int, state: bool) -> dict:
    _get_device(handle).SetExternalPowerControl(state)
    return {"success": True}


# --------------------------------------------------------------------------- #
# Tools: dynamic commands
# --------------------------------------------------------------------------- #
@mcp.tool()
def list_commands(handle: int) -> dict:
    device = _get_device(handle)
    return {"commands": [device.GetCommand(i) for i in range(device.GetCommandCount())]}


@mcp.tool()
def get_command(handle: int, command_index: int) -> dict:
    return {"command": _get_device(handle).GetCommand(command_index)}


@mcp.tool()
def list_quick_commands(handle: int) -> dict:
    device = _get_device(handle)
    return {"quick_commands": [device.GetQuickCommand(i) for i in range(device.GetQuickCommandCount())]}


# --------------------------------------------------------------------------- #
# Tools: script variables
# --------------------------------------------------------------------------- #
@mcp.tool()
def list_script_variables(handle: int) -> dict:
    device = _get_device(handle)
    return {"script_variables": [device.GetScriptVariable(i) for i in range(device.GetScriptVariableCount())]}


@mcp.tool()
def update_script_variable(handle: int, variable: str, value: str) -> dict:
    _get_device(handle).UpdateScriptVariableValue(variable, value)
    return {"success": True}


# --------------------------------------------------------------------------- #
# Tools: core command interface
# --------------------------------------------------------------------------- #
@mcp.tool()
def get_command_state(handle: int, command: str) -> dict:
    return {"command": command, "state": _get_device(handle).GetCommandState(command)}


@mcp.tool()
def send_command(handle: int, command: str, state: bool) -> dict:
    _get_device(handle).SendCommand(command, state)
    return {"success": True}


# --------------------------------------------------------------------------- #
# Tools: help / queue
# --------------------------------------------------------------------------- #
@mcp.tool()
def get_help_text(handle: int) -> dict:
    return {"help_text": _get_device(handle).GetHelpText()}


@mcp.tool()
def is_command_queue_clear(handle: int) -> dict:
    return {"queue_clear": _get_device(handle).IsCommandQueueClear()}


# --------------------------------------------------------------------------- #
# Tools: raw pin
# --------------------------------------------------------------------------- #
@mcp.tool()
def set_pin_state(handle: int, pin: int, state: bool) -> dict:
    _get_device(handle).SetPin(pin, state)
    return {"success": True}


# --------------------------------------------------------------------------- #
# Tools: battery
# --------------------------------------------------------------------------- #
@mcp.tool()
def set_battery_state(handle: int, state: bool) -> dict:
    _get_device(handle).SetBatteryState(state)
    return {"success": True}


@mcp.tool()
def get_battery_state(handle: int) -> dict:
    return {"state": _get_device(handle).GetBatteryState()}


# --------------------------------------------------------------------------- #
# Tools: USB
# --------------------------------------------------------------------------- #
@mcp.tool()
def set_usb0(handle: int, state: bool) -> dict:
    _get_device(handle).Usb0(state)
    return {"success": True}


@mcp.tool()
def get_usb0_state(handle: int) -> dict:
    return {"state": _get_device(handle).GetUsb0State()}


@mcp.tool()
def set_usb1(handle: int, state: bool) -> dict:
    _get_device(handle).Usb1(state)
    return {"success": True}


@mcp.tool()
def get_usb1_state(handle: int) -> dict:
    return {"state": _get_device(handle).GetUsb1State()}


# --------------------------------------------------------------------------- #
# Tools: power key
# --------------------------------------------------------------------------- #
@mcp.tool()
def set_power_key(handle: int, state: bool) -> dict:
    _get_device(handle).PowerKey(state)
    return {"success": True}


@mcp.tool()
def get_power_key_state(handle: int) -> dict:
    return {"state": _get_device(handle).GetPowerKeyState()}


# --------------------------------------------------------------------------- #
# Tools: volume
# --------------------------------------------------------------------------- #
@mcp.tool()
def set_volume_up(handle: int, state: bool) -> dict:
    _get_device(handle).VolumeUp(state)
    return {"success": True}


@mcp.tool()
def get_volume_up_state(handle: int) -> dict:
    return {"state": _get_device(handle).GetVolumeUpState()}


@mcp.tool()
def set_volume_down(handle: int, state: bool) -> dict:
    _get_device(handle).VolumeDown(state)
    return {"success": True}


@mcp.tool()
def get_volume_down_state(handle: int) -> dict:
    return {"state": _get_device(handle).GetVolumeDownState()}


# --------------------------------------------------------------------------- #
# Tools: SIM / SD
# --------------------------------------------------------------------------- #
@mcp.tool()
def set_disconnect_uim1(handle: int, state: bool) -> dict:
    _get_device(handle).DisconnectUIM1(state)
    return {"success": True}


@mcp.tool()
def get_disconnect_uim1_state(handle: int) -> dict:
    return {"state": _get_device(handle).GetDisconnectUIM1State()}


@mcp.tool()
def set_disconnect_uim2(handle: int, state: bool) -> dict:
    _get_device(handle).DisconnectUIM2(state)
    return {"success": True}


@mcp.tool()
def get_disconnect_uim2_state(handle: int) -> dict:
    return {"state": _get_device(handle).GetDisconnectUIM2State()}


@mcp.tool()
def set_disconnect_sd_card(handle: int, state: bool) -> dict:
    _get_device(handle).DisconnectSDCard(state)
    return {"success": True}


@mcp.tool()
def get_disconnect_sd_card_state(handle: int) -> dict:
    return {"state": _get_device(handle).GetDisconnectSDCardState()}


# --------------------------------------------------------------------------- #
# Tools: EDL
# --------------------------------------------------------------------------- #
@mcp.tool()
def set_primary_edl(handle: int, state: bool) -> dict:
    _get_device(handle).PrimaryEDL(state)
    return {"success": True}


@mcp.tool()
def get_primary_edl_state(handle: int) -> dict:
    return {"state": _get_device(handle).GetPrimaryEDLState()}


@mcp.tool()
def set_secondary_edl(handle: int, state: bool) -> dict:
    _get_device(handle).SecondaryEDL(state)
    return {"success": True}


@mcp.tool()
def get_secondary_edl_state(handle: int) -> dict:
    return {"state": _get_device(handle).GetSecondaryEDLState()}


# --------------------------------------------------------------------------- #
# Tools: PS_HOLD / RESIN_N
# --------------------------------------------------------------------------- #
@mcp.tool()
def set_force_ps_hold_high(handle: int, state: bool) -> dict:
    _get_device(handle).ForcePSHoldHigh(state)
    return {"success": True}


@mcp.tool()
def get_force_ps_hold_high_state(handle: int) -> dict:
    return {"state": _get_device(handle).GetForcePSHoldHighState()}


@mcp.tool()
def set_secondary_pm_resin_n(handle: int, state: bool) -> dict:
    _get_device(handle).SecondaryPM_RESIN_N(state)
    return {"success": True}


@mcp.tool()
def get_secondary_pm_resin_n_state(handle: int) -> dict:
    return {"state": _get_device(handle).GetSecondaryPM_RESIN_NState()}


# --------------------------------------------------------------------------- #
# Tools: EUD
# --------------------------------------------------------------------------- #
@mcp.tool()
def set_eud(handle: int, state: bool) -> dict:
    _get_device(handle).Eud(state)
    return {"success": True}


@mcp.tool()
def get_eud_state(handle: int) -> dict:
    return {"state": _get_device(handle).GetEUDState()}


# --------------------------------------------------------------------------- #
# Tools: headset
# --------------------------------------------------------------------------- #
@mcp.tool()
def set_headset_disconnect(handle: int, state: bool) -> dict:
    _get_device(handle).HeadsetDisconnect(state)
    return {"success": True}


@mcp.tool()
def get_headset_disconnect_state(handle: int) -> dict:
    return {"state": _get_device(handle).GetHeadsetDisconnectState()}


# --------------------------------------------------------------------------- #
# Tools: name / reset count
# --------------------------------------------------------------------------- #
@mcp.tool()
def set_name(handle: int, new_name: str) -> dict:
    _get_device(handle).SetName(new_name)
    return {"success": True}


@mcp.tool()
def get_reset_count(handle: int) -> dict:
    return {"reset_count": _get_device(handle).GetResetCount()}


@mcp.tool()
def clear_reset_count(handle: int) -> dict:
    _get_device(handle).ClearResetCount()
    return {"success": True}


# --------------------------------------------------------------------------- #
# Tools: button sequences
# --------------------------------------------------------------------------- #
@mcp.tool()
def power_on_button(handle: int) -> dict:
    _get_device(handle).PowerOnButton()
    return {"success": True}


@mcp.tool()
def power_off_button(handle: int) -> dict:
    _get_device(handle).PowerOffButton()
    return {"success": True}


@mcp.tool()
def boot_to_fastboot_button(handle: int) -> dict:
    _get_device(handle).BootToFastBootButton()
    return {"success": True}


@mcp.tool()
def boot_to_uefi_menu_button(handle: int) -> dict:
    _get_device(handle).BootToUEFIMenuButton()
    return {"success": True}


@mcp.tool()
def boot_to_edl_button(handle: int) -> dict:
    _get_device(handle).BootToEDLButton()
    return {"success": True}


@mcp.tool()
def boot_to_secondary_edl_button(handle: int) -> dict:
    _get_device(handle).BootToSecondaryEDLButton()
    return {"success": True}


# --------------------------------------------------------------------------- #
# Entry point
# --------------------------------------------------------------------------- #
if __name__ == "__main__":
    server_cfg = cfg["server"]
    log.info("Starting TACDev MCP server on %s:%d", server_cfg["host"], server_cfg["port"])
    try:
        mcp.run(
            transport="sse",
            host=server_cfg["host"],
            port=server_cfg["port"],
        )
    except KeyboardInterrupt:
        pass
    finally:
        log.info("Server stopped.")
