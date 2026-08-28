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

// Author: Michael Simpson

#include <qtac/PlatformID.h>
#include <qtac/json_util.h>

#include <fstream>
#include <filesystem>
#ifdef _WIN32
#  include <windows.h>
#else
#  include <unistd.h>
#endif

// -----------------------------------------------------------------------
// Static storage
// -----------------------------------------------------------------------

PlatformIDs PlatformContainer::_platformIds;

// -----------------------------------------------------------------------
// JSON key constants (mirrors devicelist.json schema)
// -----------------------------------------------------------------------

static const char* kCatalog        = "catalog";
static const char* kPlatformId     = "platform_id";
static const char* kDebugBoardType = "debugBoardType";
static const char* kDescription    = "description";
static const char* kName           = "name";
static const char* kUsbDescriptor  = "usb_descriptor";
static const char* kConfigPath     = "configPath";
static const char* kRevision       = "revision";
static const char* kFirmwareChip   = "firmware_chip";
static const char* kChip1BusSet    = "chip1BusSet";
static const char* kChip2BusSet    = "chip2BusSet";
static const char* kChip3BusSet    = "chip3BusSet";
static const char* kChip4BusSet    = "chip4BusSet";

// -----------------------------------------------------------------------
// Locate devicelist.json
//
// Search order (first found wins):
//   1. QTAC_DEVICELIST env var (explicit override)
//   2. Executable directory
//   3. Current working directory
//   4. Four levels up from current working directory
//      (mirrors the relative path used inside devicelist.json itself:
//       "configPath": "../../../../configurations/...")
// -----------------------------------------------------------------------

// Walk upward from |start| up to |maxLevels| looking for
// configurations/devicelist.json.  Returns the canonical path on success.
static std::string walkUpForDeviceList(const std::filesystem::path& start,
                                       int maxLevels)
{
    static const char* kFilename = "devicelist.json";
    static const char* kSubdir   = "configurations";

    std::filesystem::path dir = start;
    for (int i = 0; i <= maxLevels; ++i)
    {
        // Direct file alongside the directory
        {
            auto candidate = dir / kFilename;
            if (std::filesystem::exists(candidate))
                return candidate.string();
        }
        // configurations/ subdirectory
        {
            auto candidate = dir / kSubdir / kFilename;
            std::error_code ec;
            auto canonical = std::filesystem::canonical(candidate, ec);
            if (!ec && std::filesystem::exists(canonical))
                return canonical.string();
        }

        std::filesystem::path parent = dir.parent_path();
        if (parent == dir)
            break; // reached filesystem root
        dir = parent;
    }
    return {};
}

static std::string findDeviceList()
{
    // 1. Environment variable override
    const char* envPath = std::getenv("QTAC_DEVICELIST");
    if (envPath && std::filesystem::exists(envPath))
        return envPath;

    // 2. Walk upward from executable directory (up to 8 levels)
#ifdef __linux__
    {
        char buf[4096]{};
        ssize_t len = ::readlink("/proc/self/exe", buf, sizeof(buf) - 1);
        if (len > 0)
        {
            std::filesystem::path exeDir = std::filesystem::path(buf).parent_path();
            std::string found = walkUpForDeviceList(exeDir, 8);
            if (!found.empty()) return found;
        }
    }
#endif
#ifdef _WIN32
    {
        // Prefer the DLL's own directory so ctypes / hosted scenarios (where
        // GetModuleFileNameA(nullptr) returns python.exe, not this DLL) still
        // find the devicelist.json that was deployed alongside TACDev.dll.
        HMODULE hSelf = nullptr;
        ::GetModuleHandleExA(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCSTR>(&findDeviceList),
            &hSelf);
        if (hSelf)
        {
            char buf[MAX_PATH]{};
            DWORD len = ::GetModuleFileNameA(hSelf, buf, MAX_PATH);
            if (len > 0)
            {
                std::filesystem::path dllDir = std::filesystem::path(buf).parent_path();
                std::string found = walkUpForDeviceList(dllDir, 8);
                if (!found.empty()) return found;
            }
        }

        // Fall back to the host process exe directory (e.g. TAC.exe, TACDev.exe).
        {
            char buf[MAX_PATH]{};
            DWORD len = ::GetModuleFileNameA(nullptr, buf, MAX_PATH);
            if (len > 0)
            {
                std::filesystem::path exeDir = std::filesystem::path(buf).parent_path();
                std::string found = walkUpForDeviceList(exeDir, 8);
                if (!found.empty()) return found;
            }
        }

        // Installed Alpaca layout: C:/ProgramData/Qualcomm/Alpaca/tac_configs/
        {
            char programData[MAX_PATH]{};
            if (::GetEnvironmentVariableA("ProgramData", programData, MAX_PATH) > 0)
            {
                std::filesystem::path tacConfigs =
                    std::filesystem::path(programData) / "Qualcomm" / "Alpaca" / "tac_configs";
                auto candidate = tacConfigs / "DeviceList.json";
                if (std::filesystem::exists(candidate))
                    return candidate.string();
            }
        }
    }
#endif

    // 3. Walk upward from current working directory (up to 8 levels)
    {
        std::string found = walkUpForDeviceList(std::filesystem::current_path(), 8);
        if (!found.empty()) return found;
    }

    return {};
}

