// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef PLATFORMID_H
#define PLATFORMID_H

#include "QCommonConsoleGlobal.h"

#include "DebugBoardType.h"
#include "FTDIPinSet.h"

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

typedef uint32_t PlatformID;

const PlatformID MICRO_EPM_BOARD_ID_UNKNOWN{0};
const PlatformID ALPACA_LITE_ID{13};
const PlatformID ALPACA_PIC32CX_ID{50};
const PlatformID ALPACA_BUGHOPPER_ID{80};
const PlatformID ALPACA_STM32_ID{95};


struct _PlatformEntry
{
	_PlatformEntry()
	{
		for (int i{0}; i < kMaxPinSetCount; i++)
			_pinSets[i] = NoOptions;
	}
	_PlatformEntry(PlatformID platformID, DebugBoardType boardType, const std::string& description, const std::string& path = std::string(), const std::string& usbDescriptor = std::string());
	~_PlatformEntry() = default;

	PlatformID					_platformID{0};
	DebugBoardType				_boardtype{eUnknownDebugBoard};
	std::string					_description;
	std::string					_usbDescriptor;
	std::string					_path;
	FTDIPinSets					_pinSets[kMaxPinSetCount];
};

typedef std::shared_ptr<_PlatformEntry> PlatformEntry;
typedef std::vector<PlatformEntry> PlatformIDList;
typedef std::map<PlatformID, PlatformEntry> PlatformIDs;
typedef PlatformIDs::const_iterator PlatformIDIterator;

class QCOMMONCONSOLE_EXPORT PlatformContainer
{
public:
	PlatformContainer() = delete;
	PlatformContainer(const PlatformContainer& copyMe) = delete;
	~PlatformContainer() = delete;

	static void initialize();
	static PlatformIDList getEntries();
	static void addEntry(PlatformEntry platformEntry);

	static std::string toString(PlatformID platformID);

	static PlatformID fromUSBDescriptor(const std::string& usbDescriptor);

	static DebugBoardType getDebugBoardType(PlatformID platformID);
	static PlatformIDList getDebugBoards();
	static PlatformIDList getDebugBoardsOfType(DebugBoardType debugBoardType);

private:
	static void initializeDynamic();

	static PlatformIDs			_platformIds;

};

#endif // PLATFORMID_H
