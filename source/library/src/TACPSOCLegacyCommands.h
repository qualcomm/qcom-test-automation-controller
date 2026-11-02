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

// Qt-free port of PSOCLegacyCommands from qcommon-console.
// Handles translation of pin commands for older PSoC firmware versions
// that predate certain pin IDs.

#pragma once

#include <qtac/ByteArray.h>
#include <qtac/String.h>

#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <cstdint>

// ---------------------------------------------------------------------------
// PSOCLegacyCommands
//
// Some older PSoC firmware revisions do not recognise certain pin IDs or
// use a different command encoding for them.  This class:
//   - Is configured once after the firmware version string is parsed.
//   - Translates "pin <state> <id>" commands to a legacy form when needed.
//   - Tracks pins rejected by the firmware and suppresses their retransmission.
//   - Can be put in "auto" mode (detect from rejected commands), "on" (always
//     use legacy), or "off" (never use legacy, compile-time disable).
// ---------------------------------------------------------------------------
class PSOCLegacyCommands
{
public:
    enum class Mode { Auto, On, Off };

    PSOCLegacyCommands();
    ~PSOCLegacyCommands();

    PSOCLegacyCommands(const PSOCLegacyCommands&)            = delete;
    PSOCLegacyCommands& operator=(const PSOCLegacyCommands&) = delete;

    // Called after the firmware version string is parsed.
    // major/chip/minor/revision are the four fields of "a.b.c.d".
    void configure(unsigned int major, unsigned int chip,
                   unsigned int minor, unsigned int revision,
                   const qtac::String& portName);

    // Returns a non-empty legacy command string if the given pin/state needs
    // legacy translation; returns an empty ByteArray otherwise.
    qtac::ByteArray translate(bool state, uint64_t pin) const;

    // Called when the firmware rejects a command with "CMD: Command not recognized."
    // The echoed command text (the line the firmware echoed back before the error)
    // is passed so the pin ID can be extracted and remembered.
    void onCommandRejected(const qtac::ByteArray& echoedCommand);

    // Activate / deactivate legacy mode at runtime.
    void activate(bool active);
    Mode mode() const;

    // Returns false if legacy support was compiled out via QTAC_PSOC_LEGACY_COMMANDS=OFF.
    static bool compiledIn();

    // Returns true if the configured firmware predates the given pin ID.
    bool firmwarePredatesPin(uint64_t pin) const;

private:
    Mode         _mode{Mode::Auto};
    unsigned int _major{0};
    unsigned int _chip{0};
    unsigned int _minor{0};
    unsigned int _revision{0};
    qtac::String _portName;
    bool         _configured{false};

    mutable std::mutex _mutex;

    // Pins that the firmware has rejected (only in Auto mode).
    std::unordered_set<uint64_t> _rejectedPins;

    // Internal helpers
    qtac::ByteArray buildLegacyCommand(bool state, uint64_t pin) const;
    bool            shouldUseLegacy(uint64_t pin) const;
};