// -----------------------------------------------------------------------
// Public API
// -----------------------------------------------------------------------

void PlatformContainer::initialize()
{
    if (!_platformIds.isEmpty())
        return;

    initializeDynamic();
}

void PlatformContainer::initializeDynamic()
{
    std::string deviceListPath = findDeviceList();
    if (deviceListPath.empty())
        return;

    std::ifstream file(deviceListPath);
    if (!file.is_open())
        return;

    std::string fileContents((std::istreambuf_iterator<char>(file)),
                              std::istreambuf_iterator<char>());

    json_t root;
    try { root = boost::json::parse(fileContents); }
    catch (...) { return; }

    const auto* rootObj = root.if_object();
    if (!rootObj || !rootObj->contains(kCatalog))
        return;
    const auto* catalog = (*rootObj)[kCatalog].if_array();
    if (!catalog)
        return;

    // Resolve the directory that contains devicelist.json so that relative
    // configPath values can be resolved against it.
    std::filesystem::path deviceListDir =
        std::filesystem::path(deviceListPath).parent_path();

    for (const auto& entryVal : *catalog)
    {
        const auto* entry = entryVal.if_object();
        if (!entry || !entry->contains(kPlatformId))
            continue;

        PlatformID pid = static_cast<PlatformID>(qtac::json_util::toInt((*entry)[kPlatformId]));

        int boardTypeInt = qtac::json_util::valueInt(*entry, kDebugBoardType, 0);
        DebugBoardType boardType = static_cast<DebugBoardType>(boardTypeInt);

        std::string description = qtac::json_util::valueString(*entry, kDescription, "");
        std::string usbDesc     = qtac::json_util::valueString(*entry, kUsbDescriptor, "");
        std::string configPath  = qtac::json_util::valueString(*entry, kConfigPath, "");

        // Resolve configPath relative to devicelist.json location when it is
        // a relative path.
        if (!configPath.empty())
        {
            std::filesystem::path p(configPath);
            if (p.is_relative())
            {
                // Primary: resolve relative to devicelist.json's directory
                // (works in the repo where devicelist.json sits next to configurations/)
                std::error_code ec;
                auto resolved = std::filesystem::canonical(deviceListDir / p, ec);
                if (!ec && std::filesystem::exists(resolved))
                {
                    configPath = resolved.string();
                }
                else
                {
                    // Fallback: look for just the filename alongside devicelist.json
                    // (deployed layout where all configs are copied to the exe dir)
                    auto fallback = deviceListDir / p.filename();
                    if (std::filesystem::exists(fallback))
                        configPath = fallback.string();
                    else
                        configPath = (deviceListDir / p).string(); // keep original for error reporting
                }
            }
        }

        auto platformEntry = std::make_shared<_PlatformEntry>(
            pid,
            boardType,
            qtac::String(description),
            qtac::String(configPath),
            qtac::ByteArray(usbDesc.c_str())
        );

        platformEntry->_revision     = static_cast<uint32_t>(qtac::json_util::valueInt(*entry, kRevision, 0));
        platformEntry->_firmwareChip = static_cast<uint32_t>(qtac::json_util::valueInt(*entry, kFirmwareChip, 0));

        // FTDI bus set bitmasks (chip1..chip4, 0-indexed internally as [0..3])
        static const char* kBusSets[kMaxPinSetCount] = {
            kChip1BusSet, kChip2BusSet, kChip3BusSet, kChip4BusSet
        };
        for (int i = 0; i < kMaxPinSetCount; ++i)
        {
            if (entry->contains(kBusSets[i]))
                platformEntry->_pinSets[i] =
                    static_cast<FTDIPinSets>(qtac::json_util::toInt((*entry)[kBusSets[i]]));
        }

        _platformIds.insert(pid, platformEntry);
    }
}

