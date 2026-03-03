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
// Copyright 2024-2025 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

// Author:	Biswajit Roy <biswroy@qti.qualcomm.com>

#include "PIC32CXDevice.h"

#include "private/TACPIC32CXDriveThread.h"
#include "PIC32CXSerialTableModel.h"

#include "AppCore.h"
#include "Range.h"


const QByteArray kPIC32CXDescription{"ALPACA PIC32CX Debug Board"};
PIC32CXSerialTableModel* gPIC32CXTACSerialTableModel{Q_NULLPTR};


PIC32CXDevice::~PIC32CXDevice()
{
}

quint32 PIC32CXDevice::updateAlpacaDevices()
{
	if (gPIC32CXTACSerialTableModel == Q_NULLPTR)
		gPIC32CXTACSerialTableModel = new PIC32CXSerialTableModel;

	if (gPIC32CXTACSerialTableModel == Q_NULLPTR)
		return 0;

	gPIC32CXTACSerialTableModel->refresh();

	int deviceCount = gPIC32CXTACSerialTableModel->rowCount(QModelIndex());
	for (auto deviceIndex : range(deviceCount))
	{
		AlpacaDevice alpacaDevice;

		QByteArray portName = gPIC32CXTACSerialTableModel->rowData(deviceIndex, kPortNameCol).toLatin1();
		HashType hash = ::arrayHash(portName);

		alpacaDevice = _AlpacaDevice::findAlpacaDevice(hash);
		if (alpacaDevice.isNull())
		{
			PIC32CXDevice* pic32cxDevice = new PIC32CXDevice;

			pic32cxDevice->_portName = portName;
			pic32cxDevice->_hash = hash;

			pic32cxDevice->_active = true;
			pic32cxDevice->_boardType = ePIC32CXAuto;
			pic32cxDevice->_platformID = ALPACA_PIC32CX_ID;
			pic32cxDevice->_description = kPIC32CXDescription;
			pic32cxDevice->_serialNumber = gPIC32CXTACSerialTableModel->rowData(deviceIndex, kSerialNumberCol).toLatin1();

			// Mimic part of serial number as USB descriptor for platform identification
			QStringList serialNumberList = gPIC32CXTACSerialTableModel->rowData(deviceIndex, kSerialNumberCol).split("XX");

			if (serialNumberList.size() > 1)
			{
				pic32cxDevice->_usbDescriptor = serialNumberList.at(0).toLatin1();

				if (_PlatformConfiguration::containsUSBDescriptor(pic32cxDevice->_usbDescriptor) == true)
					pic32cxDevice->_platformID = _PlatformConfiguration::getUSBDescriptor(pic32cxDevice->_usbDescriptor);
			}

			TACPlatformEntry platformEntry = _PlatformConfiguration::getEntry(pic32cxDevice->_platformID);

			pic32cxDevice->_description = platformEntry._platformEntry->_description.toLatin1();
			pic32cxDevice->_platformConfiguration = platformEntry.getConfiguration();

			_AlpacaDevice::_alpacaDevices.push_back(AlpacaDevice(pic32cxDevice));

			AppCore::writeToApplicationLog("Found a PIC32CX device with port name: '" + portName + "'\n");
		}
		else
		{
			alpacaDevice->setActive();
		}
	}

	return _AlpacaDevice::_alpacaDevices.count();
}

bool PIC32CXDevice::open()
{
	bool result{false};

	if (_driveThread == Q_NULLPTR)
	{
		_driveThread = new TACPIC32CXDriveThread(_hash);
		_driveThread->setThreadDelay(300);
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
	}

	if (result == true)
	{
		_pic32cxPlatformConfiguration = static_cast<_PIC32CXPlatformConfiguration*>(_platformConfiguration.data());

		buildMapping();

		connect(_driveThread, &TACPIC32CXDriveThread::pinStateChanged, this, &PIC32CXDevice::on_pinStateChanged, Qt::DirectConnection);
	}

	_macAddress = _driveThread->macAddress();
	_chipVersion = _driveThread->chipVersion();
	_driveThread->setThreadDelay(0);

	return result;
}

void PIC32CXDevice::buildMapping()
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
