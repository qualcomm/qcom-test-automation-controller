// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "FTDICheckApplication.h"

#include <qtac/version.h>  // TAC_LIB_VERSION

#ifdef _WIN32
#  include <Windows.h>   // Win32 base + winver.h (GetFileVersionInfo*, VerQueryValue*, VS_FIXEDFILEINFO)
#  include <psapi.h>     // EnumDeviceDrivers, GetDeviceDriverBaseName
#endif

#ifdef __linux__
#  include <dirent.h>
#  include <fcntl.h>
#  include <fnmatch.h>
#  include <sys/stat.h>
#  include <unistd.h>
#endif

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// String helpers
// ---------------------------------------------------------------------------

static std::string fileBasename(const std::string& path)
{
    auto pos = path.find_last_of("/\\");
    return pos == std::string::npos ? path : path.substr(pos + 1);
}

// ---------------------------------------------------------------------------
// Windows helpers
// ---------------------------------------------------------------------------

#ifdef _WIN32

// Run an executable synchronously and return its exit code.
// Arguments must NOT be pre-quoted; this function quotes each one.
static int runProcess(const std::string& exe, std::initializer_list<const char*> args)
{
    std::string cmd = "\"" + exe + "\"";
    for (const char* a : args)
    {
        cmd += " \"";
        cmd += a;
        cmd += "\"";
    }

    STARTUPINFOA si{};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi{};

    std::vector<char> buf(cmd.begin(), cmd.end());
    buf.push_back('\0');

    if (!CreateProcessA(nullptr, buf.data(),
                        nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi))
        return -1;

    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD code = 1;
    GetExitCodeProcess(pi.hProcess, &code);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return static_cast<int>(code);
}

