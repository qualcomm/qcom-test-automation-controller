// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "FTDIChipset.h"

// QCommon
#include "AlpacaSettings.h"
#include "AppCore.h"
#include "PlatformConfiguration.h"
#include "Range.h"
#include "version.h"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <string>

// FTDI
#include "ftd2xx.h"

FTDIChipsetList _FTDIChipset::_ftdiChipsetList;

_FTDIChipset::_FTDIChipset()
{

}

std::string _FTDIChipset::ftidStatusToString
(
	unsigned long status
)
{
	std::string numberCode{std::to_string(status)};
	std::string result{"Status " + numberCode + " not found"};

	switch (status)
	{
	case FT_OK: result = "Status Okay"; break;
	case FT_INVALID_HANDLE: result = "Invalid handle: " + numberCode; break;
	case FT_DEVICE_NOT_FOUND: result = "Device not found: " + numberCode; break;
	case FT_DEVICE_NOT_OPENED: result = "Device not opened: " + numberCode; break;
	case FT_IO_ERROR: result = "IO error: " + numberCode; break;
	case FT_INSUFFICIENT_RESOURCES: result = "Insufficient resources: " + numberCode; break;
	case FT_INVALID_PARAMETER: result = "Invalid parameter: " + numberCode; break;
	case FT_INVALID_BAUD_RATE: result = "Invalid baud rate: " + numberCode; break;
	case FT_DEVICE_NOT_OPENED_FOR_ERASE: result = "Device not opened for erase: " + numberCode; break;
	case FT_DEVICE_NOT_OPENED_FOR_WRITE: result = "Device not opened for write: " + numberCode; break;
	case FT_FAILED_TO_WRITE_DEVICE: result = "Invalid Handle: " + numberCode; break;
	case FT_EEPROM_READ_FAILED: result = "EEPROM read failed: " + numberCode; break;
	case FT_EEPROM_WRITE_FAILED: result = "EEPROM write failed: " + numberCode; break;
	case FT_EEPROM_ERASE_FAILED: result = "EEPROM erase failed: " + numberCode; break;
	case FT_EEPROM_NOT_PRESENT: result = "EEPROM not present: " + numberCode; break;
	case FT_EEPROM_NOT_PROGRAMMED: result = "EEPROM not programmed: " + numberCode; break;
	case FT_INVALID_ARGS: result = "Invalid Arguments: " + numberCode; break;
	case FT_NOT_SUPPORTED: result = "Not supported: " + numberCode; break;
	case FT_OTHER_ERROR: result = "Other error: " + numberCode; break;
	case FT_DEVICE_LIST_NOT_READY: result = "Device list not ready: " + numberCode; break;
	default: break;
	}

	return result;
}

bool _FTDIChipset::hasDevice
(
	HashType portHash
)
{
	bool result{false};

	for (const auto& device: std::as_const(_FTDIChipset::_ftdiChipsetList))
	{
		if (device->hash() == portHash)
		{
			result = true;
			break;
		}
	}

	return result;
}

uint32_t _FTDIChipset::getDeviceCount()
{
	for (auto& device: _ftdiChipsetList)
		device->reset();

#ifdef Q_OS_WIN
	windowsTraversal();
#endif

#ifdef Q_OS_LINUX
	linuxTraversal();
#endif

	return static_cast<uint32_t>(_ftdiChipsetList.size());
}

FTDIChipset _FTDIChipset::getDevice
(
	uint32_t deviceIndex
)
{
	FTDIChipset result;

	if (deviceIndex < static_cast<uint32_t>(_ftdiChipsetList.size()))
		result = _ftdiChipsetList.at(deviceIndex);

	return result;
}

FTDIChipset _FTDIChipset::getDevice
(
	const std::string& portName
)
{
	for (auto& ftdiChipset: _ftdiChipsetList)
	{
		std::string candidate = ftdiChipset->portName();
		if (candidate.find(portName) != std::string::npos)
			return ftdiChipset;

		candidate = ftdiChipset->serialNumber();
		if (candidate.find(portName) != std::string::npos)
			return ftdiChipset;
	}

	return FTDIChipset();
}

