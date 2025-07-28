# TACDev Python Library
# 	Written by Michael Simpson (msimpson@qti.qualcomm.com) and edited by Biswajit Roy (biswroy@qti.qualcomm.com)
# 	Special thanks to Brian Kahne (bkahne@qti.qualcomm.com) for giving me a Python education
# 	Don't modify this file.  If you do, you run the risk of missing out on bug fixes and product updates

import logging
import os
import time
from ctypes import *
from pathlib import Path
from sys import exit, platform

# Configure logging
logger = logging.getLogger(__name__)
log_fmt: str = logging.BASIC_FORMAT
logging.basicConfig(format=log_fmt, level=logging.INFO)


class _SetupTAC:
    """
    Internal class containing methods to configure shared library paths to TAC.
    """

    # The path to the TAC shared library
    __tacLibraryPath: Path = None
    # The path to the debug python dll
    __debugPythonPath: Path = Path("C:/github/AlpacaRepos/__Builds/x64/Debug/bin/PythonDebug.dll")
    # Whether the current execution for debugging
    __isDebugExecution: bool = False

    def __init__(self) -> None:
        self.setupSharedLibraryPath()
        logger.debug(f"Configured the TAC library path to be: {self.__tacLibraryPath.as_posix()}")

        if self.__isDebugExecution:
            logger.info(f"Process ID: {os.getpid()}")

    def PythonIsDebugging(self) -> bool:
        """
        Debug function to update the dll path to a debug dll.
        """
        if self.__debugPythonPath.exists():
            try:
                __pythonDebugLib = CDLL(self.__debugPythonPath.as_posix())
                __isPythonDebuggingFunc = __pythonDebugLib.IsPythonDebugging
                self.__isDebugExecution = __isPythonDebuggingFunc()
                return self.__isDebugExecution

            except Exception as error:
                logger.error(f"Could not load the 'IsPythonDebugging()' from PythonDebug shared library. {error}")
                exit(1)
        return False

    def setupSharedLibraryPath(self):
        """
        Configures the shared library path for TAC based on OS and QTAC installation
        """
        debugLinuxLibraryPath: Path = Path("/local/mnt/workspace/github/AlpacaRepos/__Builds/Linux/Debug/lib/libTACDev.so")
        debugWindowsLibraryPath: Path = Path("C:/github/AlpacaRepos/__Builds/x64/Debug/bin/TACDevd.dll")
        linuxLibraryPath: Path = Path("/opt/qcom/QTAC/lib/libTACDev.so")
        windowsLibraryPath: Path = Path("C:/Program Files (x86)/Qualcomm/QTAC/TACDev.dll")

        pythonIsDebugging = self.PythonIsDebugging()
        currentPlatform = platform

        if currentPlatform.startswith("linux") and pythonIsDebugging:
            self.__tacLibraryPath = debugLinuxLibraryPath

        elif platform.startswith("win32") and pythonIsDebugging:
            self.__tacLibraryPath = debugWindowsLibraryPath

        elif currentPlatform.startswith("linux") and not pythonIsDebugging:
            self.__tacLibraryPath = linuxLibraryPath

        elif currentPlatform.startswith("win32") and not pythonIsDebugging:
            self.__tacLibraryPath = windowsLibraryPath


    def getTACLibraryPath(self) -> str:
        """
        Returns the appropriate TAC library path as string.
        """
        return self.__tacLibraryPath.as_posix()


# TAC HANDLE
BAD_TAC_HANDLE = 0

# TAC RESULTS
NO_TAC_ERROR = 0
TAC_BUFFER_TOO_SMALL = 1
TAC_BAD_TAC_HANDLE = 2
TACDEV_COMMAND_NOT_FOUND = 3
TACDEV_BAD_INDEX = 4
TACDEV_INIT_FAILED = 5


