// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted (subject to the limitations in the
// disclaimer below) provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//         notice, this list of conditions and the following disclaimer.
//
//     * Redistributions in binary form must reproduce the above
//         copyright notice, this list of conditions and the following
//         disclaimer in the documentation and/or other materials provided
//         with the distribution.
//
//     * Neither the name of Qualcomm Technologies, Inc. nor the names of its
//         contributors may be used to endorse or promote products derived
//         from this software without specific prior written permission.
//
// NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
// GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
// HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
// IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
// IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "TACDev.h"
#include "TACDevCore.h"

#include <qtac/AlpacaScript.h>
#include <qtac/DebugBoardType.h>

#include <cstring>
#include <thread>
#include <chrono>

// ---------------------------------------------------------------------------
// Version constants
// ---------------------------------------------------------------------------

#define ALPACA_VERSION  "5.5.2"
#define TAC_VERSION     "7.0.1"

// ---------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------

static DevTACCore gCore;

static const qtac::ByteArray kHandleNotOpen(
    "TAC device is not open. Please reopen the TAC device");
static const qtac::ByteArray kBufferTooSmall("TACDev buffer is too small");
static const qtac::ByteArray kBadIndex("User provided invalid index");

// ---------------------------------------------------------------------------
// Helper: safe copy into a caller-supplied char buffer.
// Returns NO_TAC_ERROR on success, TACDEV_BUFFER_TOO_SMALL if too small.
// ---------------------------------------------------------------------------

static TAC_RESULT safeCopy(char* dest, int destSize,
                           const qtac::ByteArray& src)
{
    if (src.size() >= destSize)
    {
        gCore.setLastError(kBufferTooSmall);
        return TACDEV_BUFFER_TOO_SMALL;
    }
    std::memcpy(dest, src.constData(), static_cast<size_t>(src.size()));
    dest[src.size()] = '\0';
    return NO_TAC_ERROR;
}

// ---------------------------------------------------------------------------
// InitializeTACDev
// ---------------------------------------------------------------------------

TAC_RESULT InitializeTACDev()
{
    // Nothing to initialise in the Qt-free implementation.
    return NO_TAC_ERROR;
}

// ---------------------------------------------------------------------------
// Version / error queries
// ---------------------------------------------------------------------------

TAC_RESULT GetAlpacaVersion(char* alpacaVersion, int bufferSize)
{
    return safeCopy(alpacaVersion, bufferSize, qtac::ByteArray(ALPACA_VERSION));
}

TAC_RESULT GetTACVersion(char* tacVersion, int bufferSize)
{
    return safeCopy(tacVersion, bufferSize, qtac::ByteArray(TAC_VERSION));
}

TAC_RESULT GetLastTACError(char* lastError, int bufferSize)
{
    return safeCopy(lastError, bufferSize, gCore.lastError());
}

// ---------------------------------------------------------------------------
// Logging (no-op in Qt-free build)
// ---------------------------------------------------------------------------

TAC_RESULT GetLoggingState(bool* loggingState)
{
    *loggingState = false;
    return NO_TAC_ERROR;
}

TAC_RESULT SetLoggingState(bool /*loggingState*/)
{
    return NO_TAC_ERROR;
}

// ---------------------------------------------------------------------------
// Device enumeration
// ---------------------------------------------------------------------------

TAC_RESULT GetDeviceCount(int* deviceCount)
{
    return gCore.getDeviceCount(deviceCount);
}

unsigned long GetPortData(int deviceIndex, char* portData, int bufferSize)
{
    const AlpacaDevices& devices = gCore.alpacaDevices();
    if (deviceIndex < 0 || deviceIndex >= static_cast<int>(devices.size()))
        return 0;

    const AlpacaDevice& dev = devices.at(deviceIndex);

    qtac::ByteArray data = dev->portName();
    data += ";";
    data += dev->description();
    data += ";";
    data += dev->serialNumber();
    data += ";";
    data += qtac::ByteArray::number(deviceIndex);

    if (data.size() >= bufferSize)
        return static_cast<unsigned long>(data.size());

    std::memcpy(portData, data.constData(), static_cast<size_t>(data.size()));
    portData[data.size()] = '\0';
    return static_cast<unsigned long>(data.size());
}

// ---------------------------------------------------------------------------
// Handle open / close
// ---------------------------------------------------------------------------