std::string _FTDIChipset::normalizeSerialNumber(DebugBoardType boardType, const std::string& segmentSerialNumber)
{
	std::string serialNumber{segmentSerialNumber};

	if (boardType == eFTDI)
	{
		if (!serialNumber.empty())
			serialNumber.pop_back();
	}

	return serialNumber;
}

bool _FTDIChipset::open(FTDIPinSets pinsets)
{
	bool result{false};
	uint8_t mask;
	uint8_t mode;

	if (_boardType == eFT232H)
	{
		mask = 0xFF;
		mode = FT_BITMODE_CBUS_BITBANG;
	}
	else
	{
		mask = 0xFF;
		mode = FT_BITMODE_ASYNC_BITBANG;
	}

	FT_STATUS ftStatus;

	if (pinsets.testFlag(eA))
	{
		ftStatus = FT_OpenEx(const_cast<char*>(_aSerialNumber.c_str()), FT_OPEN_BY_SERIAL_NUMBER, &_aHandle);
		if (ftStatus == FT_OK)
		{
			FT_SetBitMode(_aHandle, mask, mode);
			result = true;
		}
		else
		{
			result = false;
			AppCore::writeToApplicationLog("FTDI Port A failure: " + _FTDIChipset::ftidStatusToString(ftStatus) + "\n");
			_aHandle = nullptr;
		}
	}

	if (pinsets.testFlag(eB))
	{
		ftStatus = FT_OpenEx(const_cast<char*>(_bSerialNumber.c_str()), FT_OPEN_BY_SERIAL_NUMBER, &_bHandle);
		if (ftStatus == FT_OK)
		{
			FT_SetBitMode(_bHandle, mask, mode);
			result = true;
		}
		else
		{
			result = false;
			AppCore::writeToApplicationLog("FTDI Port B failure: " + _FTDIChipset::ftidStatusToString(ftStatus) + "\n");
			_bHandle = nullptr;
		}
	}

	if (pinsets.testFlag(eC))
	{
		ftStatus = FT_OpenEx(const_cast<char*>(_cSerialNumber.c_str()), FT_OPEN_BY_SERIAL_NUMBER, &_cHandle);
		if (ftStatus == FT_OK)
		{
			FT_SetBitMode(_cHandle, mask, mode);
			result = true;
		}
		else
		{
			result = false;
			AppCore::writeToApplicationLog("FTDI Port C failure: " + _FTDIChipset::ftidStatusToString(ftStatus) + "\n");
			_cHandle = nullptr;
		}
	}

	if (pinsets.testFlag(eD))
	{
		ftStatus = FT_OpenEx(const_cast<char*>(_dSerialNumber.c_str()), FT_OPEN_BY_SERIAL_NUMBER, &_dHandle);
		if (ftStatus == FT_OK)
		{
			FT_SetBitMode(_dHandle, mask, mode);
			result = true;
		}
		else
		{
			result = false;
			AppCore::writeToApplicationLog("FTDI Port D failure: " + _FTDIChipset::ftidStatusToString(ftStatus) + "\n");
			_dHandle = nullptr;
		}
	}

	return result;
}

bool _FTDIChipset::isOpen()
{
	bool result{false};

	result = _aHandle != nullptr;

	if (result == false)
		result = _bHandle != nullptr;

	if (result == false)
		result = _cHandle != nullptr;

	if (result == false)
		result = _dHandle != nullptr;

	return result;
}

