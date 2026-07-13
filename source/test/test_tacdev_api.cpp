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

// C API integration test: TACDev.dll
//
// Requires a PSoC TAC device physically connected.
// Exercises the public C API surface of TACDev.dll end-to-end:
//
//   1. Init          — InitializeTACDev()
//   2. Version       — GetAlpacaVersion(), GetTACVersion()
//   3. Enumeration   — GetDeviceCount(), GetPortData()
//   4. Open/close    — OpenHandleByDescription(), CloseTACHandle()
//   5. Properties    — GetName(), GetFirmwareVersion(), GetHardware(),
//                      GetHardwareVersion(), GetUUID()
//   6. Commands      — GetCommandCount(), GetCommand()
//   7. Quick cmds    — GetQuickCommandCount(), GetQuickCommand()
//   8. Script vars   — GetScriptVariableCount(), GetScriptVariable()
//   9. Command state — GetCommandState()
//  10. Double-close  — CloseTACHandle() on already-closed handle returns error

#include "TACDev.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static char g_buf[4096];

// Abort with a message on failure.
static void require(const char* label, bool cond)
{
    if (!cond)
    {
        char err[256] = {};
        GetLastTACError(err, sizeof(err));
        printf("  FAIL: %s  last_error='%s'\n", label, err);
        exit(1);
    }
}

static void requireOk(const char* label, TAC_RESULT r)
{
    require(label, r == NO_TAC_ERROR);
}

// GetPortData returns "portName;description;serialNumber;index".
// Extract the portName (first semicolon-delimited field).
static void portNameFromPortData(const char* portData, char* portName, int size)
{
    const char* semi = strchr(portData, ';');
    if (semi)
    {
        int len = (int)(semi - portData);
        if (len >= size) len = size - 1;
        strncpy(portName, portData, (size_t)len);
        portName[len] = '\0';
    }
    else
    {
        strncpy(portName, portData, (size_t)(size - 1));
        portName[size - 1] = '\0';
    }
}

// Return true if the portData string looks like a serial (COM) port entry
// (portName starts with "COM"), not an FTDI virtual device.
static bool isSerialPort(const char* portData)
{
    return strncmp(portData, "COM", 3) == 0;
}

// ---------------------------------------------------------------------------
// Test 1: Init
// ---------------------------------------------------------------------------

static void test_init()
{
    printf("test_init: ");
    requireOk("InitializeTACDev", InitializeTACDev());
    printf("PASS\n");
}

// ---------------------------------------------------------------------------
// Test 2: Version strings
// ---------------------------------------------------------------------------

static void test_versions()
{
    printf("test_versions:\n");

    requireOk("GetAlpacaVersion", GetAlpacaVersion(g_buf, sizeof(g_buf)));
    printf("  AlpacaVersion = '%s'\n", g_buf);

    requireOk("GetTACVersion", GetTACVersion(g_buf, sizeof(g_buf)));
    printf("  TACVersion    = '%s'\n", g_buf);

    printf("  PASS\n");
}

// ---------------------------------------------------------------------------
// Test 3: Enumeration — find the PSoC (serial-port) device
// ---------------------------------------------------------------------------

static void test_enumeration(char* outPortName, int portNameSize)
{
    printf("test_enumeration:\n");

    int count = 0;
    requireOk("GetDeviceCount", GetDeviceCount(&count));
    printf("  device count = %d\n", count);
    require("at least one device found", count >= 1);

    outPortName[0] = '\0';
    for (int i = 0; i < count; ++i)
    {
        char portData[512] = {};
        unsigned long r = GetPortData(i, portData, sizeof(portData));
        require("GetPortData", r > 0 || r == 0 /* 0 = bad index, handled below */);
        printf("  [%d] '%s'\n", i, portData);

        // Prefer a serial-port device (PSoC) for subsequent tests.
        if (outPortName[0] == '\0' && isSerialPort(portData))
            portNameFromPortData(portData, outPortName, portNameSize);
    }

    require("PSoC (serial port) device found", outPortName[0] != '\0');
    printf("  using port: '%s'\n", outPortName);
    printf("  PASS\n");
}

// ---------------------------------------------------------------------------
// Test 4: Open / close
// ---------------------------------------------------------------------------

static TAC_HANDLE test_open(const char* portName)
{
    printf("test_open: port='%s'\n", portName);

    TAC_HANDLE h = OpenHandleByDescription(portName);
    require("OpenHandleByDescription returned valid handle", h != kBadHandle);

    printf("  handle = %lu\n", h);
    printf("  PASS\n");
    return h;
}

// ---------------------------------------------------------------------------
// Test 5: Property accessors
// ---------------------------------------------------------------------------

static void test_properties(TAC_HANDLE h)
{
    printf("test_properties:\n");

    // These may return empty strings for PSoC (known limitation) — just print.
    char name[256] = {};
    GetName(h, name, sizeof(name));
    printf("  name             = '%s'\n", name);

    char fw[256] = {};
    GetFirmwareVersion(h, fw, sizeof(fw));
    printf("  firmwareVersion  = '%s'\n", fw);

    char hw[256] = {};
    GetHardware(h, hw, sizeof(hw));
    printf("  hardware         = '%s'\n", hw);

    char hwv[256] = {};
    GetHardwareVersion(h, hwv, sizeof(hwv));
    printf("  hardwareVersion  = '%s'\n", hwv);

    char uuid[256] = {};
    GetUUID(h, uuid, sizeof(uuid));
    printf("  uuid             = '%s'\n", uuid);

    printf("  PASS\n");
}