// Enumerate files in `dir` matching `pattern` (Win32 wildcard syntax).
// Returns filenames only (not full paths).
static std::vector<std::string> listDir(const std::string& dir, const std::string& pattern)
{
    std::vector<std::string> result;
    WIN32_FIND_DATAA fd{};
    HANDLE h = FindFirstFileA((dir + pattern).c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return result;
    do {
        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            result.push_back(fd.cFileName);
    } while (FindNextFileA(h, &fd));
    FindClose(h);
    return result;
}

#endif // _WIN32

// ---------------------------------------------------------------------------
// Linux helpers
// ---------------------------------------------------------------------------

#ifdef __linux__

static bool fileExists(const std::string& path) { return access(path.c_str(), F_OK) == 0; }
static bool fileWritable(const std::string& path) { return access(path.c_str(), W_OK) == 0; }

static std::string symlinkTarget(const std::string& path)
{
    char buf[4096]{};
    ssize_t n = readlink(path.c_str(), buf, sizeof(buf) - 1);
    return n > 0 ? std::string(buf, static_cast<size_t>(n)) : std::string();
}

static bool copyFile(const std::string& src, const std::string& dst)
{
    int in = open(src.c_str(), O_RDONLY);
    if (in < 0) return false;
    int out = open(dst.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out < 0) { close(in); return false; }
    char buf[65536];
    ssize_t n = 0;
    bool ok = true;
    while ((n = read(in, buf, sizeof(buf))) > 0)
    {
        if (write(out, buf, static_cast<size_t>(n)) != n) { ok = false; break; }
    }
    close(in);
    close(out);
    return ok && n == 0;
}

static std::vector<std::string> listDir(const std::string& dir, const std::string& pattern)
{
    std::vector<std::string> result;
    DIR* d = opendir(dir.c_str());
    if (!d) return result;
    struct dirent* ent;
    while ((ent = readdir(d)) != nullptr)
    {
        if (ent->d_type != DT_REG) continue;
        if (fnmatch(pattern.c_str(), ent->d_name, 0) == 0)
            result.push_back(ent->d_name);
    }
    closedir(d);
    return result;
}

#endif // __linux__

// ---------------------------------------------------------------------------
// INF file parser — detects FTDI driver INFs by checking [Strings] Ftdi=FTDI
// Portable (used on both Windows and Linux paths in uninstallDriver).
// ---------------------------------------------------------------------------

static bool isFTDIInfContent(const std::string& path)
{
    std::ifstream f(path);
    if (!f) return false;

    bool inStrings = false;
    std::string line;
    while (std::getline(f, line))
    {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        size_t s = line.find_first_not_of(" \t");
        if (s == std::string::npos) continue;
        if (line[s] == ';') continue; // INF comment

        if (line[s] == '[')
        {
            size_t e = line.find(']', s + 1);
            std::string sec = line.substr(s + 1, e == std::string::npos ? std::string::npos : e - s - 1);
            std::string lower(sec);
            for (auto& c : lower) c = static_cast<char>(::tolower(static_cast<unsigned char>(c)));
            inStrings = (lower == "strings");
            continue;
        }

        if (!inStrings) continue;

        size_t eq = line.find('=', s);
        if (eq == std::string::npos) continue;

        std::string key = line.substr(s, eq - s);
        size_t ke = key.find_last_not_of(" \t");
        if (ke != std::string::npos) key = key.substr(0, ke + 1);
        std::string klower(key);
        for (auto& c : klower) c = static_cast<char>(::tolower(static_cast<unsigned char>(c)));
        if (klower != "ftdi") continue;

        std::string val = line.substr(eq + 1);
        size_t vs = val.find_first_not_of(" \t\"");
        size_t ve = val.find_last_not_of(" \t\"");
        if (vs == std::string::npos) continue;
        return val.substr(vs, ve - vs + 1) == "FTDI";
    }
    return false;
}

// ---------------------------------------------------------------------------
// Driver path constants
// ---------------------------------------------------------------------------

// Windows — installed driver store
static const std::string kWindowsDriverDir      = "C:\\Windows\\INF\\";

// Windows — QTAC installer drops FTDI packages here
static const std::string kInstallerX64FTDIBus   = "C:\\ProgramData\\Qualcomm\\QTAC\\FTDI\\x64\\ftdibus.inf";
static const std::string kInstallerX64FTDIPort  = "C:\\ProgramData\\Qualcomm\\QTAC\\FTDI\\x64\\ftdiport.inf";
static const std::string kInstallerARMFTDIBus   = "C:\\ProgramData\\Qualcomm\\QTAC\\FTDI\\arm\\ARM64\\Release\\FTDIBUS.inf";
static const std::string kInstallerARMFTDIPort  = "C:\\ProgramData\\Qualcomm\\QTAC\\FTDI\\arm\\ARM64\\Release\\FTDIPORT.inf";

// Windows — sys files whose product version we verify
static const std::string kWindowsFTDIBusSys     = "C:\\Windows\\System32\\drivers\\ftdibus.sys";
static const std::string kWindowsFTDIPortSys    = "C:\\Windows\\System32\\drivers\\ftser2k.sys";

// Linux
static const std::string kLinuxDriverDir        = "/usr/local/lib/";
static const std::string kLinuxDriverPath       = "/usr/local/lib/libftd2xx.so.1.4.27";
static const std::string kLinuxDriverLinkPath   = "/usr/local/lib/libftd2xx.so";
static const std::string kLinuxInstallerDir     = "~/QTAC/FTDI/release/build";

// ---------------------------------------------------------------------------
// FTDICheckApplication
// ---------------------------------------------------------------------------

FTDICheckApplication::FTDICheckApplication()
{
    _expectedProductVersionMap = {
        {"ftbusui.dll",  "2.12.36.4"},
        {"ftcserco.dll", "2.12.36.4"},
        {"ftd2xx.dll",   "2.12.36.4"},
        {"ftlang.dll",   "2.12.36.4"},
        {"ftserui2.dll", "2.12.36.4"},
        {"ftdibus.sys",  "2.12.36.4"},
        {"ftser2k.sys",  "2.12.36.4"},
    };
}

int FTDICheckApplication::defaultActivity()
{
    bool result = verifyInstalledDriver();
    if (!result)
    {
        uninstallDriver();
        if (installDriver())
            verifyInstalledDriver();
    }
    return result ? 0 : 1;
}

void FTDICheckApplication::showVersion()
{
    std::cout << "Version " << TAC_LIB_VERSION << "\n";
}

bool FTDICheckApplication::verifyInstalledDriver()
{
    bool result{true};

#ifdef _WIN32
    std::cout << "\nVerifying installed driver...\nLooking for the FTDI driver: CDM v2.12.36.4\n";

    std::vector<std::string> expectedLibraries;
    for (const auto& kv : _expectedProductVersionMap)
    {
        // Exclude .sys files; those are verified via isWindowsFTDIDriverFilesPresent()
        if (kv.first != "ftdibus.sys" && kv.first != "ftser2k.sys")
            expectedLibraries.push_back(kv.first);
    }

    if (getArchitecture() == eX32)
        expectedLibraries.push_back("ftd2xx64.dll");

    for (const auto& lib : expectedLibraries)
    {
        if (!result) break;
        result = detectLibrary(lib);
    }

    for (const auto& path : _detectedFiles)
    {
        if (!result) break;
        result = checkLibraryVersion(path);
    }

    if (result)
        result = isWindowsFTDIDriverFilesPresent() && isDriverLatest();
#endif

#ifdef __linux__
    std::cout << "\nVerifying installed driver...\nLooking for the FTDI driver: v1.4.27\n";

    if (!fileExists(kLinuxDriverPath))
    {
        std::cout << "\nCould not find the expected FTDI driver. Installing...\n";
        result = false;
    }
    else if (!fileWritable(kLinuxDriverPath))
    {
        std::cout << "\nThe driver file was found but is not writable by current user. Fixing permissions...\n";
        result = false;
    }
    else if (!fileExists(kLinuxDriverLinkPath))
    {
        result = false;
    }
    else if (!fileWritable(kLinuxDriverLinkPath))
    {
        std::cout << "\nThe driver link file was found but is not writable by current user. Fixing permissions...\n";
        result = false;
    }
    else
    {
        std::string target = symlinkTarget(kLinuxDriverLinkPath);
        if (target != kLinuxDriverPath)
            result = false;
    }
#endif

    return result;
}

bool FTDICheckApplication::uninstallDriver()
{
    bool result{true};
    std::cout << "\nAttempting to uninstall driver...\n\n";

#ifdef _WIN32
    // Collect oem*.inf, ftdibus.inf, and ftdiport.inf from the Windows INF directory.
    std::vector<std::string> candidates;
    for (auto& f : listDir(kWindowsDriverDir, "oem*.inf"))   candidates.push_back(f);
    for (auto& f : listDir(kWindowsDriverDir, "ftdibus.inf")) candidates.push_back(f);
    for (auto& f : listDir(kWindowsDriverDir, "ftdiport.inf"))candidates.push_back(f);

    if (candidates.empty())
    {
        std::cout << "Could not find any driver files at path: " << kWindowsDriverDir << "\n";
        return false;
    }

    bool ftdiFound{false};
    for (const auto& name : candidates)
    {
        std::string fullPath = kWindowsDriverDir + name;
        if (!isFTDIInfContent(fullPath)) continue;
        ftdiFound = true;
        if (!deleteWindowsDriver(name))
        {
            std::cout << "Could not uninstall driver. Are you root? Is pnputil installed?\n";
            result = false;
            break;
        }
    }

    if (!ftdiFound)
        std::cout << "No FTDI drivers were found. Nothing to uninstall.\n";
#endif

#ifdef __linux__
    result = deleteLinuxDriver();
#endif

    return result;
}

bool FTDICheckApplication::installDriver()
{
    bool result{false};

#ifdef _WIN32
    std::vector<std::string> infPaths;
    ArchType arch = getArchitecture();
    if (arch == eX64)
    {
        infPaths.push_back(kInstallerX64FTDIBus);
        infPaths.push_back(kInstallerX64FTDIPort);
    }
    else if (arch == eARM)
    {
        infPaths.push_back(kInstallerARMFTDIBus);
        infPaths.push_back(kInstallerARMFTDIPort);
    }

    std::cout << "\nAttempting to install driver...\n\n";
    for (const auto& inf : infPaths)
        result = addWindowsDriver(inf);
#endif

#ifdef __linux__
    result = addLinuxDriver();
#endif

    return result;
}

ArchType FTDICheckApplication::getArchitecture()
{
#ifdef _WIN32
    USHORT processMachine = 0, nativeMachine = 0;
    IsWow64Process2(GetCurrentProcess(), &processMachine, &nativeMachine);
    switch (nativeMachine)
    {
        case IMAGE_FILE_MACHINE_I386:  return eX32;
        case IMAGE_FILE_MACHINE_AMD64: return eX64;
        case IMAGE_FILE_MACHINE_ARM64: return eARM;
        default: break;
    }
#endif
    return eUnknown;
}

bool FTDICheckApplication::detectLibrary(const std::string& libraryName)
{
    bool result{true};
#ifdef _WIN32
    HMODULE hLib = LoadLibraryA(libraryName.c_str());
    if (hLib)
    {
        char buf[MAX_PATH]{};
        GetModuleFileNameA(hLib, buf, sizeof(buf));
        _detectedFiles.push_back(buf);
        FreeLibrary(hLib);
    }
    else
    {
        std::cout << "The '" << libraryName << "' library was not found!\n";
        result = false;
    }
#endif
    return result;
}

bool FTDICheckApplication::checkLibraryVersion(const std::string& filePath)
{
    bool result{false};
#ifdef _WIN32
    DWORD dwHandle = 0;
    DWORD bufSize  = GetFileVersionInfoSizeA(filePath.c_str(), &dwHandle);
    if (bufSize == 0) return false;

    std::vector<BYTE> buf(bufSize);
    if (!GetFileVersionInfoA(filePath.c_str(), dwHandle, bufSize, buf.data()))
        return false;

    VS_FIXEDFILEINFO* info = nullptr;
    UINT infoLen = 0;
    if (!VerQueryValueA(buf.data(), "\\", reinterpret_cast<LPVOID*>(&info), &infoLen) || !info)
    {
        std::cout << "Could not find version information for '" << fileBasename(filePath) << "'\n";
        return false;
    }

    char version[32];
    std::snprintf(version, sizeof(version), "%hu.%hu.%hu.%hu",
        HIWORD(info->dwProductVersionMS), LOWORD(info->dwProductVersionMS),
        HIWORD(info->dwProductVersionLS), LOWORD(info->dwProductVersionLS));

    result = isExpectedProductVersion(fileBasename(filePath), version);
#endif
    return result;
}

bool FTDICheckApplication::isExpectedProductVersion(const std::string& fileName,
                                                     const std::string& version)
{
    auto it = _expectedProductVersionMap.find(fileName);
    if (it == _expectedProductVersionMap.end())
    {
        std::cout << "Could not find the expected product version for: " << fileName << "\n";
        return false;
    }
    if (it->second != version)
    {
        std::cout << "Found an unexpected product version '" << version
                  << "' for file: " << fileName << "\n";
        return false;
    }
    return true;
}

bool FTDICheckApplication::isFTDIDriverInfFile(const std::string& filePath)
{
    return isFTDIInfContent(filePath);
}

bool FTDICheckApplication::isWindowsFTDIDriverFilesPresent()
{
#ifdef _WIN32
    LPVOID driverList[1024];
    DWORD  actualSize = 0;
    if (!EnumDeviceDrivers(driverList, sizeof(driverList), &actualSize))
    {
        std::cout << "isWindowsFTDIDriverFilesPresent(): EnumDeviceDrivers failed."
                  << " Array needs " << actualSize / sizeof(LPVOID) << " entries.\n";
        return false;
    }

    bool busSysFound{false}, portSysFound{false};
    int  driverCount = static_cast<int>(actualSize / sizeof(driverList[0]));

    for (int i = 0; i < driverCount; ++i)
    {
        char name[MAX_PATH]{};
        if (!GetDeviceDriverBaseNameA(driverList[i], name, sizeof(name))) continue;
        if (std::string("ftdibus.sys") == name) busSysFound  = true;
        if (std::string("ftser2k.sys") == name) portSysFound = true;
    }

    if (busSysFound && portSysFound) return true;
    std::cout << "Could not find the required FTDI sys drivers.\n";
    return false;
#else
    return false;
#endif
}

bool FTDICheckApplication::isLinuxFTDIDriverFilesPresent()
{
#ifdef __linux__
    return listDir(kLinuxInstallerDir, "libftd2xx.*").size() == 3;
#else
    return false;
#endif
}

bool FTDICheckApplication::isDriverLatest()
{
    bool result{false};
    for (const auto& path : {kWindowsFTDIBusSys, kWindowsFTDIPortSys})
        result = checkLibraryVersion(path);

    if (result)
        std::cout << "\nThe expected driver version is already installed. Nothing to install.\n";
    return result;
}

bool FTDICheckApplication::deleteWindowsDriver(const std::string& infFile)
{
    std::cout << "Attempting to delete driver: " << infFile << "\n";
#ifdef _WIN32
    int exitCode = runProcess("C:\\Windows\\System32\\pnputil.exe",
                              {"/delete-driver", infFile.c_str(), "/uninstall", "/force"});
    if (exitCode != 0)
        std::cout << "Driver delete failed. Exit code: " << exitCode << "\n";
    std::cout << "\n";
    return exitCode == 0;
#else
    return false;
#endif
}

bool FTDICheckApplication::addWindowsDriver(const std::string& infFile)
{
    std::cout << "Attempting to install driver from: " << infFile << "\n";
#ifdef _WIN32
    int exitCode = runProcess("C:\\Windows\\System32\\pnputil.exe",
                              {"/add-driver", infFile.c_str(), "/install"});
    if (exitCode != 0)
        std::cout << "Driver installation failed. Exit code: " << exitCode << "\n";
    std::cout << "\n";
    return exitCode == 0;
#else
    return false;
#endif
}

bool FTDICheckApplication::deleteLinuxDriver()
{
#ifdef __linux__
    if (!isLinuxFTDIDriverFilesPresent())
    {
        std::cout << "Linux driver package not found at '" << kLinuxInstallerDir
                  << "'. Uninstall will not proceed for safety reasons.\n";
        return false;
    }

    bool result{true};
    for (const auto& name : listDir(kLinuxDriverDir, "libftd2xx.*"))
    {
        if (unlink((kLinuxDriverDir + name).c_str()) != 0)
        {
            std::cout << "Could not remove '" << name << "'. Try running as root.\n";
            result = false;
        }
    }
    return result;
#else
    return false;
#endif
}

bool FTDICheckApplication::addLinuxDriver()
{
#ifdef __linux__
    if (!isLinuxFTDIDriverFilesPresent())
    {
        std::cout << "Linux driver package not found at '" << kLinuxInstallerDir << "'\n";
        return false;
    }

    static const std::vector<std::string> kDriverFiles = {
        "libftd2xx.a", "libftd2xx.so.1.4.27", "libftd2xx.txt"
    };

    bool result{true};
    for (const auto& name : kDriverFiles)
    {
        std::string src = kLinuxInstallerDir + "/" + name;
        std::string dst = kLinuxDriverDir    + name;
        if (!copyFile(src, dst))
        {
            std::cout << "Failed to copy '" << name << "'\n";
            result = false;
            break;
        }
        // Match original: set other-execute only (mirrors QFileDevice::ExeOther)
        if (chmod(dst.c_str(), S_IXOTH) != 0)
        {
            std::cout << "Failed to set execute permission for '" << name << "'\n";
            result = false;
            break;
        }
    }
    return result;
#else
    return false;
#endif
}