TAC_HANDLE OpenHandleByDescription(const char* portName)
{
    return gCore.openHandleByDescription(portName);
}

TAC_RESULT CloseTACHandle(TAC_HANDLE tacHandle)
{
    return gCore.closeHandle(tacHandle);
}

// ---------------------------------------------------------------------------
// Device properties
// ---------------------------------------------------------------------------

TAC_RESULT GetName(TAC_HANDLE tacHandle, char* deviceName, int bufferSize)
{
    AlpacaDevice dev = gCore.getAlpacaDevice(tacHandle);
    if (!dev) { gCore.setLastError(kHandleNotOpen); return TACDEV_BAD_TAC_HANDLE; }
    return safeCopy(deviceName, bufferSize, dev->name());
}

TAC_RESULT GetFirmwareVersion(TAC_HANDLE tacHandle, char* firmwareVersion, int bufferSize)
{
    AlpacaDevice dev = gCore.getAlpacaDevice(tacHandle);
    if (!dev) { gCore.setLastError(kHandleNotOpen); return TACDEV_BAD_TAC_HANDLE; }
    return safeCopy(firmwareVersion, bufferSize, dev->firmwareVersion().toLatin1());
}

TAC_RESULT GetHardware(TAC_HANDLE tacHandle, char* hardware, int bufferSize)
{
    AlpacaDevice dev = gCore.getAlpacaDevice(tacHandle);
    if (!dev) { gCore.setLastError(kHandleNotOpen); return TACDEV_BAD_TAC_HANDLE; }
    return safeCopy(hardware, bufferSize, dev->debugBoardTypeString().toLatin1());
}

TAC_RESULT GetHardwareVersion(TAC_HANDLE tacHandle, char* hardwareVersion, int bufferSize)
{
    AlpacaDevice dev = gCore.getAlpacaDevice(tacHandle);
    if (!dev) { gCore.setLastError(kHandleNotOpen); return TACDEV_BAD_TAC_HANDLE; }
    return safeCopy(hardwareVersion, bufferSize, dev->hardwareVersionString().toLatin1());
}

TAC_RESULT GetUUID(TAC_HANDLE tacHandle, char* uuid, int bufferSize)
{
    AlpacaDevice dev = gCore.getAlpacaDevice(tacHandle);
    if (!dev) { gCore.setLastError(kHandleNotOpen); return TACDEV_BAD_TAC_HANDLE; }
    return safeCopy(uuid, bufferSize, dev->uuid().toLatin1());
}

// ---------------------------------------------------------------------------
// External power
// ---------------------------------------------------------------------------

TAC_ERROR SetExternalPowerControl(TAC_HANDLE tacHandle, bool state)
{
    AlpacaDevice dev = gCore.getAlpacaDevice(tacHandle);
    if (!dev) { gCore.setLastError(kHandleNotOpen); return TACDEV_BAD_TAC_HANDLE; }
    if (!dev->hasCommand(qtac::ByteArray("extpower")))
    {
        gCore.setLastError(qtac::ByteArray("External power control not supported on this device"));
        return TACDEV_COMMAND_NOT_FOUND;
    }
    dev->externalPowerControl(state);
    return NO_TAC_ERROR;
}

// ---------------------------------------------------------------------------
// Dynamic command list
// ---------------------------------------------------------------------------

TAC_ERROR GetCommandCount(TAC_HANDLE tacHandle, unsigned long* commandCount)
{
    AlpacaDevice dev = gCore.getAlpacaDevice(tacHandle);
    if (!dev) { gCore.setLastError(kHandleNotOpen); return TACDEV_BAD_TAC_HANDLE; }
    *commandCount = static_cast<unsigned long>(dev->commandCount());
    return NO_TAC_ERROR;
}

TAC_ERROR GetCommand(TAC_HANDLE tacHandle, unsigned long commandIndex,
                     char* commandBuffer, int bufferSize)
{
    AlpacaDevice dev = gCore.getAlpacaDevice(tacHandle);
    if (!dev) { gCore.setLastError(kHandleNotOpen); return TACDEV_BAD_TAC_HANDLE; }

    TACCommand entry = dev->commandEntry(static_cast<uint32_t>(commandIndex));
    if (entry._pin == static_cast<PinID>(-1))
    {
        gCore.setLastError(kBadIndex);
        return TACDEV_BAD_INDEX;
    }

    qtac::ByteArray data = entry._command.toLatin1();
    data += ";";
    data += entry._helpText.toLatin1();
    data += ";";
    data += qtac::ByteArray::number(static_cast<int>(entry._pin));
    data += ";";
    data += entry._tabName;
    data += ";";
    data += entry._groupName;
    data += ";";
    data += entry._cellLocation;

    return safeCopy(commandBuffer, bufferSize, data);
}

