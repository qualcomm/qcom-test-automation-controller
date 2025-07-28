// Confidential and Proprietary Qualcomm Technologies, Inc.

// NO PUBLIC DISCLOSURE PERMITTED:  Please report postings of this software on public servers or websites
// to: DocCtrlAgent@qualcomm.com.

// RESTRICTED USE AND DISCLOSURE:
// This software contains confidential and proprietary information and is not to be used, copied, reproduced, modified
// or distributed, in whole or in part, nor its contents revealed in any manner, without the express written permission
// of Qualcomm Technologies, Inc.

// Qualcomm is a trademark of QUALCOMM Incorporated, registered in the United States and other countries. All
// QUALCOMM Incorporated trademarks are used with permission.

// This software may be subject to U.S. and international export, re-export, or transfer laws.  Diversion contrary to U.S.
// and international law is strictly prohibited.

// Qualcomm Technologies, Inc.
// 5775 Morehouse Drive
// San Diego, CA 92121 U.S.A.
// Copyright 2022-2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "FTDIDevice.h"

// libTAC
#include "private/TACLiteDriveThread.h"

// QCommonConsolde
#include "AppCore.h"
#include "FTDIPlatformConfiguration.h"
#include "mymemcpy.h"
#include "Range.h"

// Qt
#include <QThread>

// FTDI
#include "ftd2xx.h"

class FTDIInitializer
{
public:
	FTDIInitializer()
	{
#if defined(FTD2XX_STATIC)
		FT_Initialise();
#endif
	}

	~FTDIInitializer()
	{
#if defined(FTD2XX_STATIC)
		FT_Finalise();
#endif
	}
} gFTDIInitializer;

FTDIDevice::~FTDIDevice()
{

}

bool FTDIDevice::programDevice
(
	AlpacaDevice alpacaDevice,
	PlatformID platformID,
	QByteArray& errorMessage
)
{
	bool result{false};

	TACPlatformEntry tacPlatformEntry = _PlatformConfiguration::getEntry(platformID);
	if (tacPlatformEntry._platformEntry->_platformID == platformID)
	{
		FT_STATUS ftStatus;
		FT_HANDLE ftHandle{Q_NULLPTR};

		QByteArray serialNumber = alpacaDevice->serialNumber() + "A";
		QByteArray usbDescriptor = tacPlatformEntry._platformEntry->_usbDescriptor;

		if (usbDescriptor.isEmpty())
			usbDescriptor = "ALPACA-LITE MTP DEBUG BOARD";

		ftStatus = FT_OpenEx(serialNumber.data(), FT_OPEN_BY_SERIAL_NUMBER, &ftHandle);
		if (ftStatus == FT_OK)
		{
			FT_PROGRAM_DATA ftData;

			::memset(&ftData, 0, sizeof(FT_PROGRAM_DATA));

			char manufacturerBuf[32];
			char manufacturerIdBuf[16];
			char descriptionBuf[64];
			char serialNumberBuf[16];

			ftData.Signature1 = 0x00000000;
			ftData.Signature2 = 0xffffffff;
			ftData.Version = 0x00000008; // EEPROM structure with Rev 8 (FT4232H) extensions
			ftData.Manufacturer = manufacturerBuf;
			ftData.ManufacturerId = manufacturerIdBuf;
			ftData.Description = descriptionBuf;
			ftData.SerialNumber = serialNumberBuf;

			ftStatus = FT_EE_Read(ftHandle, &ftData);
			if (ftStatus == FT_OK)
			{
				my_memcpy(descriptionBuf, sizeof(descriptionBuf), usbDescriptor, usbDescriptor.size());

				bool temp;

				temp = tacPlatformEntry._platformEntry->_pinSets[0].testFlag(eA);
				ftData.AIsVCP8 = temp ? 0 : 1;

				temp = tacPlatformEntry._platformEntry->_pinSets[0].testFlag(eB);
				ftData.BIsVCP8 = temp ? 0 : 1;

				temp = tacPlatformEntry._platformEntry->_pinSets[0].testFlag(eC);
				ftData.CIsVCP8 = temp ? 0 : 1;

				temp = tacPlatformEntry._platformEntry->_pinSets[0].testFlag(eD);
				ftData.DIsVCP8= temp ? 0 : 1;

				ftStatus = FT_EE_Program(ftHandle, &ftData);
				if (ftStatus == FT_OK)
				{
					result = true;
					FT_CyclePort(ftHandle);
				}
				else
				{

					errorMessage = QString("FT_EE_Program failed Error Code: %1").arg(_FTDIChipset::ftidStatusToString(ftStatus)).toLatin1();
				}
			}
			else
			{
				errorMessage = QString("FT_EE_ReadEx failed Error Code: %1").arg(_FTDIChipset::ftidStatusToString(ftStatus)).toLatin1();
			}

			FT_Close(ftHandle);
		}
		else
		{
			errorMessage = QString("FT_OpenEx failed Error Code: %1").arg(_FTDIChipset::ftidStatusToString(ftStatus)).toLatin1();
		}
	}

	return result;
}

