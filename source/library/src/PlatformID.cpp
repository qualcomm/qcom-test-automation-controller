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
#include <system_error>
#ifdef _WIN32
#  include <windows.h>
#else
#  include <sys/stat.h>
#  include <unistd.h>
#  include <limits.h>
#endif

// -----------------------------------------------------------------------
// Minimal C++11-compatible filesystem helpers (replaces std::filesystem,
// which is C++17 and not available on GCC/Clang with -std=c++11).
// -----------------------------------------------------------------------
namespace fs_compat {

class Path
{
public:
    Path() {}
    Path(const std::string& s) : _p(s) { normalize(); }
    Path(const char* s) : _p(s ? s : "") { normalize(); }

    Path operator/(const std::string& rhs) const
    {
        if (_p.empty()) return Path(rhs);
        Path r(_p + "/" + rhs);
        return r;
    }
    Path operator/(const char* rhs) const { return operator/(std::string(rhs ? rhs : "")); }
    Path operator/(const Path& rhs) const { return operator/(rhs._p); }

    Path parent_path() const
    {
        size_t pos = _p.find_last_of('/');
        if (pos == std::string::npos) return Path();
        if (pos == 0) return Path("/");
        return Path(_p.substr(0, pos));
    }

    Path filename() const
    {
        size_t pos = _p.find_last_of('/');
        if (pos == std::string::npos) return Path(_p);
        return Path(_p.substr(pos + 1));
    }

    bool is_relative() const
    {
        if (_p.empty()) return true;
#ifdef _WIN32
        if (_p.size() >= 3 && std::isalpha((unsigned char)_p[0]) && _p[1] == ':' && _p[2] == '/')
            return false;
        if (_p.size() >= 2 && _p[0] == '/' && _p[1] == '/')
            return false;
#else
        if (_p[0] == '/') return false;
#endif
        return true;
    }

    bool operator==(const Path& o) const { return _p == o._p; }
    bool operator!=(const Path& o) const { return _p != o._p; }