// ---------------------------------------------------------------------------
// Quick commands (named script functions from ButtonEntries)
// ---------------------------------------------------------------------------

TAC_ERROR GetQuickCommandCount(TAC_HANDLE tacHandle, unsigned long* commandCount)
{
    AlpacaDevice dev = gCore.getAlpacaDevice(tacHandle);
    if (!dev) { gCore.setLastError(kHandleNotOpen); return TACDEV_BAD_TAC_HANDLE; }
    *commandCount = static_cast<unsigned long>(dev->getButtons().size());
    return NO_TAC_ERROR;
}

TAC_ERROR GetQuickCommand(TAC_HANDLE tacHandle, unsigned long commandIndex,
                          char* commandBuffer, int bufferSize)
{
    AlpacaDevice dev = gCore.getAlpacaDevice(tacHandle);
    if (!dev) { gCore.setLastError(kHandleNotOpen); return TACDEV_BAD_TAC_HANDLE; }

    const qtac::ButtonEntries& buttons = dev->getButtons();
    if (commandIndex >= static_cast<unsigned long>(buttons.size()))
    {
        gCore.setLastError(kBadIndex);
        return TACDEV_BAD_INDEX;
    }

    const qtac::ButtonEntry& btn = buttons[commandIndex];
    qtac::ByteArray data = btn._name.toLatin1();
    data += ";";
    data += btn._command.toLatin1();
    data += ";";
    data += btn._tooltip.toLatin1();
    data += ";";
    data += qtac::ByteArray::number(btn._cellX);
    data += ",";
    data += qtac::ByteArray::number(btn._cellY);
    return safeCopy(commandBuffer, bufferSize, data);
}

// ---------------------------------------------------------------------------
// Script variables (from VariableEntries)
// ---------------------------------------------------------------------------

TAC_ERROR GetScriptVariableCount(TAC_HANDLE tacHandle, unsigned long* scriptVariableCount)
{
    AlpacaDevice dev = gCore.getAlpacaDevice(tacHandle);
    if (!dev) { gCore.setLastError(kHandleNotOpen); return TACDEV_BAD_TAC_HANDLE; }
    *scriptVariableCount = static_cast<unsigned long>(dev->getVariables().size());
    return NO_TAC_ERROR;
}

TAC_ERROR GetScriptVariable(TAC_HANDLE tacHandle, unsigned long scriptVariableIndex,
                            char* scriptVariableBuffer, int bufferSize)
{
    AlpacaDevice dev = gCore.getAlpacaDevice(tacHandle);
    if (!dev) { gCore.setLastError(kHandleNotOpen); return TACDEV_BAD_TAC_HANDLE; }

    const qtac::VariableEntries& vars = dev->getVariables();
    if (scriptVariableIndex >= static_cast<unsigned long>(vars.size()))
    {
        gCore.setLastError(kBadIndex);
        return TACDEV_BAD_INDEX;
    }

    // Iterate to the requested index (qtac::Map iterates in sorted order).
    auto it = vars.begin();
    std::advance(it, scriptVariableIndex);
    const qtac::VariableEntry& var = it->second;

    // Build the semicolon-separated payload:
    // name;label;tooltip;type;defaultValue;cellX,cellY
    qtac::ByteArray defaultVal;
    switch (var._type)
    {
    case qtac::VariableType::Boolean:
        defaultVal = var._defaultValue.toBool() ? "1" : "0";
        break;
    case qtac::VariableType::Float:
        defaultVal = var._defaultValue.toString().toLatin1();
        break;
    default:
        defaultVal = qtac::ByteArray::number(static_cast<int>(var._defaultValue.toUInt()));
        break;
    }

    qtac::ByteArray data = var._name.toLatin1();
    data += ";";
    data += var._label.toLatin1();
    data += ";";
    data += var._tooltip.toLatin1();
    data += ";";
    data += qtac::ByteArray::number(static_cast<int>(var._type));
    data += ";";
    data += defaultVal;
    data += ";";
    data += qtac::ByteArray::number(var._cellX);
    data += ",";
    data += qtac::ByteArray::number(var._cellY);
    return safeCopy(scriptVariableBuffer, bufferSize, data);
}