void _FTDIChipset::close()
{
	FT_STATUS ftStatus;

	if (_aHandle != nullptr)
	{
		FT_SetBitMode(_aHandle, 0, 0);

		ftStatus = FT_Close(_aHandle);
		if (ftStatus == FT_OK)
			_aHandle = nullptr;
		else
			AppCore::writeToApplicationLog("FTDI Port A close failure: " + _FTDIChipset::ftidStatusToString(ftStatus) + "\n");
	}

	if (_bHandle != nullptr)
	{
		FT_SetBitMode(_bHandle, 0, 0);
		ftStatus = FT_Close(_bHandle);
		if (ftStatus == FT_OK)
			_bHandle = nullptr;
		else
			AppCore::writeToApplicationLog("FTDI Port B close failure: " + _FTDIChipset::ftidStatusToString(ftStatus) + "\n");
	}

	if (_cHandle != nullptr)
	{
		FT_SetBitMode(_cHandle, 0, 0);
		ftStatus = FT_Close(_cHandle);
		if (ftStatus == FT_OK)
			_cHandle = nullptr;
		else
			AppCore::writeToApplicationLog("FTDI Port C close failure: " + _FTDIChipset::ftidStatusToString(ftStatus) + "\n");
	}

	if (_dHandle != nullptr)
	{
		FT_SetBitMode(_dHandle, 0, 0);
		ftStatus = FT_Close(_dHandle);
		if (ftStatus == FT_OK)
			_dHandle = nullptr;
		else
			AppCore::writeToApplicationLog("FTDI Port D close failure: " + _FTDIChipset::ftidStatusToString(ftStatus) + "\n");
	}
}

DebugBoardType _FTDIChipset::boardType()
{
	return _boardType;
}

void _FTDIChipset::setBoardType(DebugBoardType boardType)
{
	_boardType = boardType;
}

PlatformID _FTDIChipset::platformID()
{
	return _platformID;
}

void _FTDIChipset::setPlatformID(PlatformID platformID)
{
	_platformID = platformID;
}

std::string _FTDIChipset::versionString()
{
	std::string result = PlatformContainer::toString(_platformID) + " " + firmwareString();
	return result;
}

std::string _FTDIChipset::firmwareString()
{
	return QCOMMONCONSOLE_VERSION;
}

HashType _FTDIChipset::hash()
{
	return _hash;
}

std::string _FTDIChipset::serialNumber()
{
	return _serialNumber;
}

bool _FTDIChipset::newDevice()
{
	bool result{_new};

	_new = false;

	return result;
}

std::string _FTDIChipset::aSerialNumber()
{
	return _aSerialNumber;
}

void _FTDIChipset::setASerialNumber(const std::string& aSerialNumber)
{
	if (_aSerialNumber.empty())
	{
		setupHash(aSerialNumber);

		_aSerialNumber = aSerialNumber;
	}
}

std::string _FTDIChipset::bSerialNumber()
{
	return _bSerialNumber;
}

void _FTDIChipset::setBSerialNumber(const std::string& bSerialNumber)
{
	if (_bSerialNumber.empty())
	{
		setupHash(bSerialNumber);

		_bSerialNumber = bSerialNumber;
	}
}

std::string _FTDIChipset::cSerialNumber()
{
	return _cSerialNumber;
}

void _FTDIChipset::setCSerialNumber(const std::string& cSerialNumber)
{
	if (_cSerialNumber.empty())
	{
		setupHash(cSerialNumber);

		_cSerialNumber = cSerialNumber;
	}
}

std::string _FTDIChipset::dSerialNumber()
{
	return _dSerialNumber;
}

void _FTDIChipset::setDSerialNumber(const std::string& dSerialNumber)
{
	if (_dSerialNumber.empty())
	{
		setupHash(dSerialNumber);

		_dSerialNumber = dSerialNumber;
	}
}

