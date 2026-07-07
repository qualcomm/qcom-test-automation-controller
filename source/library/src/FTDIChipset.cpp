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

// Authors: msimpson, biswroy

#include <qtac/FTDIChipset.h>
#include <qtac/StringUtilities.h>

// FTDI D2XX
#include "ftd2xx.h"

#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <cstdlib>
#ifdef _WIN32
#  include <direct.h>  // _mkdir
#else
#  include <sys/stat.h> // mkdir
#endif

// -----------------------------------------------------------------------
// Minimal key-value settings store (replaces QSettings)
// Stores VTP port assignments in ~/.qtac/vtps.ini (Linux) or
// %APPDATA%\qtac\vtps.ini (Windows).
// -----------------------------------------------------------------------

namespace {

std::string settingsFilePath()
{
#ifdef _WIN32
	const char* appdata = std::getenv("APPDATA");
	std::string dir = appdata ? std::string(appdata) + "\\qtac" : ".";
	return dir + "\\vtps.ini";
#else
	const char* home = std::getenv("HOME");
	std::string dir = home ? std::string(home) + "/.qtac" : ".";
	return dir + "/vtps.ini";
#endif
}

std::map<std::string, std::string> loadSettings()
{
	std::map<std::string, std::string> m;
	std::ifstream f(settingsFilePath());
	if (!f.is_open()) return m;
	std::string line;
	while (std::getline(f, line))
	{
		auto eq = line.find('=');
		if (eq == std::string::npos) continue;
		m[line.substr(0, eq)] = line.substr(eq + 1);
	}
	return m;
}

void saveSettings(const std::map<std::string, std::string>& m)
{
	// Ensure directory exists (best-effort)
#ifdef _WIN32
	const char* appdata = std::getenv("APPDATA");
	if (appdata)
	{
		std::string dir = std::string(appdata) + "\\qtac";
		_mkdir(dir.c_str());
	}
#else
	const char* home = std::getenv("HOME");
	if (home)
	{
		std::string dir = std::string(home) + "/.qtac";
		::mkdir(dir.c_str(), 0755);
	}
#endif
	std::ofstream f(settingsFilePath());
	for (const auto& kv : m)
		f << kv.first << "=" << kv.second << "\n";
}

} // anonymous namespace

// -----------------------------------------------------------------------

