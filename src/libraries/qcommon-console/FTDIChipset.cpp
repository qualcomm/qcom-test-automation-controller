// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "FTDIChipset.h"

// QCommon
#include "AppCore.h"
#include "PlatformConfiguration.h"
#include "Range.h"
#include "version.h"

// Qt
#include <QSettings>

#include <cstdint>

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

	return _ftdiChipsetList.count();
}

FTDIChipset _FTDIChipset::getDevice
(
	uint32_t deviceIndex
)
{
	FTDIChipset result;

	if (deviceIndex < static_cast<uint32_t>(_ftdiChipsetList.count()))
		result = _ftdiChipsetList.at(deviceIndex);

	return result;
}

FTDIChipset _FTDIChipset::getDevice
(
	const QByteArray& portName
)
{
	for (auto& ftdiChipset: _ftdiChipsetList)
	{
		QByteArray candidate = ftdiChipset->portName();
		if (candidate.contains(portName))
			return ftdiChipset;

		candidate = ftdiChipset->serialNumber();
		if (candidate.contains(portName))
			return ftdiChipset;
	}

	return FTDIChipset();
}

QByteArray _FTDIChipset::normalizeSerialNumber(DebugBoardType boardType, const QByteArray& segmentSerialNumber)
{
	QByteArray serialNumber{segmentSerialNumber};

	if (boardType == eFTDI)
	{
		serialNumber = serialNumber.remove(serialNumber.length() - 1, 1);
	}

	return serialNumber;
}

bool _FTDIChipset::open(FTDIPinSets pinsets)
{
	bool result{false};
	uint8_t mask;
	uint8_t mode;

	if (_boardType == eFTDI)
	{
		mask = 0xff;
		mode = FT_BITMODE_ASYNC_BITBANG;
	}
	else
	{
		mask = 0x0f;
		mode = FT_BITMODE_CBUS_BITBANG;
	}

	FT_STATUS ftStatus;

	AppCore::writeToApplicationLog(std::string("Bus A : ") + (pinsets.testFlag(eA) ? "true" : "false") + "\n");
	AppCore::writeToApplicationLog(std::string("Bus B : ") + (pinsets.testFlag(eB) ? "true" : "false") + "\n");
	AppCore::writeToApplicationLog(std::string("Bus C : ") + (pinsets.testFlag(eC) ? "true" : "false") + "\n");
	AppCore::writeToApplicationLog(std::string("Bus D : ") + (pinsets.testFlag(eD) ? "true" : "false") + "\n");

	if (pinsets.testFlag(eA))
	{
		ftStatus = FT_OpenEx(_aSerialNumber.data(), FT_OPEN_BY_SERIAL_NUMBER, &_aHandle);
		if (ftStatus == FT_OK)
		{
			FT_SetBitMode(_aHandle, mask, mode);
			result = true;
		}
		else
		{
            result = false;
			AppCore::writeToApplicationLog("FTDI Port A failure: " + _FTDIChipset::ftidStatusToString(ftStatus) + "\n");
			_aHandle = Q_NULLPTR;
		}
	}

	if (pinsets.testFlag(eB))
	{
		ftStatus = FT_OpenEx(_bSerialNumber.data(), FT_OPEN_BY_SERIAL_NUMBER, &_bHandle);
		if (ftStatus == FT_OK)
		{
			FT_SetBitMode(_bHandle, mask, mode);
			result = true;
		}
		else
		{
            result = false;
			AppCore::writeToApplicationLog("FTDI Port B failure: " + _FTDIChipset::ftidStatusToString(ftStatus) + "\n");
			_bHandle = Q_NULLPTR;
		}
	}

	if (pinsets.testFlag(eC))
	{
		ftStatus = FT_OpenEx(_cSerialNumber.data(), FT_OPEN_BY_SERIAL_NUMBER, &_cHandle);
		if (ftStatus == FT_OK)
		{
			FT_SetBitMode(_cHandle, mask, mode);
			result = true;
		}
		else
		{
            result = false;
			AppCore::writeToApplicationLog("FTDI Port C failure: " + _FTDIChipset::ftidStatusToString(ftStatus) + "\n");
			_cHandle = Q_NULLPTR;
		}
	}

	if (pinsets.testFlag(eD))
	{
		ftStatus = FT_OpenEx(_dSerialNumber.data(), FT_OPEN_BY_SERIAL_NUMBER, &_dHandle);
		if (ftStatus == FT_OK)
		{
			FT_SetBitMode(_dHandle, mask, mode);
			result = true;
		}
		else
		{
            result = false;
			AppCore::writeToApplicationLog("FTDI Port D failure: " + _FTDIChipset::ftidStatusToString(ftStatus) + "\n");
			_dHandle = Q_NULLPTR;
		}
	}

	return result;
}