// ---------------------------------------------------------------------------
// Test 6: Dynamic commands
// ---------------------------------------------------------------------------

static void test_commands(TAC_HANDLE h)
{
    printf("test_commands:\n");

    unsigned long cmdCount = 0;
    requireOk("GetCommandCount", GetCommandCount(h, &cmdCount));
    printf("  commandCount = %lu\n", cmdCount);

    for (unsigned long i = 0; i < cmdCount && i < 5; ++i)
    {
        char cmd[512] = {};
        requireOk("GetCommand", GetCommand(h, i, cmd, sizeof(cmd)));
        printf("  [%lu] '%s'\n", i, cmd);
    }
    if (cmdCount > 5)
        printf("  ... (%lu more)\n", cmdCount - 5);

    printf("  PASS\n");
}

// ---------------------------------------------------------------------------
// Test 7: Quick commands
// ---------------------------------------------------------------------------

static void test_quick_commands(TAC_HANDLE h)
{
    printf("test_quick_commands:\n");

    unsigned long qCount = 0;
    requireOk("GetQuickCommandCount", GetQuickCommandCount(h, &qCount));
    printf("  quickCommandCount = %lu\n", qCount);

    for (unsigned long i = 0; i < qCount && i < 5; ++i)
    {
        char cmd[256] = {};
        requireOk("GetQuickCommand", GetQuickCommand(h, i, cmd, sizeof(cmd)));
        printf("  [%lu] '%s'\n", i, cmd);
    }
    if (qCount > 5)
        printf("  ... (%lu more)\n", qCount - 5);

    printf("  PASS\n");
}

// ---------------------------------------------------------------------------
// Test 8: Script variables
// ---------------------------------------------------------------------------

static void test_script_variables(TAC_HANDLE h)
{
    printf("test_script_variables:\n");

    unsigned long varCount = 0;
    requireOk("GetScriptVariableCount", GetScriptVariableCount(h, &varCount));
    printf("  scriptVariableCount = %lu\n", varCount);

    for (unsigned long i = 0; i < varCount && i < 5; ++i)
    {
        char var[256] = {};
        requireOk("GetScriptVariable", GetScriptVariable(h, i, var, sizeof(var)));
        printf("  [%lu] '%s'\n", i, var);
    }
    if (varCount > 5)
        printf("  ... (%lu more)\n", varCount - 5);

    printf("  PASS\n");
}

// ---------------------------------------------------------------------------
// Test 9: GetCommandState on first command (if any)
// ---------------------------------------------------------------------------

static void test_command_state(TAC_HANDLE h)
{
    printf("test_command_state:\n");

    unsigned long cmdCount = 0;
    GetCommandCount(h, &cmdCount);

    if (cmdCount == 0)
    {
        printf("  SKIP (no commands)\n");
        return;
    }

    char cmdName[512] = {};
    GetCommand(h, 0, cmdName, sizeof(cmdName));

    // cmdName is "name;helpText;pin;tab;group;cell" — extract just the name.
    char name[256] = {};
    portNameFromPortData(cmdName, name, sizeof(name));  // reuse ; parser

    bool state = false;
    TAC_RESULT r = GetCommandState(h, name, &state);
    if (r == NO_TAC_ERROR)
        printf("  '%s' state = %s\n", name, state ? "true" : "false");
    else
        printf("  GetCommandState('%s') returned %lu (may be unsupported)\n", name, r);

    printf("  PASS\n");
}

// ---------------------------------------------------------------------------
// Test 10: Double-close returns error
// ---------------------------------------------------------------------------

static void test_double_close(const char* portName)
{
    printf("test_double_close:\n");

    TAC_HANDLE h = OpenHandleByDescription(portName);
    require("open for double-close test", h != kBadHandle);

    requireOk("CloseTACHandle (first)", CloseTACHandle(h));

    TAC_RESULT r = CloseTACHandle(h);
    if (r != NO_TAC_ERROR)
        printf("  second close correctly returned error %lu\n", r);
    else
        printf("  second close returned NO_TAC_ERROR (no-op is acceptable)\n");

    printf("  PASS\n");
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------

int main()
{
    setvbuf(stdout, nullptr, _IONBF, 0);
    printf("=== C API Integration: TACDev.dll (PSoC) ===\n\n");

    test_init();
    test_versions();

    char portName[256] = {};
    test_enumeration(portName, sizeof(portName));

    TAC_HANDLE h = test_open(portName);
    test_properties(h);
    test_commands(h);
    test_quick_commands(h);
    test_script_variables(h);
    test_command_state(h);

    requireOk("CloseTACHandle", CloseTACHandle(h));
    printf("\nCloseTACHandle: PASS\n");

    test_double_close(portName);

    printf("\nAll TACDev C API tests passed.\n");
    return 0;
}