bool _FTDIChipset::write
(
	uint8_t pin,
	bool state
)
{
	bool result{false};
	void* handle{nullptr};
	CharBit* charBit{nullptr};

	if (pin <= 7) // a
	{
		if (_aHandle != nullptr)
		{
			handle = _aHandle;
			_aPins.set(pin, state);
			charBit = &_aPins;
		}
	}
	else if (pin <= 15) // b
	{
		if (_bHandle != nullptr)
		{
			handle = _bHandle;
			_bPins.set(pin, state);
			charBit = &_bPins;
		}
	}
	else if (pin <= 23) // c
	{
		if (_cHandle != nullptr)
		{
			handle = _cHandle;
			_cPins.set(pin, state);
			charBit = &_cPins;
		}
	}
	else if (pin <= 31) // d
	{
		if (_dHandle != nullptr)
		{
			handle = _dHandle;
			_dPins.set(pin, state);
			charBit = &_dPins;
		}
	}

	if (handle != nullptr && charBit != nullptr)
	{
		FT_STATUS status{FT_OTHER_ERROR};

		if (_boardType == eFT232H)
		{
			uint8_t logicalValue = *charBit->value();
			uint8_t writeValue = logicalValue ^ _invertMask;
			uint8_t cbusValue = (0x0F << 4) | writeValue;

			char logBuf[128];
			std::snprintf(logBuf, sizeof(logBuf), "FT232H W  CBUS3=%d CBUS2=%d CBUS1=%d CBUS0=%d  pre=0x%02X ^inv=0x%02X hw=0x%02X",
				(logicalValue >> 3) & 1,
				(logicalValue >> 2) & 1,
				(logicalValue >> 1) & 1,
				(logicalValue >> 0) & 1,
				logicalValue,
				_invertMask,
				cbusValue);
			AppCore::writeToApplicationLogLine(logBuf);

			status = FT_SetBitMode(handle, cbusValue, FT_BITMODE_CBUS_BITBANG);

			UCHAR readBack = 0;
			FT_STATUS readStatus = FT_GetBitMode(handle, &readBack);
			if (readStatus == FT_OK)
			{
				std::snprintf(logBuf, sizeof(logBuf), "FT232H R  CBUS3=%d CBUS2=%d CBUS1=%d CBUS0=%d",
					(readBack >> 3) & 1,
					(readBack >> 2) & 1,
					(readBack >> 1) & 1,
					(readBack >> 0) & 1);
				AppCore::writeToApplicationLogLine(logBuf);

				uint8_t mismatch = (writeValue ^ readBack) & 0x0F;
				if (mismatch != 0)
				{
					std::snprintf(logBuf, sizeof(logBuf), "FT232H MISMATCH  bits=0x%02X  CBUS3=%d CBUS2=%d CBUS1=%d CBUS0=%d",
						mismatch,
						(mismatch >> 3) & 1,
						(mismatch >> 2) & 1,
						(mismatch >> 1) & 1,
						(mismatch >> 0) & 1);
					AppCore::writeToApplicationLogLine(logBuf);
				}
			}
			else
			{
				AppCore::writeToApplicationLogLine(
					"FT232H R  FT_GetBitMode failed: " + _FTDIChipset::ftidStatusToString(readStatus));
			}
		}
		else
		{
			DWORD byteWritten;
			status = FT_Write(handle, charBit->value(), 1, &byteWritten);
		}

		if (status == FT_OK)
		{
			result = true;
		}
		else
		{
			std::string message = "FTDI Write Failed: " + _FTDIChipset::ftidStatusToString(status)  + "\n";
			AppCore::writeToApplicationLog(message);
			_lastError = message;
		}
	}

	return result;
}

HashType _FTDIChipset::hash(const std::string &serialNumber)
{
	HashType result{0};

	result = ::arrayHash(serialNumber);

	return result;
}

void _FTDIChipset::setSerialNumber
(
	const std::string& serialNumber
)
{
	if (_serialNumber.empty())
	{
		_serialNumber = serialNumber;

		setupPortName();
	}
}

unsigned long long _FTDIChipset::setCustomVIDPID()
{
	unsigned long long ftStatus = !FT_OK;
#ifdef Q_OS_LINUX
	ftStatus = FT_SetVIDPID(1027, 6552);
	if (ftStatus == FT_OK)
		AppCore::writeToApplicationLog("Configured FTDI to detect VID: 0403 and PID: 1998\n");
#endif
	return ftStatus;
}

