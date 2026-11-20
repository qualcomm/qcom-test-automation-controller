// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// Qt-free port of src/applications/updatedevicelist/UpdateDeviceList.
//
// Scans all *.tcnf files in a given directory and regenerates
// configurations/devicelist.json from them.  Existing entries that have no
// corresponding tcnf file (legacy hard-coded entries, manually added fields
// such as "firmware_chip") are preserved from the existing devicelist.json.
//
// Usage:
//   update-device-list [--dir <config_dir>]
//   update-device-list [dir=<config_dir>]        (compatible with legacy form)
//
// <config_dir> defaults to the current working directory.

#include <qtac/Json.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace fs = std::filesystem;

// ---------------------------------------------------------------------------
// DebugBoardType integer values — must match source/library/include/qtac/DebugBoardType.h
// ---------------------------------------------------------------------------
static constexpr int kUnknownDebugBoard = 0;
static constexpr int kPSOC              = 2;
static constexpr int kFTDI              = 3;
static constexpr int kPIC32CXAuto       = 4;
static constexpr int kFT232H            = 5;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static bool startsWith(const std::string& s, const std::string& prefix)
{
    return s.rfind(prefix, 0) == 0;
}

// Infer DebugBoardType integer from the tcnf filename prefix.
static int inferBoardType(const std::string& filename)
{
    if (startsWith(filename, "TAC_FTDI_"))         return kFTDI;
    if (startsWith(filename, "TAC_FT232H_"))       return kFT232H;
    if (startsWith(filename, "TAC_PSOC_"))         return kPSOC;
    if (startsWith(filename, "TAC_PIC32CXAuto_"))  return kPIC32CXAuto;
    return kUnknownDebugBoard;
}

// Compute D2XX bus pinsets for each chip index from the "bus" array.
// Returns four uint32_t values (chip 0 .. chip 3).
// Bus flags: A=1, B=2, C=4, D=8  (matches FTDIPinSet.h eA/eB/eC/eD).
// bus_function 2 == D2XX; 1 == VCP (not included in the pinset).
static void computePinSets(const qtac::JsonValue& tcnf, uint32_t pinSets[4])
{
    static const struct { char bus; uint32_t flag; } kBusFlags[] = {
        {'A', 1u}, {'B', 2u}, {'C', 4u}, {'D', 8u}
    };

    for (int i = 0; i < 4; ++i)
        pinSets[i] = 0;

    if (!tcnf.isObject()) return;
    qtac::JsonObject root = tcnf.asObject();
    if (!root.contains("bus")) return;

    qtac::JsonValue busVal = root.at("bus");
    if (!busVal.isArray()) return;

    qtac::JsonArray busArr = busVal.asArray();
    for (const auto& entryVal : busArr)
    {
        if (!entryVal.isObject()) continue;
        qtac::JsonObject entry = entryVal.asObject();

        int         chipIndex = qtac::json::getInt   (entry, "chip_index",   0);
        int         busFunc   = qtac::json::getInt   (entry, "bus_function", 0);
        std::string bus       = qtac::json::getString(entry, "bus",          "");

        if (chipIndex < 0 || chipIndex >= 4) continue;
        if (busFunc != 2)   continue;  // only D2XX buses contribute to the pinset
        if (bus.empty())    continue;

        const char busChar = bus[0];
        for (const auto& bf : kBusFlags)
        {
            if (bf.bus == busChar)
            {
                pinSets[chipIndex] |= bf.flag;
                break;
            }
        }
    }
}

