/*
	Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries. 
	 
	Redistribution and use in source and binary forms, with or without
	modification, are permitted (subject to the limitations in the
	disclaimer below) provided that the following conditions are met:
	 
		* Redistributions of source code must retain the above copyright
		  notice, this list of conditions and the following disclaimer.
	 
		* Redistributions in binary form must reproduce the above
		  copyright notice, this list of conditions and the following
		  disclaimer in the documentation and/or other materials provided
		  with the distribution.
	 
		* Neither the name of Qualcomm Technologies, Inc. nor the names of its
		  contributors may be used to endorse or promote products derived
		  from this software without specific prior written permission.
	 
	NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
	GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
	HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
	IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// Author: msimpson, biswroy

#include "FTDIChipset.h"

// QCommon
#include "AppCore.h"
#include "PlatformConfiguration.h"
#include "Range.h"
#include "version.h"

// Qt
#include <QSettings>

// FTDI
#include "ftd2xx.h"

FTDIChipsetList _FTDIChipset::_ftdiChipsetList;

_FTDIChipset::_FTDIChipset()
{

}

QString _FTDIChipset::ftidStatusToString
(
	unsigned long status
)
{
	QString numberCode{QString::number(status)};
	QString result{QString("Status ") + numberCode + QString(" not found")};

	switch (status)
	{
	case FT_OK: result = "Status Okay"; break;
	case FT_INVALID_HANDLE: result = QString("Invalid handle: %1").arg(numberCode); break;
	case FT_DEVICE_NOT_FOUND: result = QString("Device not found: %1").arg(numberCode); break;
	case FT_DEVICE_NOT_OPENED: result = QString("Device not opened: %1").arg(numberCode); break;
	case FT_IO_ERROR: result = QString("IO error: %1").arg(numberCode); break;
	case FT_INSUFFICIENT_RESOURCES: result = QString("Insufficient resources: %1").arg(numberCode); break;
	case FT_INVALID_PARAMETER: result = QString("Invalid parameter: %1").arg(numberCode); break;
	case FT_INVALID_BAUD_RATE: result = QString("Invalid baud rate: %1").arg(numberCode); break;
	case FT_DEVICE_NOT_OPENED_FOR_ERASE: result = QString("Device not opened for erase: %1").arg(numberCode); break;
	case FT_DEVICE_NOT_OPENED_FOR_WRITE: result = QString("Device not opened for write: %1").arg(numberCode); break;
	case FT_FAILED_TO_WRITE_DEVICE: result = QString("Invalid Handle: %1").arg(numberCode); break;
	case FT_EEPROM_READ_FAILED: result = QString("EEPROM read failed: %1").arg(numberCode); break;
	case FT_EEPROM_WRITE_FAILED: result = QString("EEPROM write failed: %1").arg(numberCode); break;
	case FT_EEPROM_ERASE_FAILED: result = QString("EEPROM erase failed: %1").arg(numberCode); break;
	case FT_EEPROM_NOT_PRESENT: result = QString("EEPROM not present: %1").arg(numberCode); break;
	case FT_EEPROM_NOT_PROGRAMMED: result = QString("EEPROM not programmed: %1").arg(numberCode); break;
	case FT_INVALID_ARGS: result = QString("Invalid Arguments: %1").arg(numberCode); break;
	case FT_NOT_SUPPORTED: result = QString("Not supported: %1").arg(numberCode); break;
	case FT_OTHER_ERROR: result = QString("Other error: %1").arg(numberCode); break;
	case FT_DEVICE_LIST_NOT_READY: result = QString("Device list not ready: %1").arg(numberCode); break;
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

quint32 _FTDIChipset::getDeviceCount()
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
	quint32 deviceIndex
)
{
	FTDIChipset result;

	if (deviceIndex < static_cast<quint32>(_ftdiChipsetList.count()))
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

QByteArray _FTDIChipset::normalizeSerialNumber
(
	const QByteArray& segmentSerialNumber
)
{
	QByteArray serialNumber{segmentSerialNumber};
	serialNumber = serialNumber.remove(serialNumber.length() - 1, 1);

	return serialNumber;
}

bool _FTDIChipset::open
(
	FTDIPinSets pinsets
)
{
	bool result{false};
	quint8 mask = 0xff;
	quint8 mode = FT_BITMODE_ASYNC_BITBANG;

	FT_STATUS ftStatus;

	AppCore::writeToApplicationLog(QString("Bus A : ") + (pinsets.testFlag(eA) ? "true" : "false") + "\n");
	AppCore::writeToApplicationLog(QString("Bus B : ") + (pinsets.testFlag(eB) ? "true" : "false") + "\n");
	AppCore::writeToApplicationLog(QString("Bus C : ") + (pinsets.testFlag(eC) ? "true" : "false") + "\n");
	AppCore::writeToApplicationLog(QString("Bus D : ") + (pinsets.testFlag(eD) ? "true" : "false") + "\n");

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
			AppCore::writeToApplicationLog(QString("FTDI Port A failure: ") + _FTDIChipset::ftidStatusToString(ftStatus) + "\n");
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
			AppCore::writeToApplicationLog(QString("FTDI Port B failure: ") + _FTDIChipset::ftidStatusToString(ftStatus) + "\n");
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
			AppCore::writeToApplicationLog(QString("FTDI Port C failure: ") + _FTDIChipset::ftidStatusToString(ftStatus) + "\n");
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
			AppCore::writeToApplicationLog(QString("FTDI Port D failure: ") + _FTDIChipset::ftidStatusToString(ftStatus) + "\n");
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
			AppCore::writeToApplicationLog(QString("FTDI Port A close failure: ") + _FTDIChipset::ftidStatusToString(ftStatus) + "\n");
	}

	if (_bHandle != Q_NULLPTR)
	{
		FT_SetBitMode(_bHandle, 0, 0);
		ftStatus = FT_Close(_bHandle);
		if (ftStatus == FT_OK)
			_bHandle = Q_NULLPTR;
		else
			AppCore::writeToApplicationLog(QString("FTDI Port B close failure: ") + _FTDIChipset::ftidStatusToString(ftStatus) + "\n");
	}

	if (_cHandle != Q_NULLPTR)
	{
		FT_SetBitMode(_cHandle, 0, 0);
		ftStatus = FT_Close(_cHandle);
		if (ftStatus == FT_OK)
			_cHandle = Q_NULLPTR;
		else
			AppCore::writeToApplicationLog(QString("FTDI Port C close failure: ") + _FTDIChipset::ftidStatusToString(ftStatus) + "\n");
	}

	if (_dHandle != Q_NULLPTR)
	{
		FT_SetBitMode(_dHandle, 0, 0);
		ftStatus = FT_Close(_dHandle);
		if (ftStatus == FT_OK)
			_dHandle = Q_NULLPTR;
		else
			AppCore::writeToApplicationLog(QString("FTDI Port D close failure: ") + _FTDIChipset::ftidStatusToString(ftStatus) + "\n");
	}
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

	result = PlatformContainer::toString(_platformID) + " " + firmwareString();

	return result;
}

QString _FTDIChipset::firmwareString()
{
	return TAC_LIB_VERSION;
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

void _FTDIChipset::setASerialNumber
(
	const QByteArray& aSerialNumber
)
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

void _FTDIChipset::setBSerialNumber
(
	const QByteArray& bSerialNumber
)
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

void _FTDIChipset::setCSerialNumber
(
	const QByteArray& cSerialNumber
)
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

void _FTDIChipset::setDSerialNumber
(
	const QByteArray& dSerialNumber
)
{
	if (_dSerialNumber.isEmpty())
	{
		setupHash(dSerialNumber);

		_dSerialNumber = dSerialNumber;
	}
}

bool _FTDIChipset::write
(
	quint8 pin,
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
		DWORD byteWritten;

		FT_STATUS status = FT_Write(handle, charBit->value(), 1, &byteWritten);
		if (status == FT_OK)
		{
			result = true;
		}
		else
		{
			QString message = "FTDI Write Failed: " + _FTDIChipset::ftidStatusToString(status)  + "\n";
			AppCore::writeToApplicationLog(message);
			_lastError = message.toLatin1();
		}
	}

	return result;
}

HashType _FTDIChipset::hash(const QByteArray &serialNumber)
{
	HashType result{0};

	result = ::arrayHash(serialNumber);

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

void _FTDIChipset::setupHash
(
	const QByteArray& segmentSerialNumber
)
{
	if (_hash == 0)
	{
		QByteArray serialNumber = _FTDIChipset::normalizeSerialNumber(segmentSerialNumber);

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
			QSettings settings(QSettings::UserScope, "Qualcomm, Inc.", "QTAC");

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
						QByteArray usbDescriptor = description.left(description.length() - 2).toLatin1();
						if (_PlatformConfiguration::containsUSBDescriptor(usbDescriptor) || usbDescriptor.startsWith("ALPACA-LITE "))
						{
							PlatformID platformID = _PlatformConfiguration::getUSBDescriptor(usbDescriptor);
							QByteArray deviceSerialNumber(SerialNumber);
							QByteArray serialNumber = _FTDIChipset::normalizeSerialNumber(deviceSerialNumber);

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
					QByteArray usbDescriptor = description.left(description.length() - 2).toLatin1();
					if (_PlatformConfiguration::containsUSBDescriptor(usbDescriptor) || usbDescriptor.startsWith("ALPACA-LITE "))
					{
						PlatformID platformID = _PlatformConfiguration::getUSBDescriptor(usbDescriptor);
						QByteArray deviceSerialNumber = devInfoList[i].SerialNumber;
						QByteArray serialNumber = _FTDIChipset::normalizeSerialNumber(deviceSerialNumber);

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

