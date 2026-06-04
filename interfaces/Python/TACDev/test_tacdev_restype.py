"""
Tests for TACDev.py ctypes function signature correctness.

These tests verify that OpenHandleByDescription has the correct restype (c_ulong)
so that TAC_HANDLE values are not truncated on 64-bit Linux where unsigned long
is 8 bytes but ctypes defaults to c_int (4 bytes).

No hardware or installed Alpaca is required to run these tests.
"""

import ctypes
import sys
import unittest
from unittest.mock import MagicMock, patch
from ctypes import c_ulong, c_char_p, CDLL


class MockDll:
    """Minimal mock of a loaded TACDev shared library."""

    def __init__(self):
        # Simulate DLL functions as ctypes-like objects
        self.OpenHandleByDescription = MagicMock()
        self.OpenHandleByDescription.argtypes = None
        self.OpenHandleByDescription.restype = None

        self.CloseTACHandle = MagicMock()
        self.CloseTACHandle.restype = None

        # All other functions used by SetupClassEntries
        for name in [
            "GetName", "GetFirmwareVersion", "GetHardware", "GetHardwareVersion",
            "GetUUID", "SetExternalPowerControl", "GetCommandCount", "GetCommand",
            "GetQuickCommandCount", "GetQuickCommand", "GetCommandState", "SendCommand",
            "SetPinState", "GetHelpText", "SetBatteryState", "GetBatteryState",
            "Usb0", "GetUsb0State", "Usb1", "GetUsb1State", "PowerKey",
            "GetPowerKeyState", "VolumeUp", "GetVolumeUpState", "VolumeDown",
            "GetVolumeDownState", "DisconnectUIM1", "GetDisconnectUIM1State",
            "DisconnectUIM2", "GetDisconnectUIM2State", "DisconnectSDCard",
            "GetDisconnectSDCardState", "PrimaryEDL", "GetPrimaryEDLState",
            "SecondaryEDL", "GetSecondaryEDLState", "ForcePSHoldHigh",
            "GetForcePSHoldHighState", "SecondaryPmResinN", "GetSecondaryPmResinNState",
            "Eud", "GetEUDState", "HeadsetDisconnect", "GetHeadsetDisconnectState",
            "SetName", "GetResetCount", "ClearResetCount", "PowerOnButton",
            "PowerOffButton", "BootToFastBootButton", "BootToUEFIMenuButton",
            "BootToEDLButton", "BootToSecondaryEDLButton", "GetScriptVariableCount",
            "GetScriptVariable", "UpdateScriptVariableValue", "IsCommandQueueClear",
        ]:
            setattr(self, name, MagicMock())


def _make_tacdevice_with_mock_dll():
    """
    Import TACDev module with the DLL load patched out, then call
    SetupClassEntries with a MockDll so we can inspect the configured
    function signatures.
    """
    mock_dll = MockDll()

    # Patch CDLL so the module-level import doesn't try to load a real DLL
    mock_cdll_instance = MagicMock()
    mock_cdll_instance.InitializeTACDev = MagicMock(return_value=0)
    mock_cdll_instance.GetAlpacaVersion = MagicMock()
    mock_cdll_instance.GetTACVersion = MagicMock()
    mock_cdll_instance.GetDeviceCount = MagicMock()
    mock_cdll_instance.GetPortData = MagicMock()
    mock_cdll_instance.GetLoggingState = MagicMock()
    mock_cdll_instance.SetLoggingState = MagicMock()

    with patch("ctypes.CDLL", return_value=mock_cdll_instance):
        # Force reimport so module-level code runs with the mock
        if "TACDev" in sys.modules:
            del sys.modules["TACDev"]
        import TACDev

    device = TACDev.TACDevice("COM1", "Test Device", "SN123")
    device.SetupClassEntries(mock_dll)
    return device, mock_dll


class TestOpenHandleByDescriptionRestype(unittest.TestCase):

    def setUp(self):
        self.device, self.mock_dll = _make_tacdevice_with_mock_dll()

    def test_open_handle_restype_is_c_ulong(self):
        """
        OpenHandleByDescription returns TAC_HANDLE = unsigned long.
        On Linux x64, unsigned long is 8 bytes. Without restype=c_ulong,
        ctypes defaults to c_int (4 bytes) and truncates the handle,
        corrupting it for all subsequent calls.
        """
        self.assertEqual(
            self.mock_dll.OpenHandleByDescription.restype,
            c_ulong,
            "OpenHandleByDescription.restype must be c_ulong to avoid handle "
            "truncation on 64-bit Linux (unsigned long = 8 bytes there)"
        )

    def test_open_handle_argtypes_is_c_char_p(self):
        """argtypes must be [c_char_p] so ctypes passes the port name correctly."""
        self.assertEqual(
            self.mock_dll.OpenHandleByDescription.argtypes,
            [c_char_p]
        )

    def test_close_handle_restype_is_c_ulong(self):
        """
        CloseTACHandle returns TAC_RESULT = unsigned long.
        Same truncation risk on Linux without restype=c_ulong.
        """
        self.assertEqual(
            self.mock_dll.CloseTACHandle.restype,
            c_ulong,
            "CloseTACHandle.restype must be c_ulong"
        )

    def test_bad_handle_constant_is_zero(self):
        """BAD_TAC_HANDLE must be 0 to match kBadHandle in TACDev.h."""
        import TACDev
        self.assertEqual(TACDev.BAD_TAC_HANDLE, 0)

    def test_open_returns_false_on_bad_handle(self):
        """
        If OpenHandleByDescription returns BAD_TAC_HANDLE (0), Open() must
        return False without raising.
        """
        self.mock_dll.OpenHandleByDescription.return_value = 0  # BAD_TAC_HANDLE
        result = self.device.Open()
        self.assertFalse(result)

    def test_open_returns_true_on_valid_handle(self):
        """
        If OpenHandleByDescription returns a non-zero handle, Open() must
        return True and store the handle.
        """
        # Use a handle value that would be truncated without the restype fix:
        # 0x1_0000_0001 has bits above bit 31 set; with c_int restype it
        # would be truncated to 0x0000_0001 (still non-zero here, but the
        # stored value would be wrong for subsequent calls).
        valid_handle = 0x100000001
        self.mock_dll.OpenHandleByDescription.return_value = valid_handle
        result = self.device.Open()
        self.assertTrue(result)


if __name__ == "__main__":
    unittest.main(verbosity=2)