bool _FTDIChipset::isOpen()
{
	bool result{false};

	result = _aHandle != Q_NULLPTR;

	if (result == false)
		result = _bHandle != Q_NULLPTR;

	if (result == false)
		result = _cHandle != Q_NULLPTR;

	if (result == false)
		result = _dHandle != Q_NULLPTR;

	return result;
}

void _FTDIChipset::close()
{
	FT_STATUS ftStatus;

	if (_aHandle != Q_NULLPTR)
	{
		FT_SetBitMode(_aHandle, 0, 0);

		ftStatus = FT_Close(_aHandle);
		if (ftStatus == FT_OK)
			_aHandle = Q_NULLPTR;
		else
			AppCore::writeToApplicationLog("FTDI Port A close failure: " + _FTDIChipset::ftidStatusToString(ftStatus) + "\n");
	}

	if (_bHandle != Q_NULLPTR)
	{
		FT_SetBitMode(_bHandle, 0, 0);
		ftStatus = FT_Close(_bHandle);
		if (ftStatus == FT_OK)
			_bHandle = Q_NULLPTR;
		else
			AppCore::writeToApplicationLog("FTDI Port B close failure: " + _FTDIChipset::ftidStatusToString(ftStatus) + "\n");
	}

	if (_cHandle != Q_NULLPTR)
	{
		FT_SetBitMode(_cHandle, 0, 0);
		ftStatus = FT_Close(_cHandle);
		if (ftStatus == FT_OK)
			_cHandle = Q_NULLPTR;
		else
			AppCore::writeToApplicationLog("FTDI Port C close failure: " + _FTDIChipset::ftidStatusToString(ftStatus) + "\n");
	}

	if (_dHandle != Q_NULLPTR)
	{
		FT_SetBitMode(_dHandle, 0, 0);
		ftStatus = FT_Close(_dHandle);
		if (ftStatus == FT_OK)
			_dHandle = Q_NULLPTR;
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

QString _FTDIChipset::versionString()
{
	QString result;

	result = QString::fromStdString(PlatformContainer::toString(_platformID)) + " " + firmwareString();

	return result;
}

QString _FTDIChipset::firmwareString()
{
	return QCOMMONCONSOLE_VERSION;
}

HashType _FTDIChipset::hash()
{
	return _hash;
}

QByteArray _FTDIChipset::serialNumber()
{
	return _serialNumber;
}

bool _FTDIChipset::newDevice()
{
	bool result{_new};

	_new = false;

	return result;
}

QByteArray _FTDIChipset::aSerialNumber()
{
	return _aSerialNumber;
}

void _FTDIChipset::setASerialNumber(const QByteArray& aSerialNumber)
{
	if (_aSerialNumber.isEmpty())
	{
		setupHash(aSerialNumber);

		_aSerialNumber = aSerialNumber;
	}
}

QByteArray _FTDIChipset::bSerialNumber()
{
	return _bSerialNumber;
}

void _FTDIChipset::setBSerialNumber(const QByteArray& bSerialNumber)
{
	if (_bSerialNumber.isEmpty())
	{
		setupHash(bSerialNumber);

		_bSerialNumber = bSerialNumber;
	}
}

QByteArray _FTDIChipset::cSerialNumber()
{
	return _cSerialNumber;
}

void _FTDIChipset::setCSerialNumber(const QByteArray& cSerialNumber)
{
	if (_cSerialNumber.isEmpty())
	{
		setupHash(cSerialNumber);

		_cSerialNumber = cSerialNumber;
	}
}

QByteArray _FTDIChipset::dSerialNumber()
{
	return _dSerialNumber;
}

void _FTDIChipset::setDSerialNumber(const QByteArray& dSerialNumber)
{
	if (_dSerialNumber.isEmpty())
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
	void* handle{Q_NULLPTR};
	CharBit* charBit{Q_NULLPTR};

	if (pin <= 7) // a
	{
		if (_aHandle != Q_NULLPTR)
		{
			handle = _aHandle;
			_aPins.set(pin, state);
			charBit = &_aPins;
		}
	}
	else if (pin <= 15) // b
	{
		if (_bHandle != Q_NULLPTR)
		{
			handle = _bHandle;
			_bPins.set(pin, state);
			charBit = &_bPins;
		}
	}
	else if (pin <= 23) // c
	{
		if (_cHandle != Q_NULLPTR)
		{
			handle = _cHandle;
			_cPins.set(pin, state);
			charBit = &_cPins;
		}
	}
	else if (pin <= 31) // d
	{
		if (_dHandle != Q_NULLPTR)
		{
			handle = _dHandle;
			_dPins.set(pin, state);
			charBit = &_dPins;
		}
	}

	if (handle != Q_NULLPTR && charBit != Q_NULLPTR)
	{
		FT_STATUS status{FT_OTHER_ERROR};

		if (_boardType == eFT232H)
		{
			uint8_t logicalValue = *charBit->value();
			uint8_t writeValue = logicalValue ^ _invertMask;
			uint8_t cbusValue = (0x0F << 4) | writeValue;

			AppCore::writeToApplicationLogLine(
				QString("FT232H W  CBUS3=%1 CBUS2=%2 CBUS1=%3 CBUS0=%4  pre=0x%5 ^inv=0x%6 hw=0x%7")
				.arg((logicalValue >> 3) & 1)
				.arg((logicalValue >> 2) & 1)
				.arg((logicalValue >> 1) & 1)
				.arg((logicalValue >> 0) & 1)
				.arg(logicalValue, 2, 16, QChar('0'))
				.arg(_invertMask, 2, 16, QChar('0'))
				.arg(cbusValue, 2, 16, QChar('0')).toStdString());

			status = FT_SetBitMode(handle, cbusValue, FT_BITMODE_CBUS_BITBANG);

			UCHAR readBack = 0;
			FT_STATUS readStatus = FT_GetBitMode(handle, &readBack);
			if (readStatus == FT_OK)
			{
				AppCore::writeToApplicationLogLine(
					QString("FT232H R  CBUS3=%1 CBUS2=%2 CBUS1=%3 CBUS0=%4")
					.arg((readBack >> 3) & 1)
					.arg((readBack >> 2) & 1)
					.arg((readBack >> 1) & 1)
					.arg((readBack >> 0) & 1).toStdString());

				uint8_t mismatch = (writeValue ^ readBack) & 0x0F;
				if (mismatch != 0)
				{
					AppCore::writeToApplicationLogLine(
						QString("FT232H MISMATCH  bits=0x%1  CBUS3=%2 CBUS2=%3 CBUS1=%4 CBUS0=%5")
						.arg(mismatch, 2, 16, QChar('0'))
						.arg((mismatch >> 3) & 1)
						.arg((mismatch >> 2) & 1)
						.arg((mismatch >> 1) & 1)
						.arg((mismatch >> 0) & 1).toStdString());
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
			_lastError = QByteArray::fromStdString(message);
		}
	}

	return result;
}

HashType _FTDIChipset::hash(const QByteArray &serialNumber)
{
	HashType result{0};

	result = ::arrayHash(serialNumber.toStdString());

	return result;
}

void _FTDIChipset::setSerialNumber
(
	const QByteArray& serialNumber
)
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
#ifdef Q_OS_LINUX
	ftStatus = FT_SetVIDPID(1027, 6552);
	if (ftStatus == FT_OK)
		AppCore::writeToApplicationLog("Configured FTDI to detect VID: 0403 and PID: 1998\n");
#endif
	return ftStatus;
}

void _FTDIChipset::setupHash(const QByteArray& segmentSerialNumber)
{
	if (_hash == 0)
	{
		QByteArray serialNumber = _FTDIChipset::normalizeSerialNumber(_boardType, segmentSerialNumber);

		_hash = hash(serialNumber);
		_serialNumber = serialNumber;
	}
}

void _FTDIChipset::setupPortName()
{
	if (_portName.isEmpty())
	{
		if (_serialNumber.isEmpty() == false)
		{
			QSettings settings(QSettings::UserScope, "Qualcomm, Inc.", "Alpaca");

			settings.beginGroup("VTPs");
			uint nextPort = settings.value("NextPortNumber", 1).toUInt();

			QString vtpName = settings.value(_serialNumber, QString()).toString();
			if (vtpName.isEmpty())
			{
				vtpName = QString("VTP%1").arg(nextPort);
				settings.setValue(_serialNumber, vtpName);

				nextPort++;

				settings.setValue("NextPortNumber", nextPort);
			}

			settings.endGroup();

			_portName = vtpName.toLatin1();
		}
	}
}

PlatformID _FTDIChipset::nameToPlatform
(
	const QByteArray& deviceName
)
{
	return PlatformContainer::fromUSBDescriptor(deviceName.toStdString());
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
				FT_DEVICE_LIST_INFO_NODE* devInfoList{Q_NULLPTR};

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

						QString description(Description);
						QByteArray usbDescriptor;
						if (description.endsWith("BugHopper", Qt::CaseInsensitive))
							usbDescriptor = description.toLatin1();
						else
							usbDescriptor = description.left(description.length() - 2).toLatin1();
						if (_PlatformConfiguration::containsUSBDescriptor(usbDescriptor) || usbDescriptor.startsWith("ALPACA-LITE "))
						{
							PlatformID platformID = _PlatformConfiguration::getUSBDescriptor(usbDescriptor);
							QByteArray deviceSerialNumber(SerialNumber);
							TACPlatformEntry platformEntry = _PlatformConfiguration::getEntry(platformID);
							DebugBoardType boardType = platformEntry._platformEntry->_boardtype;
							QByteArray serialNumber = _FTDIChipset::normalizeSerialNumber(boardType, deviceSerialNumber);

							if (serialNumber.isEmpty() == false)
							{
								uint portHash = hash(serialNumber);
								QChar segment = description.right(1)[0];

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

									_ftdiChipsetList.append(ftdiChipset);
								}

								ftdiChipset->setSerialNumber(serialNumber);
								if (ftdiChipset->_usbDescriptor.isEmpty() == true)
									ftdiChipset->_usbDescriptor = usbDescriptor;

								switch (segment.toLatin1())
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
			FT_DEVICE_LIST_INFO_NODE* devInfoList{Q_NULLPTR};

			devInfoList = new FT_DEVICE_LIST_INFO_NODE[deviceCount];

			ftStatus = FT_GetDeviceInfoList(devInfoList, &deviceCount);
			if (ftStatus == FT_OK)
			{
				for (auto i: range(deviceCount))
				{
					QString description = devInfoList[i].Description;
					QByteArray usbDescriptor;
					if (description.endsWith("BugHopper", Qt::CaseInsensitive))
						usbDescriptor = description.toLatin1();
					else
						usbDescriptor = description.left(description.length() - 2).toLatin1();
					if (_PlatformConfiguration::containsUSBDescriptor(usbDescriptor) || usbDescriptor.startsWith("ALPACA-LITE "))
					{
						PlatformID platformId = _PlatformConfiguration::getUSBDescriptor(usbDescriptor);
						QByteArray deviceSerialNumber = devInfoList[i].SerialNumber;
						TACPlatformEntry platformEntry = _PlatformConfiguration::getEntry(platformId);
						DebugBoardType boardType = platformEntry._platformEntry->_boardtype;
						QByteArray serialNumber = _FTDIChipset::normalizeSerialNumber(boardType, deviceSerialNumber);


						if (serialNumber.isEmpty() == false)
						{
							uint portHash = hash(serialNumber);

							QChar segment = description.right(1)[0]; // This is 'r' for Bug Hopper devices

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

								_ftdiChipsetList.append(ftdiChipset);
							}

							ftdiChipset->setSerialNumber(serialNumber);
							if (ftdiChipset->_usbDescriptor.isEmpty())
								ftdiChipset->_usbDescriptor = usbDescriptor;

							switch (segment.toLatin1())
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

							if (devInfoList->Flags == FT_FLAGS_OPENED)
								ftdiChipset->_lastError = "QTAC failed to enumerate all connected FTDI devices as they are in use";
						}
					}
				}
			}
		}
	}
}

#endif