TAC_ERROR UpdateScriptVariableValue(TAC_HANDLE tacHandle,
                                    const char* scriptVariable,
                                    const char* scriptVariableValue)
{
    AlpacaDevice dev = gCore.getAlpacaDevice(tacHandle);
    if (!dev) { gCore.setLastError(kHandleNotOpen); return TACDEV_BAD_TAC_HANDLE; }

    // Determine variable type and set accordingly.
    const qtac::VariableEntries& vars = dev->getVariables();
    qtac::String name(scriptVariable);
    auto it = vars.find(name);
    if (it == vars.end())
    {
        gCore.setLastError(qtac::ByteArray("Script variable not found: ") +
                           qtac::ByteArray(scriptVariable));
        return TACDEV_SCRIPT_VARIABLE_NOT_FOUND;
    }

    const qtac::VariableEntry& var = it->second;
    if (var._type == qtac::VariableType::Boolean)
    {
        bool val = (std::strcmp(scriptVariableValue, "true") == 0 ||
                    std::strcmp(scriptVariableValue, "1")    == 0);
        dev->setVariableValue(name, qtac::Variant(val));
    }
    else
    {
        bool ok = false;
        int val = qtac::ByteArray(scriptVariableValue).toInt(&ok);
        dev->setVariableValue(name, qtac::Variant(ok ? val : 0));
    }

    return NO_TAC_ERROR;
}

// ---------------------------------------------------------------------------
// Generic command state get/set
// ---------------------------------------------------------------------------

TAC_ERROR GetCommandState(TAC_HANDLE tacHandle, const char* command, bool* state)
{
    AlpacaDevice dev = gCore.getAlpacaDevice(tacHandle);
    if (!dev) { gCore.setLastError(kHandleNotOpen); return TACDEV_BAD_TAC_HANDLE; }

    qtac::ByteArray cmd(command);
    if (!dev->hasCommand(cmd))
    {
        gCore.setLastError(qtac::ByteArray("Command '") + cmd + "' not found");
        return TACDEV_COMMAND_NOT_FOUND;
    }
    *state = dev->getCommandState(cmd);
    return NO_TAC_ERROR;
}

TAC_ERROR SendCommand(TAC_HANDLE tacHandle, const char* command, bool state)
{
    AlpacaDevice dev = gCore.getAlpacaDevice(tacHandle);
    if (!dev) { gCore.setLastError(kHandleNotOpen); return TACDEV_BAD_TAC_HANDLE; }

    qtac::ByteArray cmd(command);
    if (!dev->hasCommand(cmd))
    {
        gCore.setLastError(qtac::ByteArray("Command '") + cmd + "' not found");
        return TACDEV_COMMAND_NOT_FOUND;
    }
    dev->setWaitForCompletion();
    dev->sendCommand(cmd, state);
    return NO_TAC_ERROR;
}

// ---------------------------------------------------------------------------
// Help text
// ---------------------------------------------------------------------------

TAC_ERROR GetHelpText(TAC_HANDLE tacHandle, char* helpBuffer,
                      int bufferSize, int* actualSize)
{
    AlpacaDevice dev = gCore.getAlpacaDevice(tacHandle);
    if (!dev)
    {
        if (actualSize) *actualSize = 0;
        gCore.setLastError(kHandleNotOpen);
        return TACDEV_BAD_TAC_HANDLE;
    }

    qtac::ByteArray help = dev->getHelp();
    if (actualSize) *actualSize = help.size();
    return safeCopy(helpBuffer, bufferSize, help);
}

// ---------------------------------------------------------------------------
// Raw pin state
// ---------------------------------------------------------------------------

TAC_ERROR SetPinState(TAC_HANDLE tacHandle, int pin, bool state)
{
    AlpacaDevice dev = gCore.getAlpacaDevice(tacHandle);
    if (!dev) { gCore.setLastError(kHandleNotOpen); return TACDEV_BAD_TAC_HANDLE; }
    dev->setWaitForCompletion();
    dev->setPinState(static_cast<PinID>(pin), state);
    return NO_TAC_ERROR;
}

