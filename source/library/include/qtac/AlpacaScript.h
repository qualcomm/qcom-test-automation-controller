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

// Qt-free port of AlpacaScript from qcommon-console.
// Parses the tcnf "script" DSL into CommandEntries that can be passed
// to TACDriveThread::sendCommandSequence().

#pragma once

#include <qtac/ByteArray.h>
#include <qtac/String.h>
#include <qtac/Map.h>
#include <qtac/Variant.h>
#include <qtac/TACCommand.h>    // TACCommands
#include <qtac/TACDriveThread.h>   // CommandEntry / CommandEntries

#include <memory>
#include <vector>

namespace qtac {

// -----------------------------------------------------------------------
// VariableEntry — mirrors ScriptVariable from qcommon-console.
// -----------------------------------------------------------------------
enum class VariableType
{
    Unknown = 0,
    Integer = 1,
    Boolean = 2,
    Float   = 3
};

struct VariableEntry
{
    qtac::String   _name;
    qtac::String   _label;
    qtac::String   _tooltip;
    VariableType   _type{VariableType::Unknown};
    qtac::Variant  _defaultValue{0};
    int            _cellX{-1};
    int            _cellY{-1};
};

using VariableEntries = qtac::Map<qtac::String, VariableEntry>;

// -----------------------------------------------------------------------
// ButtonEntry — mirrors Button from qcommon-console.
// -----------------------------------------------------------------------
struct ButtonEntry
{
    qtac::String   _name;
    qtac::String   _command;    // script function name to invoke
    int            _commandGroup{0};
    qtac::String   _tab;
    qtac::String   _tooltip;
    int            _cellX{-1};
    int            _cellY{-1};
};

using ButtonEntries = std::vector<ButtonEntry>;

// -----------------------------------------------------------------------
// Internal parse tree node
// -----------------------------------------------------------------------
struct _ScriptCommand
{
    qtac::ByteArray  _command;
    CommandEntries   _subCommands;

    bool isEmpty() const { return _command.isEmpty() || _subCommands.empty(); }
};

using ScriptCommand  = std::shared_ptr<_ScriptCommand>;
using ScriptCommands = qtac::Map<qtac::String, ScriptCommand>;

// -----------------------------------------------------------------------
// AlpacaScript — Qt-free port of AlpacaScript from qcommon-console.
// -----------------------------------------------------------------------
class AlpacaScript
{
public:
    AlpacaScript() = default;

    // Parse the tcnf "script" string.  tacCommands maps command names to pins.
    // Returns false if any unresolvable tokens were encountered.
    bool parseScript(const qtac::String&     alpacaScript,
                     const VariableEntries&  variables,
                     const TACCommands&      tacCommands);

    bool hasCommand(const qtac::ByteArray& command) const;
    bool isEmpty() const { return _scriptCommands.isEmpty(); }

    // Retrieve the flat CommandEntries for a named function, resolving
    // sub-function calls up to 7 levels deep.
    CommandEntries getCommandEntries(const qtac::ByteArray& command) const;

    // Copy commandEntries, substituting $VarName tokens with values from vars.
    static CommandEntries replaceTokens(const VariableEntries&  variables,
                                        const CommandEntries&   commandEntries);

private:
    static bool isVariableRef(const qtac::String& token);
    static qtac::String stripParens(const qtac::String& s);

    ScriptCommand getCommand(const qtac::ByteArray& command) const;

    void processCommandEntries(ScriptCommand       scriptCommand,
                               CommandEntries&     out,
                               int                 level) const;

    ScriptCommands _scriptCommands;
};

} // namespace qtac
