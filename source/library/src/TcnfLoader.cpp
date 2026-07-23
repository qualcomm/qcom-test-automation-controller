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

#include <qtac/TcnfLoader.h>

#include <fstream>
#include <sstream>

// -----------------------------------------------------------------------

static bool openAndParse(const std::string& path, json_t& out)
{
    if (path.empty()) return false;
    std::ifstream file(path);
    if (!file.is_open()) return false;
    try { out = json_t::parse(file); }
    catch (...) { return false; }
    return true;
}

static std::string readTextFile(const std::string& path)
{
    std::ifstream f(path);
    if (!f.is_open()) return {};
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

// Derive the directory component from a file path (includes trailing slash).
static std::string dirOf(const std::string& path)
{
    auto pos = path.find_last_of("/\\");
    return (pos == std::string::npos) ? std::string() : path.substr(0, pos + 1);
}

bool TcnfLoader::loadPSOC(const std::string& path, _PSOCPlatformConfiguration* cfg)
{
    if (!cfg) return false;
    json_t j;
    if (!openAndParse(path, j)) return false;
    return cfg->read(j);
}

bool TcnfLoader::loadFTDI(const std::string& path, _FTDIPlatformConfiguration* cfg)
{
    if (!cfg) return false;
    json_t j;
    if (!openAndParse(path, j)) return false;
    if (!cfg->read(j)) return false;

    // If the tcnf defined no script, load the sibling DefaultScript.txt so that
    // the default Quick Settings buttons (powerOn, bootToEDL, etc.) are functional.
    if (cfg->getScript().isEmpty())
    {
        std::string defaultScriptPath = dirOf(path) + "DefaultScript.txt";
        std::string text = readTextFile(defaultScriptPath);
        if (!text.empty())
            cfg->loadDefaultScript(qtac::String(text));
    }

    return true;
}

bool TcnfLoader::loadPIC32CX(const std::string& path, _PIC32CXPlatformConfiguration* cfg)
{
    if (!cfg) return false;
    json_t j;
    if (!openAndParse(path, j)) return false;
    return cfg->read(j);
}