def GetErrorString(errorCode: int) -> str:
    errorDict: dict[int,str] = {
        0: "No TAC error",
        1: "TAC buffer is too small",
        2: "Bad TAC handle. Is the device open?",
        3: "TAC command not found",
        4: "Bad TAC index",
        5: "TAC initialization failed",
        6: "TAC Script variable was not found",
    }
    return errorDict.get(errorCode, f"Error string cannot be determined. Error code: {errorCode}")


global __tacLib
global __alpacaVersionFunc
global __tacVersionFunc
global __getDeviceCountFunc
global __getDeviceFunc

# the path to the EPMDev shared library
tacLibraryPath = _SetupTAC().getTACLibraryPath()

if tacLibraryPath is not None:
    try:
        tacLibrary = CDLL(tacLibraryPath)

        if not tacLibrary:
            raise RuntimeError("TACDev not found")

        initializeFunc = tacLibrary.InitializeTACDev
        initResult = initializeFunc()
        if initResult == TACDEV_INIT_FAILED:
            logger.error("TACDev failed to initialize. Please report this issue to support.")
            exit(1)

        __alpacaVersionFunc = tacLibrary.GetAlpacaVersion
        __tacVersionFunc = tacLibrary.GetTACVersion
        __getDeviceCountFunc = tacLibrary.GetDeviceCount
        __getDeviceFunc = tacLibrary.GetPortData

    except Exception as error:
        logger.error(f"Error with TACDev shared Object. {error}")
        exit(1)