namespace qtac {

FTDIChipsetList _FTDIChipset::_ftdiChipsetList;

_FTDIChipset::_FTDIChipset() = default;
_FTDIChipset::~_FTDIChipset() = default;

qtac::String _FTDIChipset::ftidStatusToString(unsigned long status)
{
	qtac::String code = qtac::String::number(static_cast<int>(status));
	switch (status)
	{
	case FT_OK:                          return "Status Okay";
	case FT_INVALID_HANDLE:              return "Invalid handle: " + code;
	case FT_DEVICE_NOT_FOUND:            return "Device not found: " + code;
	case FT_DEVICE_NOT_OPENED:           return "Device not opened: " + code;
	case FT_IO_ERROR:                    return "IO error: " + code;
	case FT_INSUFFICIENT_RESOURCES:      return "Insufficient resources: " + code;
	case FT_INVALID_PARAMETER:           return "Invalid parameter: " + code;
	case FT_INVALID_BAUD_RATE:           return "Invalid baud rate: " + code;
	case FT_DEVICE_NOT_OPENED_FOR_ERASE: return "Device not opened for erase: " + code;
	case FT_DEVICE_NOT_OPENED_FOR_WRITE: return "Device not opened for write: " + code;
	case FT_FAILED_TO_WRITE_DEVICE:      return "Failed to write device: " + code;
	case FT_EEPROM_READ_FAILED:          return "EEPROM read failed: " + code;
	case FT_EEPROM_WRITE_FAILED:         return "EEPROM write failed: " + code;
	case FT_EEPROM_ERASE_FAILED:         return "EEPROM erase failed: " + code;
	case FT_EEPROM_NOT_PRESENT:          return "EEPROM not present: " + code;
	case FT_EEPROM_NOT_PROGRAMMED:       return "EEPROM not programmed: " + code;
	case FT_INVALID_ARGS:                return "Invalid arguments: " + code;
	case FT_NOT_SUPPORTED:               return "Not supported: " + code;
	case FT_OTHER_ERROR:                 return "Other error: " + code;
	case FT_DEVICE_LIST_NOT_READY:       return "Device list not ready: " + code;
	default:                             return "Status " + code + " not found";
	}
}

bool _FTDIChipset::hasDevice(HashType portHash)
{
	for (const auto& dev : _ftdiChipsetList)
		if (dev->hash() == portHash)
			return true;
	return false;
}

uint32_t _FTDIChipset::getDeviceCount()
{
	for (auto& dev : _ftdiChipsetList)
		dev->reset();

#ifdef _WIN32
	windowsTraversal();
#endif
#ifdef __linux__
	linuxTraversal();
#endif

	return static_cast<uint32_t>(_ftdiChipsetList.size());
}

FTDIChipset _FTDIChipset::getDevice(uint32_t deviceIndex)
{
	if (deviceIndex < static_cast<uint32_t>(_ftdiChipsetList.size()))
		return _ftdiChipsetList.at(static_cast<int>(deviceIndex));
	return FTDIChipset();
}

FTDIChipset _FTDIChipset::getDevice(const qtac::ByteArray& portName)
{
	for (auto& dev : _ftdiChipsetList)
	{
		if (dev->portName().contains(portName)) return dev;
		if (dev->serialNumber().contains(portName)) return dev;
	}
	return FTDIChipset();
}

qtac::ByteArray _FTDIChipset::normalizeSerialNumber(const qtac::ByteArray& segmentSerialNumber)
{
	qtac::ByteArray sn{segmentSerialNumber};
	if (!sn.isEmpty())
		sn.remove(sn.length() - 1, 1);
	return sn;
}

bool _FTDIChipset::open(FTDIPinSets pinsets)
{
	bool result{false};
	uint8_t mask = 0xff;
	uint8_t mode = FT_BITMODE_ASYNC_BITBANG;
	FT_STATUS ftStatus;

	if (testFlag(pinsets, eA) && !_aSerialNumber.isEmpty())
	{
		ftStatus = FT_OpenEx(_aSerialNumber.data(), FT_OPEN_BY_SERIAL_NUMBER, &_aHandle);
		if (ftStatus == FT_OK)
		{
			FT_SetBitMode(_aHandle, mask, mode);
			result = true;
		}
		else
			_aHandle = nullptr;
	}

	if (testFlag(pinsets, eB) && !_bSerialNumber.isEmpty())
	{
		ftStatus = FT_OpenEx(_bSerialNumber.data(), FT_OPEN_BY_SERIAL_NUMBER, &_bHandle);
		if (ftStatus == FT_OK)
		{
			FT_SetBitMode(_bHandle, mask, mode);
			result = true;
		}
		else
			_bHandle = nullptr;
	}

	if (testFlag(pinsets, eC) && !_cSerialNumber.isEmpty())
	{
		ftStatus = FT_OpenEx(_cSerialNumber.data(), FT_OPEN_BY_SERIAL_NUMBER, &_cHandle);
		if (ftStatus == FT_OK)
		{
			FT_SetBitMode(_cHandle, mask, mode);
			result = true;
		}
		else
			_cHandle = nullptr;
	}

	if (testFlag(pinsets, eD) && !_dSerialNumber.isEmpty())
	{
		ftStatus = FT_OpenEx(_dSerialNumber.data(), FT_OPEN_BY_SERIAL_NUMBER, &_dHandle);
		if (ftStatus == FT_OK)
		{
			FT_SetBitMode(_dHandle, mask, mode);
			result = true;
		}
		else
			_dHandle = nullptr;
	}

	return result;
}

bool _FTDIChipset::isOpen()
{
	return _aHandle != nullptr || _bHandle != nullptr
	    || _cHandle != nullptr || _dHandle != nullptr;
}

void _FTDIChipset::close()
{
	auto closeHandle = [&](void*& handle) {
		if (handle != nullptr)
		{
			FT_SetBitMode(handle, 0, 0);
			FT_STATUS s = FT_Close(handle);
			if (s == FT_OK) handle = nullptr;
		}
	};
	closeHandle(_aHandle);
	closeHandle(_bHandle);
	closeHandle(_cHandle);
	closeHandle(_dHandle);
}

PlatformID _FTDIChipset::platformID()
{
	return _platformID;
}

void _FTDIChipset::setPlatformID(PlatformID platformID)
{
	_platformID = platformID;
}

qtac::String _FTDIChipset::versionString()
{
	return PlatformContainer::toString(_platformID) + " " + firmwareString();
}

qtac::String _FTDIChipset::firmwareString()
{
	return qtac::String("qtac-core");
}

HashType _FTDIChipset::hash()
{
	return _hash;
}

qtac::ByteArray _FTDIChipset::serialNumber()
{
	return _serialNumber;
}

bool _FTDIChipset::newDevice()
{
	bool result{_new};
	_new = false;
	return result;
}

qtac::ByteArray _FTDIChipset::aSerialNumber() { return _aSerialNumber; }
qtac::ByteArray _FTDIChipset::bSerialNumber() { return _bSerialNumber; }
qtac::ByteArray _FTDIChipset::cSerialNumber() { return _cSerialNumber; }
qtac::ByteArray _FTDIChipset::dSerialNumber() { return _dSerialNumber; }

void _FTDIChipset::setASerialNumber(const qtac::ByteArray& sn)
{
	if (_aSerialNumber.isEmpty()) { setupHash(sn); _aSerialNumber = sn; }
}
void _FTDIChipset::setBSerialNumber(const qtac::ByteArray& sn)
{
	if (_bSerialNumber.isEmpty()) { setupHash(sn); _bSerialNumber = sn; }
}
void _FTDIChipset::setCSerialNumber(const qtac::ByteArray& sn)
{
	if (_cSerialNumber.isEmpty()) { setupHash(sn); _cSerialNumber = sn; }
}
void _FTDIChipset::setDSerialNumber(const qtac::ByteArray& sn)
{
	if (_dSerialNumber.isEmpty()) { setupHash(sn); _dSerialNumber = sn; }
}

bool _FTDIChipset::write(uint8_t pin, bool state)
{
	void* handle{nullptr};
	CharBit* charBit{nullptr};

	if (pin <= 7 && _aHandle != nullptr)
	{
		handle = _aHandle; _aPins.set(pin, state); charBit = &_aPins;
	}
	else if (pin <= 15 && _bHandle != nullptr)
	{
		handle = _bHandle; _bPins.set(pin, state); charBit = &_bPins;
	}
	else if (pin <= 23 && _cHandle != nullptr)
	{
		handle = _cHandle; _cPins.set(pin, state); charBit = &_cPins;
	}
	else if (pin <= 31 && _dHandle != nullptr)
	{
		handle = _dHandle; _dPins.set(pin, state); charBit = &_dPins;
	}

	if (handle != nullptr && charBit != nullptr)
	{
		DWORD byteWritten;
		FT_STATUS status = FT_Write(handle, charBit->value(), 1, &byteWritten);
		if (status == FT_OK) return true;
		_lastError = ("FTDI Write Failed: " + _FTDIChipset::ftidStatusToString(status) + "\n").toLatin1();
	}
	return false;
}

HashType _FTDIChipset::hash(const qtac::ByteArray& serialNumber)
{
	return arrayHash(serialNumber);
}

void _FTDIChipset::setSerialNumber(const qtac::ByteArray& serialNumber)
{
	if (_serialNumber.isEmpty())
	{
		_serialNumber = serialNumber;
		setupPortName();
	}
}

unsigned long long _FTDIChipset::setCustomVIDPID()
{
	unsigned long long ftStatus = !FT_OK;
#ifdef __linux__
	ftStatus = FT_SetVIDPID(1027, 6552);
#endif
	return ftStatus;
}

void _FTDIChipset::setupHash(const qtac::ByteArray& segmentSerialNumber)
{
	if (_hash == 0)
	{
		qtac::ByteArray sn = _FTDIChipset::normalizeSerialNumber(segmentSerialNumber);
		_hash = hash(sn);
		_serialNumber = sn;
	}
}

void _FTDIChipset::setupPortName()
{
	if (_portName.isEmpty() && !_serialNumber.isEmpty())
	{
		auto settings = loadSettings();
		std::string key = _serialNumber.toStdString();

		std::string vtpName;
		if (settings.count(key))
		{
			vtpName = settings[key];
		}
		else
		{
			unsigned int nextPort = 1;
			if (settings.count("NextPortNumber"))
				nextPort = static_cast<unsigned int>(std::stoul(settings["NextPortNumber"]));

			std::ostringstream oss;
			oss << "VTP" << nextPort;
			vtpName = oss.str();

			settings[key] = vtpName;
			settings["NextPortNumber"] = std::to_string(nextPort + 1);
			saveSettings(settings);
		}
		_portName = qtac::ByteArray(vtpName.c_str());
	}
}

PlatformID _FTDIChipset::nameToPlatform(const qtac::ByteArray& deviceName)
{
	return PlatformContainer::fromUSBDescriptor(deviceName);
}

// -----------------------------------------------------------------------
// Platform traversal
// -----------------------------------------------------------------------

#ifdef __linux__
void _FTDIChipset::linuxTraversal()
{
	FT_STATUS ftStatus;
	DWORD deviceCount;

	ftStatus = setCustomVIDPID();
	if (ftStatus != FT_OK)
		return;

	ftStatus = FT_CreateDeviceInfoList(&deviceCount);
	if (ftStatus != FT_OK || deviceCount == 0)
		return;

	FT_DEVICE_LIST_INFO_NODE* devInfoList = new FT_DEVICE_LIST_INFO_NODE[deviceCount];

	ftStatus = FT_GetDeviceInfoList(devInfoList, &deviceCount);
	if (ftStatus == FT_OK)
	{
		FT_HANDLE ftHandleTemp;
		DWORD Flags, ID, Type, LocId;
		char SerialNumber[16];
		char Description[64];

		for (DWORD i = 0; i < deviceCount; ++i)
		{
			ftStatus = FT_GetDeviceInfoDetail(i, &Flags, &Type, &ID, &LocId,
			                                  SerialNumber, Description, &ftHandleTemp);

			qtac::String desc(Description);
			qtac::ByteArray usbDescriptor = desc.left(desc.length() - 2).toLatin1();

			if (PlatformContainer::fromUSBDescriptor(usbDescriptor) != MICRO_EPM_BOARD_ID_UNKNOWN
			    || usbDescriptor.startsWith("ALPACA-LITE "))
			{
				PlatformID platformID = PlatformContainer::fromUSBDescriptor(usbDescriptor);
				qtac::ByteArray deviceSerialNumber(SerialNumber);
				qtac::ByteArray serialNumber = normalizeSerialNumber(deviceSerialNumber);

				if (serialNumber.isEmpty()) continue;

				uint32_t portHash = hash(serialNumber);
				char segment = (!desc.isEmpty()) ? Description[desc.length() - 1] : 0;

				FTDIChipset ftdiChipset;
				if (hasDevice(portHash))
					ftdiChipset = getDevice(serialNumber);
				else
				{
					ftdiChipset = FTDIChipset(new _FTDIChipset);
					ftdiChipset->setPlatformID(platformID);
					_ftdiChipsetList.append(ftdiChipset);
				}

				ftdiChipset->setSerialNumber(serialNumber);
				if (ftdiChipset->_usbDescriptor.isEmpty())
					ftdiChipset->_usbDescriptor = usbDescriptor;

				switch (segment)
				{
				case 'A': case 'a': ftdiChipset->setASerialNumber(deviceSerialNumber); break;
				case 'B': case 'b': ftdiChipset->setBSerialNumber(deviceSerialNumber); break;
				case 'C': case 'c': ftdiChipset->setCSerialNumber(deviceSerialNumber); break;
				case 'D': case 'd': ftdiChipset->setDSerialNumber(deviceSerialNumber); break;
				}
				ftdiChipset->_active = true;
			}
		}
	}

	delete[] devInfoList;
}

void _FTDIChipset::windowsTraversal() {}
#endif // __linux__

#ifdef _WIN32
void _FTDIChipset::linuxTraversal() {}

void _FTDIChipset::windowsTraversal()
{
	FT_STATUS ftStatus;
	DWORD deviceCount;

	ftStatus = FT_CreateDeviceInfoList(&deviceCount);
	if (ftStatus != FT_OK || deviceCount == 0)
		return;

	FT_DEVICE_LIST_INFO_NODE* devInfoList = new FT_DEVICE_LIST_INFO_NODE[deviceCount];

	ftStatus = FT_GetDeviceInfoList(devInfoList, &deviceCount);
	if (ftStatus == FT_OK)
	{
		for (DWORD i = 0; i < deviceCount; ++i)
		{
			qtac::String desc(devInfoList[i].Description);
			qtac::ByteArray usbDescriptor = desc.left(desc.length() - 2).toLatin1();

			if (PlatformContainer::fromUSBDescriptor(usbDescriptor) != MICRO_EPM_BOARD_ID_UNKNOWN
			    || usbDescriptor.startsWith("ALPACA-LITE "))
			{
				PlatformID platformID = PlatformContainer::fromUSBDescriptor(usbDescriptor);
				qtac::ByteArray deviceSerialNumber(devInfoList[i].SerialNumber);
				qtac::ByteArray serialNumber = normalizeSerialNumber(deviceSerialNumber);

				if (serialNumber.isEmpty()) continue;

				uint32_t portHash = hash(serialNumber);
				char segment = (!desc.isEmpty()) ? devInfoList[i].Description[desc.length() - 1] : 0;

				FTDIChipset ftdiChipset;
				if (hasDevice(portHash))
					ftdiChipset = getDevice(serialNumber);
				else
				{
					ftdiChipset = FTDIChipset(new _FTDIChipset);
					ftdiChipset->setPlatformID(platformID);
					_ftdiChipsetList.append(ftdiChipset);
				}

				ftdiChipset->setSerialNumber(serialNumber);
				if (ftdiChipset->_usbDescriptor.isEmpty())
					ftdiChipset->_usbDescriptor = usbDescriptor;

				switch (segment)
				{
				case 'A': case 'a': ftdiChipset->setASerialNumber(deviceSerialNumber); break;
				case 'B': case 'b': ftdiChipset->setBSerialNumber(deviceSerialNumber); break;
				case 'C': case 'c': ftdiChipset->setCSerialNumber(deviceSerialNumber); break;
				case 'D': case 'd': ftdiChipset->setDSerialNumber(deviceSerialNumber); break;
				}
				ftdiChipset->_active = true;

				if (devInfoList[i].Flags == FT_FLAGS_OPENED)
					ftdiChipset->_lastError = "QTAC failed to enumerate all connected FTDI devices as they are in use";
			}
		}
	}

	delete[] devInfoList;
}
#endif // _WIN32

} // namespace qtac
