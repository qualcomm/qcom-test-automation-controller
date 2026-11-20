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

#include <nlohmann/json.hpp>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace fs = std::filesystem;
using json   = nlohmann::json;

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
static void computePinSets(const json& tcnf, uint32_t pinSets[4])
{
    for (int i = 0; i < 4; ++i)
        pinSets[i] = 0;

    if (!tcnf.contains("bus") || !tcnf["bus"].is_array())
        return;

    static const struct { char bus; uint32_t flag; } kBusFlags[] = {
        {'A', 1u}, {'B', 2u}, {'C', 4u}, {'D', 8u}
    };

    for (const auto& entry : tcnf["bus"])
    {
        if (!entry.is_object())
            continue;

        int         chipIndex = entry.value("chip_index",    0);
        int         busFunc   = entry.value("bus_function",  0);
        std::string bus       = entry.value("bus",           std::string{});

        if (chipIndex < 0 || chipIndex >= 4)
            continue;
        if (busFunc != 2)   // only D2XX buses contribute to the pinset
            continue;
        if (bus.empty())
            continue;

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

static json loadJsonFile(const fs::path& path)
{
    std::ifstream f(path);
    if (!f.is_open())
        throw std::runtime_error("Cannot open file: " + path.string());
    return json::parse(f);
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
    std::map<int, json> entries;  // keyed by platform_id, sorted ascending

    if (fs::exists(deviceListPath))
    {
        try
        {
            json existing = loadJsonFile(deviceListPath);
            if (existing.contains("catalog") && existing["catalog"].is_array())
            {
                for (const auto& entry : existing["catalog"])
                {
                    if (entry.contains("platform_id") && entry["platform_id"].is_number_integer())
                    {
                        int pid = entry["platform_id"].get<int>();
                        entries[pid] = entry;
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

        json tcnf;
        try
        {
            tcnf = loadJsonFile(tcnfPath);
        }
        catch (const std::exception& e)
        {
            std::cerr << "Warning: failed to parse " << filename
                      << ": " << e.what() << " — skipped.\n";
            continue;
        }

        if (!tcnf.contains("platform_id") || !tcnf["platform_id"].is_number_integer())
        {
            std::cerr << "Warning: no platform_id in " << filename << " — skipped.\n";
            continue;
        }

        const int         pid         = tcnf["platform_id"].get<int>();
        const std::string name        = tcnf.value("name",           std::string{});
        const std::string description = tcnf.value("description",    std::string{});
        const int         revision    = tcnf.value("fileVersion",    0);
        const std::string usbDesc     = tcnf.value("usb_descriptor", std::string{});

        json newEntry;
        newEntry["platform_id"]    = pid;
        newEntry["debugBoardType"] = boardType;
        newEntry["name"]           = name;
        newEntry["description"]    = description;
        newEntry["revision"]       = revision;
        newEntry["usb_descriptor"] = usbDesc;
        // configPath is relative to the devicelist.json directory; since both
        // files live in configurations/, just the filename is sufficient.
        // PlatformID.cpp resolves relative paths against devicelist.json's
        // parent directory, so this is unambiguous.
        newEntry["configPath"]     = filename;

        // FTDI and FT232H boards expose per-chip D2XX bus pinsets.
        if (boardType == kFTDI || boardType == kFT232H)
        {
            uint32_t pinSets[4];
            computePinSets(tcnf, pinSets);
            newEntry["chip1BusSet"] = pinSets[0];
            newEntry["chip2BusSet"] = pinSets[1];
            newEntry["chip3BusSet"] = pinSets[2];
            newEntry["chip4BusSet"] = pinSets[3];
        }

        // Preserve manually-added fields from the existing entry that are not
        // derived from the tcnf (e.g. "firmware_chip" on PSoC platform 66).
        auto existIt = entries.find(pid);
        if (existIt != entries.end())
        {
            const json& existing = existIt->second;
            if (existing.contains("firmware_chip"))
                newEntry["firmware_chip"] = existing["firmware_chip"];
        }

        entries[pid] = newEntry;
        ++processedCount;

        std::cout << "  " << filename
                  << " (platform_id=" << pid << ", boardType=" << boardType << ")\n";
    }

    // -----------------------------------------------------------------------
    // Write devicelist.json
    // -----------------------------------------------------------------------
    json catalog = json::array();
    for (const auto& kv : entries)
        catalog.push_back(kv.second);

    json output;
    output["catalog"] = catalog;

    std::ofstream out(deviceListPath);
    if (!out.is_open())
    {
        std::cerr << "Error: cannot write to " << deviceListPath.string() << "\n";
        return 1;
    }
    out << output.dump(4) << "\n";
    out.close();

    std::cout << "Wrote " << deviceListPath.string()
              << "  (" << catalog.size() << " entries, "
              << processedCount << " from tcnf files)\n";

    return 0;
}