// ---------------------------------------------------------------------------
// Command queue
// ---------------------------------------------------------------------------

TAC_ERROR IsCommandQueueClear(TAC_HANDLE tacHandle, bool* status)
{
    AlpacaDevice dev = gCore.getAlpacaDevice(tacHandle);
    if (!dev) { gCore.setLastError(kHandleNotOpen); return TACDEV_BAD_TAC_HANDLE; }

    // Invert: isCommandQueueClear() returns true when clear.
    *status = !dev->isCommandQueueClear();

    // PIC32CX firmware doesn't send ack — apply legacy delay.
    if (dev->debugBoardType() == ePIC32CXAuto)
        std::this_thread::sleep_for(std::chrono::seconds(10));

    return NO_TAC_ERROR;
}

// ---------------------------------------------------------------------------
// Named pin commands (delegate to generic send/get)
// ---------------------------------------------------------------------------

static TAC_RESULT setCmd(TAC_HANDLE h, const char* cmd, bool state)
{
    AlpacaDevice dev = gCore.getAlpacaDevice(h);
    if (!dev) { gCore.setLastError(kHandleNotOpen); return TACDEV_BAD_TAC_HANDLE; }
    qtac::ByteArray command(cmd);
    if (!dev->hasCommand(command))
    {
        gCore.setLastError(qtac::ByteArray("Command '") + command + "' not found");
        return TACDEV_COMMAND_NOT_FOUND;
    }
    dev->setWaitForCompletion();
    dev->sendCommand(command, state);
    return NO_TAC_ERROR;
}

static TAC_RESULT getCmd(TAC_HANDLE h, const char* cmd, bool* state)
{
    AlpacaDevice dev = gCore.getAlpacaDevice(h);
    if (!dev) { gCore.setLastError(kHandleNotOpen); return TACDEV_BAD_TAC_HANDLE; }
    qtac::ByteArray command(cmd);
    if (!dev->hasCommand(command))
    {
        gCore.setLastError(qtac::ByteArray("Command '") + command + "' not found");
        return TACDEV_COMMAND_NOT_FOUND;
    }
    *state = dev->getCommandState(command);
    return NO_TAC_ERROR;
}

TAC_RESULT SetBatteryState(TAC_HANDLE h, bool s)    { return setCmd(h, "battery", s); }
TAC_RESULT GetBatteryState(TAC_HANDLE h, bool* s)   { return getCmd(h, "battery", s); }
TAC_RESULT Usb0(TAC_HANDLE h, bool s)               { return setCmd(h, "usb0", s); }
TAC_RESULT GetUsb0State(TAC_HANDLE h, bool* s)      { return getCmd(h, "usb0", s); }
TAC_RESULT Usb1(TAC_HANDLE h, bool s)               { return setCmd(h, "usb1", s); }
TAC_RESULT GetUsb1State(TAC_HANDLE h, bool* s)      { return getCmd(h, "usb1", s); }
TAC_RESULT PowerKey(TAC_HANDLE h, bool s)           { return setCmd(h, "pkey", s); }
TAC_RESULT GetPowerKeyState(TAC_HANDLE h, bool* s)  { return getCmd(h, "pkey", s); }
TAC_RESULT VolumeUp(TAC_HANDLE h, bool s)           { return setCmd(h, "volup", s); }
TAC_RESULT GetVolumeUpState(TAC_HANDLE h, bool* s)  { return getCmd(h, "volup", s); }
TAC_RESULT VolumeDown(TAC_HANDLE h, bool s)         { return setCmd(h, "voldn", s); }
TAC_RESULT GetVolumeDownState(TAC_HANDLE h, bool* s){ return getCmd(h, "voldn", s); }
TAC_RESULT DisconnectUIM1(TAC_HANDLE h, bool s)     { return setCmd(h, "uim1", s); }
TAC_RESULT GetDisconnectUIM1State(TAC_HANDLE h, bool* s) { return getCmd(h, "uim1", s); }
TAC_RESULT DisconnectUIM2(TAC_HANDLE h, bool s)     { return setCmd(h, "uim2", s); }
TAC_RESULT GetDisconnectUIM2State(TAC_HANDLE h, bool* s) { return getCmd(h, "uim2", s); }
TAC_RESULT DisconnectSDCard(TAC_HANDLE h, bool s)   { return setCmd(h, "sdcard", s); }
TAC_RESULT GetDisconnectSDCardState(TAC_HANDLE h, bool* s) { return getCmd(h, "sdcard", s); }
TAC_RESULT PrimaryEDL(TAC_HANDLE h, bool s)         { return setCmd(h, "pedl", s); }
TAC_RESULT GetPrimaryEDLState(TAC_HANDLE h, bool* s){ return getCmd(h, "pedl", s); }
TAC_RESULT SecondaryEDL(TAC_HANDLE h, bool s)       { return setCmd(h, "sedl", s); }
TAC_RESULT GetSecondaryEDLState(TAC_HANDLE h, bool* s) { return getCmd(h, "sedl", s); }
TAC_RESULT ForcePSHoldHigh(TAC_HANDLE h, bool s)    { return setCmd(h, "pshold", s); }
TAC_RESULT GetForcePSHoldHighState(TAC_HANDLE h, bool* s) { return getCmd(h, "pshold", s); }
TAC_RESULT SecondaryPmResinN(TAC_HANDLE h, bool s)  { return setCmd(h, "sresn", s); }
TAC_RESULT GetSecondaryPmResinNState(TAC_HANDLE h, bool* s) { return getCmd(h, "sresn", s); }
TAC_RESULT Eud(TAC_HANDLE h, bool s)                { return setCmd(h, "eud", s); }
TAC_RESULT GetEUDState(TAC_HANDLE h, bool* s)       { return getCmd(h, "eud", s); }
TAC_RESULT HeadsetDisconnect(TAC_HANDLE h, bool s)  { return setCmd(h, "headset", s); }
TAC_RESULT GetHeadsetDisconnectState(TAC_HANDLE h, bool* s) { return getCmd(h, "headset", s); }

