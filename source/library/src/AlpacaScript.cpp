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

#include <qtac/AlpacaScript.h>
#include <qtac/TACCommand.h>

#include <sstream>
#include <stdexcept>

namespace qtac {

// -----------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------

static std::vector<qtac::String> splitLine(const qtac::String& line)
{
    std::vector<qtac::String> tokens;
    std::istringstream ss(line.toStdString());
    std::string tok;
    while (ss >> tok)
        tokens.emplace_back(qtac::String(tok));
    return tokens;
}

static qtac::String trimString(const qtac::String& s)
{
    const std::string& str = s.toStdString();
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return {};
    size_t end = str.find_last_not_of(" \t\r\n");
    return qtac::String(str.substr(start, end - start + 1));
}

static bool startsWith(const qtac::String& s, const char* prefix)
{
    std::string str = s.toStdString();
    return str.rfind(prefix, 0) == 0;
}

static qtac::String toLower(const qtac::String& s)
{
    std::string str = s.toStdString();
    for (auto& c : str) c = static_cast<char>(::tolower(static_cast<unsigned char>(c)));
    return qtac::String(str);
}

// Replace all occurrences of `from` in `str` with `to`.
static qtac::String replaceAll(const qtac::String& str,
                                const qtac::String& from,
                                const qtac::String& to)
{
    std::string s = str.toStdString();
    const std::string& f = from.toStdString();
    const std::string& t = to.toStdString();
    std::string result;
    size_t pos = 0;
    while (pos < s.size())
    {
        size_t found = s.find(f, pos);
        if (found == std::string::npos)
        {
            result += s.substr(pos);
            break;
        }
        result += s.substr(pos, found - pos);
        result += t;
        pos = found + f.size();
    }
    return qtac::String(result);
}

// -----------------------------------------------------------------------

bool AlpacaScript::isVariableRef(const qtac::String& token)
{
    return !token.isEmpty() && token.toStdString()[0] == '$';
}

qtac::String AlpacaScript::stripParens(const qtac::String& s)
{
    qtac::String r = replaceAll(s, "(", "");
    r = replaceAll(r, ")", "");
    return trimString(r);
}

// -----------------------------------------------------------------------
// parseScript
// -----------------------------------------------------------------------

bool AlpacaScript::parseScript(const qtac::String&    alpacaScript,
                                const VariableEntries& variables,
                                const TACCommands&     tacCommands)
{
    bool result = true;
    ScriptCommand current; // current open function definition

    std::istringstream stream(alpacaScript.toStdString());
    std::string rawLine;

    while (std::getline(stream, rawLine))
    {
        qtac::String line = trimString(qtac::String(rawLine));
        auto words = splitLine(line);
        if (words.empty()) continue;

        if (words.size() >= 2 && toLower(words[0]) == "def")
        {
            // Start a new function definition
            current = std::make_shared<_ScriptCommand>();
            qtac::String funcName = stripParens(words[1]);
            current->_command    = funcName.toLatin1();
            _scriptCommands[funcName] = current;
        }
        else if (current && words.size() >= 2
                 && TACCommand::contains(words[0], tacCommands))
        {
            // pinCmd 0 / pinCmd 1
            TACCommand tacCmd = TACCommand::find(words[0], tacCommands);
            auto entry = std::make_shared<_CommandEntry>();
            entry->_commandAction = _CommandEntry::eSetPin;
            entry->_pinID         = static_cast<uint64_t>(tacCmd._pin);
            entry->_action        = words[0].toLatin1();
            entry->_arguement     = (words[1] == "1");
            current->_subCommands.push_back(entry);
        }
        else if (current && words.size() >= 2
                 && toLower(words[0]) == "logcomment")
        {
            // logcomment rest-of-line
            auto entry = std::make_shared<_CommandEntry>();
            entry->_commandAction = _CommandEntry::eLog;
            entry->_action        = qtac::ByteArray("logcomment");
            // Re-join words[1..] for the comment text
            std::string comment;
            for (size_t i = 1; i < words.size(); ++i)
            {
                if (i > 1) comment += ' ';
                comment += words[i].toStdString();
            }
            entry->_arguement = qtac::String(comment);
            current->_subCommands.push_back(entry);
        }
        else if (current && words.size() >= 2
                 && toLower(words[0]) == "delay")
        {
            auto entry = std::make_shared<_CommandEntry>();
            entry->_commandAction = _CommandEntry::eDelay;
            entry->_action        = qtac::ByteArray("delay");

            if (isVariableRef(words[1]))
            {
                // Variable reference: store the "$VarName" string — substituted later
                entry->_arguement = words[1];
                current->_subCommands.push_back(entry);
            }
            else
            {
                bool ok = false;
                const std::string& ds = words[1].toStdString();
                int delayMs = 0;
                try { delayMs = std::stoi(ds); ok = true; }
                catch (...) {}

                if (ok && delayMs > 0 && delayMs < 15000)
                {
                    entry->_arguement = static_cast<unsigned int>(delayMs);
                    current->_subCommands.push_back(entry);
                }
                else
                {
                    result = false;
                }
            }
        }
        else if (current && words.size() == 1)
        {
            // Single word: sub-function call
            if (!startsWith(words[0], "//"))
            {
                auto entry = std::make_shared<_CommandEntry>();
                entry->_commandAction = _CommandEntry::eBaseCommand;
                entry->_action        = words[0].toLatin1();
                current->_subCommands.push_back(entry);
            }
        }
        // else: blank line, comment, or unrecognised — skip silently
    }

    return result;
}

// -----------------------------------------------------------------------
// hasCommand / getCommand
// -----------------------------------------------------------------------

bool AlpacaScript::hasCommand(const qtac::ByteArray& command) const
{
    qtac::String key(command.data(), static_cast<int>(command.size()));
    return _scriptCommands.contains(key);
}

ScriptCommand AlpacaScript::getCommand(const qtac::ByteArray& command) const
{
    qtac::String key(command.data(), static_cast<int>(command.size()));
    auto it = _scriptCommands.find(key);
    if (it != _scriptCommands.end())
        return it->second;
    return {};
}

// -----------------------------------------------------------------------
// getCommandEntries / processCommandEntries
// -----------------------------------------------------------------------

CommandEntries AlpacaScript::getCommandEntries(const qtac::ByteArray& command) const
{
    CommandEntries result;
    ScriptCommand sc = getCommand(command);
    if (sc)
        processCommandEntries(sc, result, 1);
    return result;
}

void AlpacaScript::processCommandEntries(ScriptCommand    sc,
                                          CommandEntries&  out,
                                          int              level) const
{
    if (!sc) return;

    for (const auto& entry : sc->_subCommands)
    {
        switch (entry->_commandAction)
        {
        case _CommandEntry::eSetPin:
        case _CommandEntry::eLog:
        case _CommandEntry::eDelay:
            out.push_back(entry);
            break;

        case _CommandEntry::eBaseCommand:
            if (level < 7)
            {
                ScriptCommand child = getCommand(entry->_action);
                processCommandEntries(child, out, level + 1);
            }
            break;

        default:
            break;
        }
    }
}

// -----------------------------------------------------------------------
// replaceTokens — substitute $VarName in delay and logcomment entries
// -----------------------------------------------------------------------

CommandEntries AlpacaScript::replaceTokens(const VariableEntries& variables,
                                            const CommandEntries&  commandEntries)
{
    // Deep-copy
    CommandEntries result;
    result.reserve(commandEntries.size());
    for (const auto& src : commandEntries)
    {
        auto dst = std::make_shared<_CommandEntry>();
        dst->_commandAction = src->_commandAction;
        dst->_action        = src->_action;
        dst->_arguement     = src->_arguement;
        dst->_pinID         = src->_pinID;
        result.push_back(dst);
    }

    // Replace $VarName tokens
    for (auto& entry : result)
    {
        if (entry->_commandAction == _CommandEntry::eDelay)
        {
            qtac::String varRef = entry->_arguement.toString();
            if (isVariableRef(varRef))
            {
                // Strip '$' to get variable name
                qtac::String varName = qtac::String(varRef.toStdString().substr(1));
                auto it = variables.find(varName);
                if (it != variables.end())
                    entry->_arguement = static_cast<unsigned int>(it->second._defaultValue.toUInt());
            }
        }
        else if (entry->_commandAction == _CommandEntry::eLog)
        {
            qtac::String text = entry->_arguement.toString();
            auto words = splitLine(text);
            std::string out;
            for (size_t i = 0; i < words.size(); ++i)
            {
                if (i > 0) out += ' ';
                if (isVariableRef(words[i]))
                {
                    qtac::String varName = qtac::String(words[i].toStdString().substr(1));
                    auto it = variables.find(varName);
                    if (it != variables.end())
                        out += it->second._defaultValue.toString().toStdString();
                    else
                        out += words[i].toStdString();
                }
                else
                {
                    out += words[i].toStdString();
                }
            }
            entry->_arguement = qtac::String(out);
        }
    }

    return result;
}

} // namespace qtac
