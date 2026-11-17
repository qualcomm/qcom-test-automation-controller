#ifndef FTDICHIPSET_H
#define FTDICHIPSET_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QCommon
#include "CharBit.h"
#include "DebugBoardType.h"
#include "FTDIPinSet.h"
#include "PlatformID.h"
#include "StringUtilities.h"

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

class _FTDIChipset;

typedef std::shared_ptr<_FTDIChipset> FTDIChipset;
typedef std::vector<FTDIChipset> FTDIChipsetList;
typedef std::map<HashType, FTDIChipset> FTDIChipsetMap;

#include "QCommonConsoleGlobal.h"

class QCOMMONCONSOLE_EXPORT _FTDIChipset
{
public:

	_FTDIChipset(const _FTDIChipset& copyMe) = delete;

	_FTDIChipset();

	static std::string ftidStatusToString(unsigned long status);

	static uint32_t getDeviceCount();
	static FTDIChipset getDevice(uint32_t deviceIndex);
	static FTDIChipset getDevice(const std::string& portName);
	static std::string normalizeSerialNumber(DebugBoardType boardType, const std::string& segmentSerialNumber);

	bool open(FTDIPinSets pinsets);
	bool isOpen();
	void close();

	DebugBoardType boardType();
	void setBoardType(DebugBoardType boardType);

	PlatformID platformID();
	void setPlatformID(PlatformID platformID);

	std::string versionString();
	std::string firmwareString();

	void reset()
	{
		_active = false;
		_ftdiChipsetList.clear();
	}

	bool active()
	{
		return _active;
	}

	HashType hash();
	std::string serialNumber();
	bool newDevice();

	std::string portName()
	{
		return _portName;
	}

	std::string usbDescriptor()
	{
		return _usbDescriptor;
	}

	std::string lastError()
	{
		return _lastError;
	}

	std::string aSerialNumber();
	void setASerialNumber(const std::string& aSerialNumber);

	std::string bSerialNumber();
	void setBSerialNumber(const std::string& bSerialNumber);

	std::string cSerialNumber();
	void setCSerialNumber(const std::string& cSerialNumber);

	std::string dSerialNumber();
	void setDSerialNumber(const std::string& dSerialNumber);

	bool write(uint8_t pin, bool state);

	void setInvertMask(uint8_t mask) { _invertMask = mask; }

private:
	static HashType hash(const std::string& serialNumber);

	void setSerialNumber(const std::string& serialNumber);
	static unsigned long long setCustomVIDPID();

	static bool hasDevice(HashType portHash);
	void setupHash(const std::string& segmentSerialNumber);
	void setupPortName();
	static PlatformID nameToPlatform(const std::string& deviceName);

	static void linuxTraversal();
	static void windowsTraversal();

	HashType			_hash{0};
	PlatformID			_platformID{ALPACA_LITE_ID};
	DebugBoardType		_boardType{eFTDI};
	bool				_new{true};
	bool				_active{false};
	CharBit				_aPins;
	CharBit				_bPins;
	CharBit				_cPins;
	CharBit				_dPins;

	std::string			_serialNumber;
	std::string			_portName;
	std::string			_usbDescriptor;

	std::string			_aSerialNumber;
	void*				_aHandle{nullptr};

	std::string			_bSerialNumber;
	void*				_bHandle{nullptr};

	std::string			_cSerialNumber;
	void*				_cHandle{nullptr};

	std::string			_dSerialNumber;
	void*				_dHandle{nullptr};

	std::string         _lastError;

	uint8_t				_invertMask{0};

	static FTDIChipsetList		_ftdiChipsetList;
};

#endif // FTDICHIPSET_H