void PlatformContainer::addEntry(PlatformEntry platformEntry)
{
    if (platformEntry)
        _platformIds.insert(platformEntry->_platformID, platformEntry);
}

PlatformIDList PlatformContainer::getEntries()
{
    initialize();
    PlatformIDList result;
    for (const auto& kv : _platformIds)
        result.append(kv.second);
    return result;
}

qtac::String PlatformContainer::toString(PlatformID platformID)
{
    initialize();
    auto it = _platformIds.find(platformID);
    if (it != _platformIds.end() && it->second)
        return it->second->_description;
    return qtac::String();
}

PlatformID PlatformContainer::fromUSBDescriptor(const qtac::ByteArray& usbDescriptor)
{
    initialize();
    qtac::ByteArray target = usbDescriptor.toLower();
    for (const auto& kv : _platformIds)
    {
        if (!kv.second) continue;
        qtac::ByteArray desc = kv.second->_usbDescriptor.toLower();
        if (!desc.isEmpty() && desc == target)
            return kv.second->_platformID;
    }
    return MICRO_EPM_BOARD_ID_UNKNOWN;
}

PlatformID PlatformContainer::fromRevision(uint32_t revision, DebugBoardType boardType)
{
    initialize();
    if (revision == 0)
        return MICRO_EPM_BOARD_ID_UNKNOWN;
    for (const auto& kv : _platformIds)
    {
        if (!kv.second) continue;
        if (kv.second->_revision == revision && kv.second->_boardtype == boardType)
            return kv.second->_platformID;
    }
    return MICRO_EPM_BOARD_ID_UNKNOWN;
}

PlatformID PlatformContainer::fromFirmwareChip(uint32_t firmwareChip, DebugBoardType boardType)
{
    initialize();
    if (firmwareChip == 0)
        return MICRO_EPM_BOARD_ID_UNKNOWN;
    for (const auto& kv : _platformIds)
    {
        if (!kv.second) continue;
        if (kv.second->_firmwareChip == firmwareChip &&
            kv.second->_boardtype    == boardType     &&
            !kv.second->_path.isEmpty())
            return kv.second->_platformID;
    }
    return MICRO_EPM_BOARD_ID_UNKNOWN;
}

DebugBoardType PlatformContainer::getDebugBoardType(PlatformID platformID){
    initialize();
    auto it = _platformIds.find(platformID);
    if (it != _platformIds.end() && it->second)
        return it->second->_boardtype;
    return eUnknownDebugBoard;
}

PlatformIDList PlatformContainer::getDebugBoards()
{
    initialize();
    PlatformIDList result;
    for (const auto& kv : _platformIds)
        if (kv.second && kv.second->_boardtype != eUnknownDebugBoard)
            result.append(kv.second);
    return result;
}

PlatformIDList PlatformContainer::getDebugBoardsOfType(DebugBoardType debugBoardType)
{
    initialize();
    PlatformIDList result;
    for (const auto& kv : _platformIds)
        if (kv.second && kv.second->_boardtype == debugBoardType)
            result.append(kv.second);
    return result;
}
