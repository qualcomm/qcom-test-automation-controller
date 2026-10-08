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

#ifndef QTAC_TCNFLOADER_H
#define QTAC_TCNFLOADER_H

#include <qtac/FTDIPlatformConfiguration.h>
#include <qtac/PSOCPlatformConfiguration.h>
#include <qtac/PIC32CXPlatformConfiguration.h>
#include <qtac/String.h>

#include <memory>
#include <string>

// -----------------------------------------------------------------------
// TcnfLoader
//
// Opens a .tcnf file (JSON) and populates the appropriate platform
// configuration object.  Returns false if the file cannot be opened or
// parsed, or if the platform_type does not match the requested type.
//
// Usage:
//   // PSOC device open():
//   auto* cfg = new _PSOCPlatformConfiguration;
//   TcnfLoader::loadPSOC(platformEntry->_path.toStdString(), cfg);
//
//   // FTDI device open():
//   auto* cfg = new _FTDIPlatformConfiguration(chipCount);
//   TcnfLoader::loadFTDI(platformEntry->_path.toStdString(), cfg);
//
//   // PIC32CX device open():
//   auto* cfg = new _PIC32CXPlatformConfiguration;
//   TcnfLoader::loadPIC32CX(platformEntry->_path.toStdString(), cfg);
// -----------------------------------------------------------------------

class TcnfLoader
{
public:
    TcnfLoader() = delete;

    static bool loadPSOC   (const std::string& path, _PSOCPlatformConfiguration*    cfg);
    static bool loadFTDI   (const std::string& path, _FTDIPlatformConfiguration*    cfg);
    static bool loadPIC32CX(const std::string& path, _PIC32CXPlatformConfiguration* cfg);
};

#endif // QTAC_TCNFLOADER_H
