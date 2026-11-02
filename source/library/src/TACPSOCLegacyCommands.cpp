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

#include <qtac/TACPSOCLegacyCommands.h>

#include <cstdlib>
#include <sstream>
#include <string>

// ---------------------------------------------------------------------------
// PSoC firmware version threshold below which legacy pin commands are needed.
// Firmware with minor < kLegacyFirmwareMinorThreshold uses the older encoding.
// ---------------------------------------------------------------------------
static constexpr unsigned int kLegacyFirmwareMinorThreshold = 15;

// ---------------------------------------------------------------------------

PSOCLegacyCommands::PSOCLegacyCommands()
{
    // Read the TAC_PSOC_LEGACY_COMMANDS environment variable to override mode.
    const char* envVar = std::getenv("TAC_PSOC_LEGACY_COMMANDS");
    if (envVar)
    {
        std::string val(envVar);
        if (val == "on")
            _mode = Mode::On;
        else if (val == "off")
            _mode = Mode::Off;
        // else "auto" (default)
    }
}

PSOCLegacyCommands::~PSOCLegacyCommands() = default;

void PSOCLegacyCommands::configure(unsigned int major, unsigned int chip,
                                    unsigned int minor, unsigned int revision,
                                    const qtac::String& portName)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _major      = major;
    _chip       = chip;
    _minor      = minor;
    _revision   = revision;
    _portName   = portName;
    _configured = true;
    _rejectedPins.clear();
}

bool PSOCLegacyCommands::firmwarePredatesPin(uint64_t /*pin*/) const
{
    // Current heuristic: firmware with minor < threshold uses legacy encoding.
    return _configured && (_minor < kLegacyFirmwareMinorThreshold);
}

bool PSOCLegacyCommands::shouldUseLegacy(uint64_t pin) const
{
    if (_mode == Mode::Off)
        return false;
    if (_mode == Mode::On)
        return true;
    // Auto: use legacy if firmware predates pin or pin was previously rejected.
    return firmwarePredatesPin(pin) || (_rejectedPins.count(pin) > 0);
}

qtac::ByteArray PSOCLegacyCommands::buildLegacyCommand(bool state, uint64_t pin) const
{
    // Legacy PSoC command format: "setpin <pin> <on|off>\r"
    std::ostringstream oss;
    oss << "setpin " << pin << " " << (state ? "on" : "off") << "\r";
    return qtac::ByteArray(oss.str().c_str());
}

qtac::ByteArray PSOCLegacyCommands::translate(bool state, uint64_t pin) const
{
    if (!compiledIn())
        return qtac::ByteArray();

    std::lock_guard<std::mutex> lock(_mutex);
    if (!shouldUseLegacy(pin))
        return qtac::ByteArray();

    return buildLegacyCommand(state, pin);
}

void PSOCLegacyCommands::onCommandRejected(const qtac::ByteArray& echoedCommand)
{
    if (!compiledIn() || _mode == Mode::Off)
        return;

    // The echoed command is the "pin <on|off> <pinid>" string the firmware
    // echoed back.  Extract the pin ID from the third whitespace-separated token.
    const std::string s = echoedCommand.toStdString();
    std::istringstream iss(s);
    std::string tok;
    int tokenIndex = 0;
    while (iss >> tok)
    {
        if (tokenIndex == 2)
        {
            try
            {
                uint64_t pin = static_cast<uint64_t>(std::stoull(tok));
                std::lock_guard<std::mutex> lock(_mutex);
                _rejectedPins.insert(pin);
            }
            catch (...) {}
            break;
        }
        ++tokenIndex;
    }
}

void PSOCLegacyCommands::activate(bool active)
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (_mode != Mode::Off)
        _mode = active ? Mode::On : Mode::Auto;
}

PSOCLegacyCommands::Mode PSOCLegacyCommands::mode() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _mode;
}

// static
bool PSOCLegacyCommands::compiledIn()
{
#if defined(QTAC_PSOC_LEGACY_COMMANDS)
    return QTAC_PSOC_LEGACY_COMMANDS;
#else
    return true; // default: compiled in
#endif
}