    std::string string() const { return _p; }
    bool empty() const { return _p.empty(); }

private:
    void normalize()
    {
        // Unify separators to forward slash
        for (char& c : _p) if (c == '\\') c = '/';
        // Remove trailing slash unless it is the root
        while (_p.size() > 1 && _p.back() == '/')
            _p.pop_back();
    }
    std::string _p;
};

static bool exists(const Path& p)
{
    if (p.empty()) return false;
#ifdef _WIN32
    return ::GetFileAttributesA(p.string().c_str()) != INVALID_FILE_ATTRIBUTES;
#else
    struct stat st;
    return ::stat(p.string().c_str(), &st) == 0;
#endif
}

// Resolve to an absolute, normalised path.  On POSIX this also resolves
// symlinks via realpath(); on Windows it uses GetFullPathNameA().
// ec is set on failure (e.g. path does not exist on POSIX).
static Path canonical(const Path& p, std::error_code& ec)
{
    ec.clear();
#ifdef _WIN32
    char buf[MAX_PATH]{};
    DWORD len = ::GetFullPathNameA(p.string().c_str(), MAX_PATH, buf, nullptr);
    if (len == 0 || len >= MAX_PATH)
    {
        ec = std::error_code(static_cast<int>(::GetLastError()), std::system_category());
        return p;
    }
    return Path(buf);
#else
    char buf[PATH_MAX]{};
    if (!::realpath(p.string().c_str(), buf))
    {
        ec = std::error_code(errno, std::generic_category());
        return p;
    }
    return Path(buf);
#endif
}

static Path current_path()
{
#ifdef _WIN32
    char buf[MAX_PATH]{};
    ::GetCurrentDirectoryA(MAX_PATH, buf);
    return Path(buf);
#else
    char buf[PATH_MAX]{};
    if (::getcwd(buf, sizeof(buf)))
        return Path(buf);
    return Path();
#endif
}

} // namespace fs_compat

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
static std::string walkUpForDeviceList(const fs_compat::Path& start,
                                       int maxLevels)
{
    static const char* kFilename = "devicelist.json";
    static const char* kSubdir   = "configurations";

    fs_compat::Path dir = start;
    for (int i = 0; i <= maxLevels; ++i)
    {
        // Direct file alongside the directory
        {
            auto candidate = dir / kFilename;
            if (fs_compat::exists(candidate))
                return candidate.string();
        }
        // configurations/ subdirectory
        {
            auto candidate = dir / kSubdir / kFilename;
            std::error_code ec;
            auto canonical = fs_compat::canonical(candidate, ec);
            if (!ec && fs_compat::exists(canonical))
                return canonical.string();
        }

        fs_compat::Path parent = dir.parent_path();
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
    if (envPath && fs_compat::exists(fs_compat::Path(envPath)))
        return envPath;

    // 2. Walk upward from executable directory (up to 8 levels)
#ifdef __linux__
    {
        char buf[4096]{};
        ssize_t len = ::readlink("/proc/self/exe", buf, sizeof(buf) - 1);
        if (len > 0)
        {
            fs_compat::Path exeDir = fs_compat::Path(buf).parent_path();
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
                fs_compat::Path dllDir = fs_compat::Path(buf).parent_path();
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
                fs_compat::Path exeDir = fs_compat::Path(buf).parent_path();
                std::string found = walkUpForDeviceList(exeDir, 8);
                if (!found.empty()) return found;
            }
        }

        // Installed Alpaca layout: C:/ProgramData/Qualcomm/Alpaca/tac_configs/
        {
            char programData[MAX_PATH]{};
            if (::GetEnvironmentVariableA("ProgramData", programData, MAX_PATH) > 0)
            {
                fs_compat::Path tacConfigs =
                    fs_compat::Path(programData) / "Qualcomm" / "Alpaca" / "tac_configs";
                auto candidate = tacConfigs / "DeviceList.json";
                if (fs_compat::exists(candidate))
                    return candidate.string();
            }
        }
    }
#endif

    // 3. Walk upward from current working directory (up to 8 levels)
    {
        std::string found = walkUpForDeviceList(fs_compat::current_path(), 8);
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
    const auto* catalog = rootObj->at(kCatalog).if_array();
    if (!catalog)
        return;

    // Resolve the directory that contains devicelist.json so that relative
    // configPath values can be resolved against it.
    fs_compat::Path deviceListDir =
        fs_compat::Path(deviceListPath).parent_path();

    for (const auto& entryVal : *catalog)
    {
        const auto* entry = entryVal.if_object();
        if (!entry || !entry->contains(kPlatformId))
            continue;

        PlatformID pid = static_cast<PlatformID>(qtac::json_util::toInt(entry->at(kPlatformId)));

        int boardTypeInt = qtac::json_util::valueInt(*entry, kDebugBoardType, 0);
        DebugBoardType boardType = static_cast<DebugBoardType>(boardTypeInt);

        std::string description = qtac::json_util::valueString(*entry, kDescription, "");
        std::string usbDesc     = qtac::json_util::valueString(*entry, kUsbDescriptor, "");
        std::string configPath  = qtac::json_util::valueString(*entry, kConfigPath, "");

        // Resolve configPath relative to devicelist.json location when it is
        // a relative path.
        if (!configPath.empty())
        {
            fs_compat::Path p(configPath);
            if (p.is_relative())
            {
                // Primary: resolve relative to devicelist.json's directory
                std::error_code ec;
                auto resolved = fs_compat::canonical(deviceListDir / p, ec);
                if (!ec && fs_compat::exists(resolved))
                {
                    configPath = resolved.string();
                }
                else
                {
                    // Fallback: look for just the filename alongside devicelist.json
                    auto fallback = deviceListDir / p.filename();
                    if (fs_compat::exists(fallback))
                        configPath = fallback.string();
                    else
                        configPath = (deviceListDir / p).string();
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
                    static_cast<FTDIPinSets>(qtac::json_util::toInt(entry->at(kBusSets[i])));
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