void _FTDIChipset::setupHash(const std::string& segmentSerialNumber)
{
	if (_hash == 0)
	{
		std::string serialNumber = _FTDIChipset::normalizeSerialNumber(_boardType, segmentSerialNumber);

		_hash = hash(serialNumber);
		_serialNumber = serialNumber;
	}
}

void _FTDIChipset::setupPortName()
{
	if (_portName.empty())
	{
		if (!_serialNumber.empty())
		{
			AlpacaSettings settings("Alpaca");

			settings.beginGroup("VTPs");
			unsigned int nextPort = settings.value("NextPortNumber", 1).toUInt();

			std::string vtpName = settings.value(_serialNumber, std::string()).toString();
			if (vtpName.empty())
			{
				vtpName = "VTP" + std::to_string(nextPort);
				settings.setValue(_serialNumber, vtpName);

				nextPort++;

				settings.setValue("NextPortNumber", nextPort);
			}

			settings.endGroup();

			_portName = vtpName;
		}
	}
}

PlatformID _FTDIChipset::nameToPlatform
(
	const std::string& deviceName
)
{
	return PlatformContainer::fromUSBDescriptor(deviceName);
}

#ifdef Q_OS_LINUX
void _FTDIChipset::linuxTraversal()
{
	FT_STATUS ftStatus;
	DWORD deviceCount;

	ftStatus = setCustomVIDPID();
	if (ftStatus == FT_OK)
	{
		// create the device information list
		ftStatus = FT_CreateDeviceInfoList(&deviceCount);
		if (ftStatus == FT_OK)
		{
			if (deviceCount > 0)
			{
				FT_DEVICE_LIST_INFO_NODE* devInfoList{nullptr};

				devInfoList = new FT_DEVICE_LIST_INFO_NODE[deviceCount];

				ftStatus = FT_GetDeviceInfoList(devInfoList, &deviceCount);
				if (ftStatus == FT_OK)
				{
					FT_HANDLE ftHandleTemp;
					DWORD Flags;
					DWORD ID;
					DWORD Type;
					DWORD LocId;
					char SerialNumber[16];
					char Description[64];

					for (auto i: range(deviceCount))
					{
						ftStatus = FT_GetDeviceInfoDetail(i, &Flags, &Type, &ID, &LocId, SerialNumber, Description, &ftHandleTemp);

						std::string description(Description);
						std::string usbDescriptor;
						if (description.length() >= 9 && description.compare(description.length() - 9, 9, "BugHopper") == 0)
							usbDescriptor = description;
						else if (description.length() >= 2)
							usbDescriptor = description.substr(0, description.length() - 2);

						if (_PlatformConfiguration::containsUSBDescriptor(usbDescriptor) || usbDescriptor.rfind("ALPACA-LITE ", 0) == 0)
						{
							PlatformID platformID = _PlatformConfiguration::getUSBDescriptor(usbDescriptor);
							std::string deviceSerialNumber(SerialNumber);
							TACPlatformEntry platformEntry = _PlatformConfiguration::getEntry(platformID);
							DebugBoardType boardType = platformEntry._platformEntry->_boardtype;
							std::string serialNumber = _FTDIChipset::normalizeSerialNumber(boardType, deviceSerialNumber);

							if (!serialNumber.empty())
							{
								uint portHash = hash(serialNumber);
								char segment = description.empty() ? '\0' : description.back();

								FTDIChipset ftdiChipset;

								if (hasDevice(portHash))
								{
									ftdiChipset = getDevice(serialNumber);
								}
								else
								{
									ftdiChipset = FTDIChipset(new _FTDIChipset);
									ftdiChipset->setPlatformID(platformID);
									ftdiChipset->setBoardType(boardType);

									_ftdiChipsetList.push_back(ftdiChipset);
								}

								ftdiChipset->setSerialNumber(serialNumber);
								if (ftdiChipset->_usbDescriptor.empty())
									ftdiChipset->_usbDescriptor = usbDescriptor;

								switch (segment)
								{
								case 'A':
								case 'a':
									ftdiChipset->setASerialNumber(deviceSerialNumber);
									break;

								case 'B':
								case 'b':
									ftdiChipset->setBSerialNumber(deviceSerialNumber);
									break;

								case 'C':
								case 'c':
								// Handling for Bug Hopper devices
								case 'R':
								case 'r':
									ftdiChipset->setCSerialNumber(deviceSerialNumber);
									break;

								case 'D':
								case 'd':
									ftdiChipset->setDSerialNumber(deviceSerialNumber);
									break;
								}

								ftdiChipset->_active = true;
							}
						}
					}
				}
			}
		}
	}
}