class TacDevice:
    def __init__(self, portName, desription, serialNumber):
        self.__portName = portName
        self.__description = desription
        self.__serialNumber = serialNumber

    def PortName(self) -> str:
        return self.__portName

    def Description(self) -> str:
        return self.__description

    def SerialNumber(self) -> str:
        return self.__serialNumber

    def Valid(self) -> bool:
        return self.__tacHandle != BAD_TAC_HANDLE

    def Open(self) -> bool:
        if self.__portName:
            portName = self.__portName.encode("utf-8")
            self.__tacHandle = self.__openByNameFunc(portName)

        if self.__tacHandle == BAD_TAC_HANDLE:
            if self.__serialNumber:
                serialNumber = self.__serialNumber.encode("utf-8")
                self.__tacHandle = self.__openByNameFunc(serialNumber)

        if self.__tacHandle != BAD_TAC_HANDLE:
            return True

        return False

    def Close(self) -> None:
        self.__tacHandle = self.__closeFunc(self.__tacHandle)

    def Get_Name(self) -> str:
        name = "Error: Port is closed"
        if self.__tacHandle != None:
            buf = create_string_buffer(1024)
            self.__nameFunc(self.__tacHandle, buf, sizeof(buf))
            name = buf.value.decode("cp1252")
        return name

    def GetFirmwareVersion(self):
        buf = create_string_buffer(1024)
        rc = self.__firmwareVersionFunc(self.__tacHandle, buf, sizeof(buf))
        if rc != 0:
            raise RuntimeError(f"GetFirmwareVersion returned: {GetErrorString(rc)}")
        return buf.value.decode("cp1252")

    def GetHardware(self):
        buf = create_string_buffer(1024)
        rc = self.__hardwareFunc(self.__tacHandle, buf, sizeof(buf))
        if rc != 0:
            raise RuntimeError(f"Error: GetHardware returned {GetErrorString(rc)}")
        return buf.value.decode("cp1252")

    def Get_HardwareVersion(self):
        buf = create_string_buffer(1024)
        rc = self.__hardwareVersionFunc(self.__tacHandle, buf, sizeof(buf))
        if rc != 0:
            raise RuntimeError(f"Error: Get_HardwareVersion returned {GetErrorString(rc)}")
        return buf.value.decode("cp1252")

    def Get_UUID(self):
        buf = create_string_buffer(1024)
        rc = self.__uuidFunc(self.__tacHandle, buf, sizeof(buf))
        if rc != 0:
            raise RuntimeError(f"Error: Get_UUID returned {GetErrorString(rc)}")
        return buf.value.decode("cp1252")

    def Set_Button_Assert_Time(self, secondsToAssert):
        raise RuntimeError(f"Set_Button_Assert_Time deprecated.  This is now in the configurations")

    def Get_Button_Assert_Time(self):
        raise RuntimeError(f"Get_Button_Assert_Time deprecated.  This is now in the configurations")

    def SetExternalPowerControl(self, state):
        result = self.__setExternalPowerControlFunc(self.__tacHandle, state)
        return result

    def GetCommandCount(self):
        commandCount = c_int(0)
        result = self.__getCommandCount(self.__tacHandle, byref(commandCount))
        if result != NO_TAC_ERROR:
            raise RuntimeError(f"Error: GetCommandCount returned {GetErrorString(result)}")
        return commandCount.value
    
    def GetCommand(self, commandIndex):
        buf = create_string_buffer(1024)
        result = self.__getCommand(self.__tacHandle, commandIndex, buf, sizeof(buf))
        if result != NO_TAC_ERROR:
            raise RuntimeError(f"Error: GetCommand returned {GetErrorString(result)}")
        return buf.value.decode("cp1252")
    
    def GetQuickCommandCount(self):
        commandCount = c_int(0)
        result = self.__getQuickCommandCount(self.__tacHandle, byref(commandCount))
        if result != NO_TAC_ERROR:
            raise RuntimeError(f"Error: GetQuickCommandCount returned {GetErrorString(result)}")
        return commandCount.value
    
    def GetQuickCommand(self, commandIndex):
        buf = create_string_buffer(1024)
        result = self.__getQuickCommand(self.__tacHandle, commandIndex, buf, sizeof(buf))
        if result != NO_TAC_ERROR:
            raise RuntimeError(f"Error: GetQuickCommand returned {GetErrorString(result)}")
        return buf.value.decode("cp1252")

    def GetCommandState(self, command):
        commandStr = command.encode("utf-8")
        state = c_bool(False)
        result = self.__getCommandState(self.__tacHandle, commandStr, byref(state))
        if result != NO_TAC_ERROR:
            raise RuntimeError(f"Error: GetCommandState returned {GetErrorString(result)}")
        return state.value

    def SendCommand(self, command, state):
        commandStr = command.encode("utf-8")
        result = self.__sendCommand(self.__tacHandle, commandStr, state)
        if result != NO_TAC_ERROR:
            raise RuntimeError(f"Error: SendCommand returned {GetErrorString(result)}")

    def SetPin(self, pin, state):
        result = self.__setPin(self.__tacHandle, pin, state)
        if result != NO_TAC_ERROR:
            raise RuntimeError(f"Error: SetPin returned {GetErrorString(result)}")

    def GetHelpText(self):
        buf = create_string_buffer(1)
        actualBufferSize = c_int(0)
        result = self.__helpText(self.__tacHandle, buf, sizeof(buf), byref(actualBufferSize))
        buf = create_string_buffer(actualBufferSize.value + 1)
        result = self.__helpText(self.__tacHandle, buf, sizeof(buf), byref(actualBufferSize))
        if result != NO_TAC_ERROR:
           raise RuntimeError(f"Error: GetHelpText returned {GetErrorString(result)}")
        return fr'{buf.value.decode("cp1252")}'

    def SetBatteryState(self, state):
        result = self.__setBatteryStateFunc(self.__tacHandle, state)
        if result != NO_TAC_ERROR:
            raise RuntimeError(f"Error: SetBatteryState returned {GetErrorString(result)}")

    def GetBatteryState(self):
        state = c_bool(False)
        result = self.__getBatteryStateFunc(self.__tacHandle, byref(state))
        if result != NO_TAC_ERROR:
            raise RuntimeError(f"Error: GetBatteryState returned {GetErrorString(result)}")
        return state.value

    def Usb0(self, state):
        result = self.__usb0Func(self.__tacHandle, state)
        if result != NO_TAC_ERROR:
            raise RuntimeError(f"Error: Usb0 returned {GetErrorString(result)}")

    def GetUsb0State(self):
        state = c_bool(False)
        rc = self.__getUsb0Func(self.__tacHandle, byref(state))
        if rc != 0:
            raise RuntimeError(f"Error: GetUsb0State returned {GetErrorString(rc)}")
        return state.value

    def Usb1(self, state):
        result = self.__usb1Func(self.__tacHandle, state)
        if result != NO_TAC_ERROR:
            raise RuntimeError(f"Error: Usb1 returned {GetErrorString(result)}")

    def GetUsb1State(self):
        state = c_bool(False)
        rc = self.__getUsb1Func(self.__tacHandle, byref(state))
        if rc != 0:
            raise RuntimeError(f"Error: GetUsb1State returned {GetErrorString(rc)}")
        return state.value

    def PowerKey(self, state):
        result = self.__powerKeyFunc(self.__tacHandle, state)
        if result != NO_TAC_ERROR:
            raise RuntimeError(f"Error: PowerKey returned {GetErrorString(result)}")

    def GetPowerKeyState(self):
        state = c_bool(False)
        rc = self.__getPowerKeyFunc(self.__tacHandle, byref(state))
        if rc != 0:
            raise RuntimeError(f"Error: GetPowerKeyState returned {GetErrorString(rc)}")
        return state.value

    def VolumeUp(self, state):
        result = self.__volumeUpFunc(self.__tacHandle, state)
        if result != NO_TAC_ERROR:
            raise RuntimeError(f"Error: VolumeUp returned {GetErrorString(result)}")

    def GetVolumeUpState(self):
        state = c_bool(False)
        rc = self.__getVolumeUpFunc(self.__tacHandle, byref(state))
        if rc != 0:
            raise RuntimeError(f"Error: GetVolumeUpState returned {GetErrorString(rc)}")
        return state.value

    def VolumeDown(self, state):
        result = self.__volumeDownFunc(self.__tacHandle, state)
        if result != NO_TAC_ERROR:
            raise RuntimeError(f"Error: VolumeDown returned {GetErrorString(result)}")

    def GetVolumeDownState(self):
        state = c_bool(False)
        rc = self.__getVolumeDownFunc(self.__tacHandle, byref(state))
        if rc != 0:
            raise RuntimeError(f"Error: GetVolumeDownState returned {GetErrorString(rc)}")
        return state.value

    def DisconnectUIM1(self, state):
        result = self.__disconnectUIM1Func(self.__tacHandle, state)
        if result != NO_TAC_ERROR:
            raise RuntimeError(f"Error: DisconnectUIM1 returned {GetErrorString(result)}")

    def GetDisconnectUIM1State(self):
        state = c_bool(False)
        rc = self.__getDisconnectUIM1Func(self.__tacHandle, byref(state))
        if rc != 0:
            raise RuntimeError(f"Error: GetDisconnectUIM1State returned {GetErrorString(rc)}")
        return state.value

    def DisconnectUIM2(self, state):
        result = self.__disconnectUIM2Func(self.__tacHandle, state)
        if result != NO_TAC_ERROR:
            raise RuntimeError(f"Error: DisconnectUIM2 returned {GetErrorString(result)}")

    def GetDisconnectUIM2State(self):
        state = c_bool(False)
        rc = self.__getDisconnectUIM2Func(self.__tacHandle, byref(state))
        if rc != 0:
            raise RuntimeError(f"Error: GetDisconnectUIM2State returned {GetErrorString(rc)}")
        return state.value

    def DisconnectSDCard(self, state):
        result = self.__disconnectSDCardFunc(self.__tacHandle, state)
        if result != NO_TAC_ERROR:
            raise RuntimeError(f"Error: DisconnectSDCard returned {GetErrorString(result)}")

    def GetDisconnectSDCardState(self):
        state = c_bool(False)
        rc = self.__getDisconnectSDCardFunc(self.__tacHandle, byref(state))
        if rc != 0:
            raise RuntimeError(f"Error: GetDisconnectSDCardState returned {GetErrorString(rc)}")
        return state.value

    def PrimaryEDL(self, state):
        result = self.__primaryEDLFunc(self.__tacHandle, state)
        if result != NO_TAC_ERROR:
            raise RuntimeError(f"Error: PrimaryEDL returned {GetErrorString(result)}")

    def GetPrimaryEDLState(self):
        state = c_bool(False)
        rc = self.__getPrimaryEDLStateFunc(self.__tacHandle, byref(state))
        if rc != 0:
            raise RuntimeError(f"Error: GetPrimaryEDLState returned {GetErrorString(rc)}")
        return state.value

    def ForcePSHoldHigh(self, state):
        result = self.__forcePSHoldHighFunc(self.__tacHandle, state)
        if result != NO_TAC_ERROR:
            raise RuntimeError(f"Error: ForcePSHoldHigh returned {GetErrorString(result)}")

    def GetForcePSHoldHighState(self):
        state = c_bool(False)
        rc = self.__getForcePSHoldHighStateFunc(self.__tacHandle, byref(state))
        if rc != 0:
            raise RuntimeError(f"Error: GetForcePSHoldHighState returned {GetErrorString(rc)}")
        return state.value

    def SecondaryEDL(self, state):
        result = self.__secondaryEDLFunc(self.__tacHandle, state)
        if result != NO_TAC_ERROR:
            raise RuntimeError(f"Error: SecondaryEDL returned {GetErrorString(result)}")

    def GetSecondaryEDLState(self):
        state = c_bool(False)
        rc = self.__getSecondaryEDLStateFunc(self.__tacHandle, byref(state))
        if rc != 0:
            raise RuntimeError(f"Error: GetSecondaryEDLState returned {GetErrorString(rc)}")
        return state.value

    def SecondaryPM_RESIN_N(self, state):
        result = self.__secondaryPM_RESIN_NFunc(self.__tacHandle, state)
        if result != NO_TAC_ERROR:
            raise RuntimeError(f"Error: SecondaryPM_RESIN_N returned {GetErrorString(result)}")

    def GetSecondaryPM_RESIN_NState(self):
        state = c_bool(False)
        rc = self.__getSecondaryPM_RESIN_NStateFunc(self.__tacHandle, byref(state))
        if rc != 0:
            raise RuntimeError(f"Error: GetSecondaryPM_RESIN_NState returned {GetErrorString(rc)}")
        return state.value

    def Eud(self, state):
        result = self.__eudFunc(self.__tacHandle, state)
        if result != NO_TAC_ERROR:
            raise RuntimeError(f"Error: Eud returned {GetErrorString(result)}")

    def GetEUDState(self):
        state = c_bool(False)
        rc = self.__getEUDStateFunc(self.__tacHandle, byref(state))
        if rc != 0:
            raise RuntimeError(f"Error: GetEUDState returned {GetErrorString(rc)}")
        return state.value

    def HeadsetDisconnect(self, state):
        result = self.__headsetDisconnectFunc(self.__tacHandle, state)
        if result != NO_TAC_ERROR:
            raise RuntimeError(f"Error: HeadsetDisconnect returned {GetErrorString(result)}")

    def GetHeadsetDisconnectState(self):
        state = c_bool(False)
        rc = self.__getHeadsetDisconnectStateFunc(self.__tacHandle, byref(state))
        if rc != 0:
            raise RuntimeError(f"Error: GetHeadsetDisconnectState returned {GetErrorString(rc)}")
        return state.value

    def SetName(self, newName):
        newNameStr = newName.encode("utf-8")
        rc = self.__setNameFunc(self.__tacHandle, newNameStr)
        if rc != 0:
            raise RuntimeError(f"Error: SetName returned {GetErrorString(rc)}")

    def GetResetCount(self):
        resetCount = c_int(0)
        rc = self.__getResetCountFunc(self.__tacHandle, byref(resetCount))
        if rc != 0:
            raise RuntimeError(f"Error: GetResetCount returned {GetErrorString(rc)}")
        return resetCount

    def ClearResetCount(self):
        rc = self.__clearResetCountFunc(self.__tacHandle)
        if rc != 0:
            raise RuntimeError(f"Error: ClearResetCount returned {GetErrorString(rc)}")
        return True

    def PowerOnButton(self):
        rc = self.__powerOnButtonFunc(self.__tacHandle)
        if rc != 0:
            raise RuntimeError(f"Error: PowerOnButton returned {GetErrorString(rc)}")
        return True

    def PowerOffButton(self):
        rc = self.__powerOffButtonFunc(self.__tacHandle)
        if rc != 0:
            raise RuntimeError(f"Error: PowerOffButton returned {GetErrorString(rc)}")
        return True

    def BootToFastBootButton(self):
        rc = self.__bootToFastBootButtonFunc(self.__tacHandle)
        if rc != 0:
            raise RuntimeError(f"Error: BootToFastBootButton returned {GetErrorString(rc)}")
        return True

    def BootToUEFIMenuButton(self):
        rc = self.__bootToUEFIMenuButtonFunc(self.__tacHandle)
        if rc != 0:
            raise RuntimeError(f"Error: BootToUEFIMenuButton returned {GetErrorString(rc)}")
        return True

    def BootToEDLButton(self):
        rc = self.__bootToEDLButtonFunc(self.__tacHandle)
        if rc != 0:
            raise RuntimeError(f"Error: BootToEDLButton returned {GetErrorString(rc)}")
        return True

    def BootToSecondaryEDLButton(self):
        rc = self.__bootToSecondaryEDLButtonFunc(self.__tacHandle)
        if rc != 0:
            raise RuntimeError(f"Error: BootToSecondaryEDLButton returned {GetErrorString(rc)}")
        return True

    def GetScriptVariableCount(self):
        scriptVariableCount = c_int(0)
        result = self.__getScriptVariableCountFunc(self.__tacHandle, byref(scriptVariableCount))
        if result != NO_TAC_ERROR:
            raise RuntimeError(f"Error: GetScriptVariableCount returned {GetErrorString(result)}")
        return scriptVariableCount.value

    def GetScriptVariable(self, scriptVariableIndex):
        scriptVariableBuffer = create_string_buffer(1024)
        result = self.__getScriptVariableFunc(self.__tacHandle, scriptVariableIndex, scriptVariableBuffer, sizeof(scriptVariableBuffer))
        if result != NO_TAC_ERROR:
            raise RuntimeError(f"Error: GetScriptVariable returned {GetErrorString(result)}")
        return scriptVariableBuffer.value.decode("cp1252")

    def UpdateScriptVariableValue(self, scriptVariable, scriptVariableValue):
        scriptVariableStr = scriptVariable.encode("utf-8")
        scriptVariableValueStr = scriptVariableValue.encode("utf-8")
        result = self.__updateScriptVariableValueFunc(self.__tacHandle, scriptVariableStr, scriptVariableValueStr)
        if result != NO_TAC_ERROR:
            raise RuntimeError(f"Error: UpdateScriptVariableValue returned {GetErrorString(result)}")
        return result

    def SetupClassEntries(self, tacDll):
        self.__openByNameFunc = tacDll.OpenHandleByDescription
        self.__openByNameFunc.argtypes = [c_char_p]

        self.__closeFunc = tacDll.CloseTACHandle
        self.__nameFunc = tacDll.GetName
        self.__firmwareVersionFunc = tacDll.GetFirmwareVersion
        self.__hardwareFunc = tacDll.GetHardware
        self.__hardwareVersionFunc = tacDll.GetHardwareVersion
        self.__uuidFunc = tacDll.GetUUID
        self.__setExternalPowerControlFunc = tacDll.SetExternalPowerControl

        self.__getCommandCount = tacDll.GetCommandCount
        self.__getCommand = tacDll.GetCommand
        self.__getQuickCommandCount = tacDll.GetQuickCommandCount
        self.__getQuickCommand = tacDll.GetQuickCommand
        self.__getCommandState = tacDll.GetCommandState
        self.__sendCommand = tacDll.SendCommand
        self.__setPin = tacDll.SetPinState
        self.__helpText = tacDll.GetHelpText

        self.__setBatteryStateFunc = tacDll.SetBatteryState
        self.__getBatteryStateFunc = tacDll.GetBatteryState
        self.__usb0Func = tacDll.Usb0
        self.__getUsb0Func = tacDll.GetUsb0State
        self.__usb1Func = tacDll.Usb1
        self.__getUsb1Func = tacDll.GetUsb1State
        self.__powerKeyFunc = tacDll.PowerKey
        self.__getPowerKeyFunc = tacDll.GetPowerKeyState
        self.__volumeUpFunc = tacDll.VolumeUp
        self.__getVolumeUpFunc = tacDll.GetVolumeUpState
        self.__volumeDownFunc = tacDll.VolumeDown
        self.__getVolumeDownFunc = tacDll.GetVolumeDownState
        self.__disconnectUIM1Func = tacDll.DisconnectUIM1
        self.__getDisconnectUIM1Func = tacDll.GetDisconnectUIM1State
        self.__disconnectUIM2Func = tacDll.DisconnectUIM2
        self.__getDisconnectUIM2Func = tacDll.GetDisconnectUIM2State
        self.__disconnectSDCardFunc = tacDll.DisconnectSDCard
        self.__getDisconnectSDCardFunc = tacDll.GetDisconnectSDCardState
        self.__primaryEDLFunc = tacDll.PrimaryEDL
        self.__getPrimaryEDLStateFunc = tacDll.GetPrimaryEDLState
        self.__secondaryEDLFunc = tacDll.SecondaryEDL
        self.__getSecondaryEDLStateFunc = tacDll.GetSecondaryEDLState
        self.__forcePSHoldHighFunc = tacDll.ForcePSHoldHigh
        self.__getForcePSHoldHighStateFunc = tacDll.GetForcePSHoldHighState
        self.__secondaryPM_RESIN_NFunc = tacDll.SecondaryPmResinN
        self.__getSecondaryPM_RESIN_NStateFunc = tacDll.GetSecondaryPmResinNState
        self.__eudFunc = tacDll.Eud
        self.__getEUDStateFunc = tacDll.GetEUDState
        self.__headsetDisconnectFunc = tacDll.HeadsetDisconnect
        self.__getHeadsetDisconnectStateFunc = tacDll.GetHeadsetDisconnectState
        self.__setNameFunc = tacDll.SetName
        self.__getResetCountFunc = tacDll.GetResetCount
        self.__clearResetCountFunc = tacDll.ClearResetCount

        self.__powerOnButtonFunc = tacDll.PowerOnButton
        self.__powerOffButtonFunc = tacDll.PowerOffButton
        self.__bootToFastBootButtonFunc = tacDll.BootToFastBootButton
        self.__bootToUEFIMenuButtonFunc = tacDll.BootToUEFIMenuButton
        self.__bootToEDLButtonFunc = tacDll.BootToEDLButton
        self.__bootToSecondaryEDLButtonFunc = tacDll.BootToSecondaryEDLButton

        self.__getScriptVariableCountFunc = tacDll.GetScriptVariableCount
        self.__getScriptVariableFunc = tacDll.GetScriptVariable
        self.__updateScriptVariableValueFunc = tacDll.UpdateScriptVariableValue