// ---------------------------------------------------------------------------
// Rename / reset count
// ---------------------------------------------------------------------------

TAC_RESULT SetName(TAC_HANDLE tacHandle, const char* newName)
{
    AlpacaDevice dev = gCore.getAlpacaDevice(tacHandle);
    if (!dev) { gCore.setLastError(kHandleNotOpen); return TACDEV_BAD_TAC_HANDLE; }
    dev->setWaitForCompletion();
    dev->setName(qtac::ByteArray(newName));
    return NO_TAC_ERROR;
}

TAC_RESULT GetResetCount(TAC_HANDLE tacHandle, int* resetCount)
{
    AlpacaDevice dev = gCore.getAlpacaDevice(tacHandle);
    if (!dev) { gCore.setLastError(kHandleNotOpen); return TACDEV_BAD_TAC_HANDLE; }
    dev->setWaitForCompletion();
    *resetCount = dev->getResetCount();
    return NO_TAC_ERROR;
}

TAC_RESULT ClearResetCount(TAC_HANDLE tacHandle)
{
    AlpacaDevice dev = gCore.getAlpacaDevice(tacHandle);
    if (!dev) { gCore.setLastError(kHandleNotOpen); return TACDEV_BAD_TAC_HANDLE; }
    dev->setWaitForCompletion();
    dev->clearResetCount();
    return NO_TAC_ERROR;
}

// ---------------------------------------------------------------------------
// Quick command buttons
// ---------------------------------------------------------------------------

static TAC_RESULT quickCmd(TAC_HANDLE h, const char* cmd)
{
    AlpacaDevice dev = gCore.getAlpacaDevice(h);
    if (!dev) { gCore.setLastError(kHandleNotOpen); return TACDEV_BAD_TAC_HANDLE; }
    dev->quickCommand(qtac::ByteArray(cmd));
    return NO_TAC_ERROR;
}

TAC_RESULT PowerOnButton(TAC_HANDLE h)            { return quickCmd(h, "powerOn"); }
TAC_RESULT PowerOffButton(TAC_HANDLE h)           { return quickCmd(h, "powerOff"); }
TAC_RESULT BootToFastBootButton(TAC_HANDLE h)     { return quickCmd(h, "bootToFastboot"); }
TAC_RESULT BootToUEFIMenuButton(TAC_HANDLE h)     { return quickCmd(h, "bootToUEFI"); }
TAC_RESULT BootToEDLButton(TAC_HANDLE h)          { return quickCmd(h, "bootToEDL"); }
TAC_RESULT BootToSecondaryEDLButton(TAC_HANDLE h) { return quickCmd(h, "bootToSecondaryEDL"); }