void _FTDIChipset::windowsTraversal()
{

}
#endif

#ifdef Q_OS_WIN
void _FTDIChipset::linuxTraversal()
{

}

void _FTDIChipset::windowsTraversal()
{
	FT_STATUS ftStatus;
	DWORD deviceCount;

	// create the device information list
	ftStatus = FT_CreateDeviceInfoList(&deviceCount);
	if (ftStatus == FT_OK)
	{
		if (deviceCount > 0)
		{
			FT_DEVICE_LIST_INFO_NODE* devInfoList{nullptr};

			devInfoList = new FT_DEVICE_LIST_INFO_NODE[deviceCount];

			ftStatus = FT_GetDeviceInfoList(devInfoList, &deviceCount);
			if (ftStatus == FT_OK)
			{
				for (auto i: range(deviceCount))
				{
					std::string description = devInfoList[i].Description ? devInfoList[i].Description : "";
					std::string usbDescriptor;
					if (description.length() >= 9 && description.compare(description.length() - 9, 9, "BugHopper") == 0)
						usbDescriptor = description;
					else if (description.length() >= 2)
						usbDescriptor = description.substr(0, description.length() - 2);

					if (_PlatformConfiguration::containsUSBDescriptor(usbDescriptor) || usbDescriptor.rfind("ALPACA-LITE ", 0) == 0)
					{
						PlatformID platformId = _PlatformConfiguration::getUSBDescriptor(usbDescriptor);
						std::string deviceSerialNumber = devInfoList[i].SerialNumber ? devInfoList[i].SerialNumber : "";
						TACPlatformEntry platformEntry = _PlatformConfiguration::getEntry(platformId);
						DebugBoardType boardType = platformEntry._platformEntry->_boardtype;
						std::string serialNumber = _FTDIChipset::normalizeSerialNumber(boardType, deviceSerialNumber);

						if (!serialNumber.empty())
						{
							uint portHash = hash(serialNumber);
							char segment = description.empty() ? '\0' : description.back(); // This is 'r' for Bug Hopper devices

							FTDIChipset ftdiChipset;

							if (hasDevice(portHash))
							{
								ftdiChipset = getDevice(serialNumber);
							}
							else
							{
								ftdiChipset = FTDIChipset(new _FTDIChipset);
								ftdiChipset->setPlatformID(platformId);
								ftdiChipset->setBoardType(boardType);

								_ftdiChipsetList.push_back(ftdiChipset);
							}

							ftdiChipset->setSerialNumber(serialNumber);
							if (ftdiChipset->_usbDescriptor.empty())
								ftdiChipset->_usbDescriptor = usbDescriptor;

							switch (segment)
							{
							case 'A':
							case 'a':
								ftdiChipset->setASerialNumber(deviceSerialNumber);
								break;

							case 'B':
							case 'b':
								ftdiChipset->setBSerialNumber(deviceSerialNumber);
								break;

							case 'C':
							case 'c':
							// Handling for Bug Hopper devices
							case 'R':
							case 'r':
								ftdiChipset->setCSerialNumber(deviceSerialNumber);
								break;

							case 'D':
							case 'd':
								ftdiChipset->setDSerialNumber(deviceSerialNumber);
								break;
							}

							ftdiChipset->_active = true;

							if (devInfoList[i].Flags == FT_FLAGS_OPENED)
								ftdiChipset->_lastError = "QTAC failed to enumerate all connected FTDI devices as they are in use";
						}
					}
				}
			}
		}
	}
}

#endif