quint32 FTDIDevice::updateAlpacaDevices()
{
	int deviceCount = _FTDIChipset::getDeviceCount();
	for (auto deviceIndex : range(deviceCount))
	{
		FTDIChipset ftdiChipset = _FTDIChipset::getDevice(deviceIndex);

		HashType hash = ftdiChipset->hash();

		AlpacaDevice alpacaDevice = _AlpacaDevice::findAlpacaDevice(hash);
		if (alpacaDevice.isNull())
		{
			FTDIDevice* ftdiDevice = new FTDIDevice;

			ftdiDevice->_active = true;
			ftdiDevice->_boardType = eFTDI;
			ftdiDevice->_portName = ftdiChipset->portName();
			ftdiDevice->_hash = hash;
			ftdiDevice->_chipVersion = 10000;
			ftdiDevice->_usbDescriptor = ftdiChipset->usbDescriptor();
			ftdiDevice->_serialNumber = ftdiChipset->serialNumber();
			ftdiDevice->_platformID = ftdiChipset->platformID();

			TACPlatformEntry platformEntry = _PlatformConfiguration::getEntry(ftdiDevice->_platformID);
			ftdiDevice->_description = platformEntry._platformEntry->_description.toLatin1();
			ftdiDevice->_platformConfiguration = platformEntry.getConfiguration();

			alpacaDevice = AlpacaDevice(ftdiDevice);
			_AlpacaDevice::_alpacaDevices.push_back(alpacaDevice);

			AppCore::writeToApplicationLog("Found a FTDI device with port name: '" + ftdiDevice->portName() + "' and USB descriptor string: '" + ftdiDevice->usbDescriptor() + "'\n");
		}
		else
		{
			alpacaDevice->setActive();
		}
	}

	return _AlpacaDevice::_alpacaDevices.count();
}

bool FTDIDevice::open()
{
	const int maxInterations{50};

	bool result{false};

	if (_platformConfiguration.isNull() == false)
	{
		_ftdiPlatformConfiguration = static_cast<_FTDIPlatformConfiguration*>(_platformConfiguration.data());

		buildMapping();

		if (_driveThread == Q_NULLPTR)
		{
			TacLiteDriveThread* driveThread = new TacLiteDriveThread(_hash);
			driveThread->setPinSets(_ftdiPlatformConfiguration->getPinSet(0));
			_driveThread = driveThread;
			_driveThread->start();

			int count = 0;
			while (count < maxInterations)
			{
				QThread::msleep(20);

				result = _driveThread->weAreRunning();
				if (result == true)
					count = maxInterations;

				count++;
			}

			if (result == true)
			{

				connect(driveThread, &TacLiteDriveThread::pinStateChanged, this, &FTDIDevice::on_pinStateChanged, Qt::DirectConnection);

				Pins initialPins;
				Pins pins = _platformConfiguration->getPins();
				for (const auto& pin: pins)
				{
					if (pin._initialValue == true)
						initialPins.append(pin);
				}

				if (initialPins.count() > 0)
				{
					auto sortLambda = [] (PinEntry& p1, PinEntry& p2) -> bool
					{
						return p1._initializationPriority < p2._initializationPriority;
					};

					std::sort(initialPins.begin(), initialPins.end(), sortLambda);

					for (const auto& initializationPin: initialPins)
					{
						_driveThread->setPinState(initializationPin._pin, initializationPin._initialValue);

						if (AppCore::getAppCore()->appLoggingActive())
						{
							QString message = QString("LiteDevice::open() Initialize Pin: %1\n").arg(initializationPin._pin);
							AppCore::writeToApplicationLog(message);
						}
					}
				}
			}
		}
	}

	return result;
}

void FTDIDevice::buildMapping()
{
	Q_ASSERT(_ftdiPlatformConfiguration != Q_NULLPTR);

	buildCommandList();
	buildQuickSettings();
}

void FTDIDevice::buildCommandList()
{
	if (_commands.isEmpty() == true)
	{
		FTDIPinList ftdiPinList = _ftdiPlatformConfiguration->getActivePins();

		for (const auto& ftdiPin: std::as_const(ftdiPinList))
		{
			TacCommand tacCommand;

			tacCommand._pin = ftdiPin._setPin;
			tacCommand._command = ftdiPin._pinCommand;
			tacCommand._helpText = ftdiPin._pinTooltip;
			tacCommand._currentState = ftdiPin._initialValue;
			tacCommand._isInverted = ftdiPin._inverted;
			tacCommand._tabName = ftdiPin._tabName.toLatin1();
			tacCommand._groupName = CommandGroup::toString(ftdiPin._commandGroup).toLatin1();

			QByteArray cellLocation = QString::number(ftdiPin._cellLocation.x()).toLatin1() + "," + QString::number(ftdiPin._cellLocation.y()).toLatin1();
			tacCommand._cellLocation = cellLocation;

			_commands[tacCommand._command] = tacCommand;
		}
	}
}