static std::string readFile(const fs::path& path)
{
    std::ifstream f(path);
    if (!f.is_open())
        throw std::runtime_error("Cannot open file: " + path.string());
    return std::string(std::istreambuf_iterator<char>(f),
                       std::istreambuf_iterator<char>());
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    // -----------------------------------------------------------------------
    // Parse arguments
    // Supported forms:
    //   --dir <path>
    //   --dir=<path>
    //   dir=<path>          (legacy form used by the src/ UpdateDeviceList tool)
    // -----------------------------------------------------------------------
    std::string configDir;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if ((arg == "--dir" || arg == "-d") && i + 1 < argc)
        {
            configDir = argv[++i];
        }
        else if (startsWith(arg, "--dir="))
        {
            configDir = arg.substr(6);
        }
        else if (startsWith(arg, "dir="))
        {
            configDir = arg.substr(4);
        }
    }

    if (configDir.empty())
        configDir = ".";

    fs::path dir(configDir);
    if (!fs::exists(dir) || !fs::is_directory(dir))
    {
        std::cerr << "Error: directory not found: " << configDir << "\n";
        return 1;
    }

    fs::path deviceListPath = dir / "devicelist.json";

    // -----------------------------------------------------------------------
    // Load existing devicelist.json (to preserve legacy / manually-added fields)
    // -----------------------------------------------------------------------
    // keyed by platform_id, sorted ascending
    std::map<int, qtac::JsonValue> entries;

    if (fs::exists(deviceListPath))
    {
        try
        {
            std::string text = readFile(deviceListPath);
            qtac::JsonValue existing = qtac::JsonValue::parse(text);
            if (existing.isObject())
            {
                qtac::JsonObject root = existing.asObject();
                if (root.contains("catalog"))
                {
                    qtac::JsonValue catalogVal = root.at("catalog");
                    if (catalogVal.isArray())
                    {
                        for (const auto& entryVal : catalogVal.asArray())
                        {
                            if (!entryVal.isObject()) continue;
                            qtac::JsonObject obj = entryVal.asObject();
                            if (!obj.contains("platform_id")) continue;
                            int pid = qtac::json::getInt(obj, "platform_id", -1);
                            if (pid < 0) continue;
                            entries[pid] = entryVal;
                        }
                    }
                }
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Warning: failed to parse existing devicelist.json: "
                      << e.what() << " — starting from scratch.\n";
        }
    }

    // -----------------------------------------------------------------------
    // Scan *.tcnf files
    // -----------------------------------------------------------------------
    std::vector<fs::path> tcnfFiles;
    for (const auto& dirEntry : fs::directory_iterator(dir))
    {
        if (dirEntry.is_regular_file() && dirEntry.path().extension() == ".tcnf")
            tcnfFiles.push_back(dirEntry.path());
    }
    std::sort(tcnfFiles.begin(), tcnfFiles.end());

    int processedCount = 0;

    for (const auto& tcnfPath : tcnfFiles)
    {
        const std::string filename = tcnfPath.filename().string();
        const int boardType = inferBoardType(filename);

        if (boardType == kUnknownDebugBoard)
        {
            std::cerr << "Warning: cannot infer board type for: " << filename
                      << " — skipped.\n";
            continue;
        }

        qtac::JsonValue tcnf;
        try
        {
            tcnf = qtac::JsonValue::parse(readFile(tcnfPath));
        }
        catch (const std::exception& e)
        {
            std::cerr << "Warning: failed to parse " << filename
                      << ": " << e.what() << " — skipped.\n";
            continue;
        }

        if (!tcnf.isObject())
        {
            std::cerr << "Warning: not a JSON object in " << filename << " — skipped.\n";
            continue;
        }

        qtac::JsonObject tcnfObj = tcnf.asObject();

        if (!tcnfObj.contains("platform_id"))
        {
            std::cerr << "Warning: no platform_id in " << filename << " — skipped.\n";
            continue;
        }

        const int         pid         = qtac::json::getInt   (tcnfObj, "platform_id",   -1);
        const std::string name        = qtac::json::getString(tcnfObj, "name",           "");
        const std::string description = qtac::json::getString(tcnfObj, "description",    "");
        const int         revision    = qtac::json::getInt   (tcnfObj, "fileVersion",    0);
        const std::string usbDesc     = qtac::json::getString(tcnfObj, "usb_descriptor", "");

        if (pid < 0) continue;

        qtac::JsonValue newEntry = qtac::JsonValue::makeObject();
        newEntry.setKey("platform_id",    pid);
        newEntry.setKey("debugBoardType", boardType);
        newEntry.setKey("name",           name);
        newEntry.setKey("description",    description);
        newEntry.setKey("revision",       revision);
        newEntry.setKey("usb_descriptor", usbDesc);
        // configPath is relative to the devicelist.json directory; since both
        // files live in configurations/, just the filename is sufficient.
        // PlatformID.cpp resolves relative paths against devicelist.json's
        // parent directory, so this is unambiguous.
        newEntry.setKey("configPath",     filename);

        // FTDI and FT232H boards expose per-chip D2XX bus pinsets.
        if (boardType == kFTDI || boardType == kFT232H)
        {
            uint32_t pinSets[4];
            computePinSets(tcnf, pinSets);
            newEntry.setKey("chip1BusSet", static_cast<unsigned int>(pinSets[0]));
            newEntry.setKey("chip2BusSet", static_cast<unsigned int>(pinSets[1]));
            newEntry.setKey("chip3BusSet", static_cast<unsigned int>(pinSets[2]));
            newEntry.setKey("chip4BusSet", static_cast<unsigned int>(pinSets[3]));
        }

        // Preserve manually-added fields from the existing entry that are not
        // derived from the tcnf (e.g. "firmware_chip" on PSoC platform 66).
        auto existIt = entries.find(pid);
        if (existIt != entries.end() && existIt->second.isObject())
        {
            qtac::JsonObject existing = existIt->second.asObject();
            if (existing.contains("firmware_chip"))
                newEntry.setKey("firmware_chip",
                                qtac::json::getInt(existing, "firmware_chip", 0));
        }

        entries[pid] = newEntry;
        ++processedCount;

        std::cout << "  " << filename
                  << " (platform_id=" << pid << ", boardType=" << boardType << ")\n";
    }

    // -----------------------------------------------------------------------
    // Write devicelist.json
    // -----------------------------------------------------------------------
    qtac::JsonValue catalog = qtac::JsonValue::makeArray();
    for (const auto& kv : entries)
        catalog.append(kv.second);

    qtac::JsonValue output = qtac::JsonValue::makeObject();
    output.setKey("catalog", catalog);

    std::ofstream out(deviceListPath);
    if (!out.is_open())
    {
        std::cerr << "Error: cannot write to " << deviceListPath.string() << "\n";
        return 1;
    }
    out << output.serializePretty(4);
    out.close();

    std::cout << "Wrote " << deviceListPath.string()
              << "  (" << entries.size() << " entries, "
              << processedCount << " from tcnf files)\n";

    return 0;
}
