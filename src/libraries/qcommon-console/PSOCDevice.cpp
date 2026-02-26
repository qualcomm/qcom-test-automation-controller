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
// Copyright 2022 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "PSOCDevice.h"
#include "private/TACPSOCDriveThread.h"

// QCommonConsole
#include "AppCore.h"
#include "PSOCPlatformConfiguration.h"
#include "Range.h"
#include "PSOCSerialTableModel.h"

PSOCSerialTableModel* gPSOCTACSerialTableModel{Q_NULLPTR};

PSOCDevice::~PSOCDevice()
{
}

quint32 PSOCDevice::updateAlpacaDevices()
{
	if (gPSOCTACSerialTableModel == Q_NULLPTR)
		gPSOCTACSerialTableModel = new PSOCSerialTableModel;

	if (gPSOCTACSerialTableModel == Q_NULLPTR)
		return 0;

	for (auto& alpacaDevice: _AlpacaDevice::_alpacaDevices)
	{
		alpacaDevice->setActive(false);
	}

	gPSOCTACSerialTableModel->refresh();

	int deviceCount = gPSOCTACSerialTableModel->rowCount(QModelIndex());
	for (auto deviceIndex : range(deviceCount))
	{
		AlpacaDevice alpacaDevice;

		QByteArray portName = gPSOCTACSerialTableModel->rowData(deviceIndex, kPortNameCol).toLatin1();
		HashType hash = ::arrayHash(portName);

		alpacaDevice = _AlpacaDevice::findAlpacaDevice(hash);
		if (alpacaDevice.isNull())
		{
			PSOCDevice* psocDevice = new PSOCDevice;

			psocDevice->_portName = portName;
			psocDevice->_hash = hash;

			psocDevice->_active = true;
			psocDevice->_boardType = ePSOC;
			psocDevice->_platformID = MICRO_EPM_BOARD_ID_UNKNOWN;
			psocDevice->_description = gPSOCTACSerialTableModel->rowData(deviceIndex, kDescriptionCol).toLatin1();
			psocDevice->_serialNumber = gPSOCTACSerialTableModel->rowData(deviceIndex, kSerialNumberCol).toLatin1();

			_AlpacaDevice::_alpacaDevices.push_back(AlpacaDevice(psocDevice));

			AppCore::writeToApplicationLog("Found a PSOC device with port name: '" + portName + "'\n");
		}
		else
		{
			alpacaDevice->setActive(true);
		}
	}

	return _AlpacaDevice::_alpacaDevices.count();
}

bool PSOCDevice::open()
{
	bool result{false};

	if (_driveThread == Q_NULLPTR)
	{
		_driveThread = new TACPSOCDriveThread(_hash);
		_driveThread->start();

		QThread::msleep(100); // force DriveTrain thread to start faster

		for (int i{0}; i < 60; i++)
		{
			result = _driveThread->isRunning();
			if (result == true)
				break;
			else
				QThread::msleep(300);
		}

		result = false;
		for (int i{0}; i < 10; i++)
		{
			PlatformID platformID = _driveThread->platformID();
			if (platformID == MICRO_EPM_BOARD_ID_UNKNOWN)
			{
				if (_driveThread->oldFirmware() == false)
					QThread::msleep(100);
				else
				{
					_lastError = "Firmware version is too old";
					break;
				}
			}
			else
			{
				_platformID = platformID;
				if (platformID != MICRO_EPM_BOARD_ID_UNKNOWN)
				{
					TACPlatformEntry platformEntry = _PlatformConfiguration::getEntry(_platformID);
					_description = platformEntry._platformEntry->_description.toLatin1();
					_platformConfiguration = platformEntry.getConfiguration();
					_psocPlatformConfiguration = static_cast<_PSOCPlatformConfiguration*>(_platformConfiguration.data());

					result = true;
					break;
				}
				else
				{
					result = false;
				}
			}
		}
	}

	if (result == true)
	{
		buildMapping();

		connect(_driveThread, &TACPSOCDriveThread::pinStateChanged, this, &PSOCDevice::on_pinStateChanged, Qt::DirectConnection);

		Pins initialPins;
		Pins pins = _platformConfiguration->getPins();
		for (const auto& pin: pins)
		{
			if (pin._initializationPriority > 0)
			{
				initialPins.append(pin);
			}
		}

		// Once priority pins are set, append all initial pins without priority
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
				_driveThread->setPinState(initializationPin._pin, true);
				if (AppCore::getAppCore()->appLoggingActive())
				{
					QString message = QString("PSOCDevice::open() Initialize Pin: %1\n").arg(initializationPin._pin);
					AppCore::writeToApplicationLog(message);
				}
			}
		}
	}

	_chipVersion = _driveThread->chipVersion();

	return result;
}

void PSOCDevice::buildMapping()
{
	if (_commands.empty() == true)
	{
		Pins pins = _platformConfiguration->getPins();
		for (const auto& pin: std::as_const(pins))
		{
			TACCommand tacCommand;

			tacCommand._pin = pin._pin;
			tacCommand._command = pin._pinCommand;
			tacCommand._helpText = pin._pinTooltip;
			tacCommand._tabName = pin._tabName.toLatin1();
			tacCommand._groupName = CommandGroup::toString(pin._commandGroup).toLatin1();

			QByteArray cellLocation = QString::number(pin._cellLocation.x()).toLatin1() + "," + QString::number(pin._cellLocation.y()).toLatin1();
			tacCommand._cellLocation = cellLocation;

			_commands[tacCommand._command] = tacCommand;
			_commandList.append(tacCommand);
		}
	}

	buildQuickSettings();
}