def AlpacaVersion():
    buf = create_string_buffer(1024)
    __alpacaVersionFunc(buf, sizeof(buf))
    alpacaVersion = buf.value.decode("cp1252")
    return alpacaVersion


def TACVersion():
    buf = create_string_buffer(1024)
    __tacVersionFunc(buf, sizeof(buf))
    tacVersion = buf.value.decode("cp1252")
    return tacVersion


def GetDeviceCount():
    deviceCount = c_int(0)
    rc = __getDeviceCountFunc(byref(deviceCount))
    if rc != 0:
        raise RuntimeError(f"Error: GetDeviceCount returned {GetErrorString(rc)}")
    
    return deviceCount.value

def GetDevice(deviceIndex):
    buf = create_string_buffer(1024)
    __getDeviceFunc(deviceIndex, buf, sizeof(buf))
    deviceAttributes = buf.value.decode("cp1252").split(";")
    if len(deviceAttributes) >= 3:
        tacDevice = TacDevice(deviceAttributes[0], deviceAttributes[1], deviceAttributes[2])
        tacDevice.SetupClassEntries(tacLibrary)
    else:
        tacDevice = None

    return tacDevice


def main() -> None:
    """
    Sample python program that demonstrates how TAC APIs
    can be used to power off and power on the TAC device and
    display relevant TAC device information.
    """
    # get the alpaca version
    alpacaVersion = AlpacaVersion()
    print("QTAC Version:", alpacaVersion)

    # get the TAC version
    tacVersion = TACVersion()
    print("TAC Version:", tacVersion)

    # get the count of TAC devices attached to the machine
    deviceCount = GetDeviceCount()
    if deviceCount > 0:
        index = 0
        # iterate over the device indexes and print the TAC device information
        while index < deviceCount:
            tacDevice = GetDevice(index)
            index += 1
            if tacDevice is not None:
                print(f"Found device with port name: {tacDevice.PortName()} and serial number: {tacDevice.SerialNumber()}")

        # get the TACDevice object for the first TAC device with index 0.
        tacDevice = GetDevice(0)
        if tacDevice is not None:
            # Open the TAC device
            if tacDevice.Open():
                print(f"Serial Number: {tacDevice.SerialNumber()}")
                print(f"Battery State: {tacDevice.GetBatteryState()}")
                tacDevice.SetBatteryState(True)
                # The delay of 2s provides a tester a window to view the physical device's state.
                # It does not have any significance in the automation workflow
                time.sleep(2)
                print(f"Battery State: {tacDevice.GetBatteryState()}")
                
                tacDevice.SetBatteryState(False)
                # The delay of 2s provides a tester a window to view the physical device's state.
                # It does not have any significance in the automation workflow
                time.sleep(2)
                print(f"Battery State: {tacDevice.GetBatteryState()}")

                print(f"Command count: {tacDevice.GetCommandCount()}")

                exampleCommand = tacDevice.GetCommand(1)
                print(f"Get command: {exampleCommand}")
                print(f"Get battery command state: {tacDevice.GetCommandState('battery')}")

                print(f"Get quick settings command count: {tacDevice.GetQuickCommandCount()}")
                exampleQuickCommand = tacDevice.GetQuickCommand(1)
                print(f"Quick settings command: {exampleQuickCommand}")

                print("\n*****************************\nHelp Text for the TAC device:\n*****************************\n")
                print(tacDevice.GetHelpText())

                print(f"Get variable count: {tacDevice.GetScriptVariableCount()}")
                getScriptVariable = tacDevice.GetScriptVariable(1)
                print(f"Get variable value: {getScriptVariable}")

                updateScriptVariable = tacDevice.UpdateScriptVariableValue('fastboot', '9000')
                print(f"Variable update result: {updateScriptVariable}")

                getScriptVariable = tacDevice.GetScriptVariable(1)
                print(f"Get updated variable value: {getScriptVariable}")

                tacDevice.Close()
            else:
                print(f"Device with port number: {tacDevice.PortName()} cannot be opened.")
        else:
            print(f"Device with port number: {tacDevice.PortName()} not found.")


if __name__ == "__main__":
    main()
