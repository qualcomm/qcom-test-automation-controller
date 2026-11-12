// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "PIC32CXDevice.h"

#include "private/TACPIC32CXDriveThread.h"
#include "PIC32CXSerialTableModel.h"

#include "AppCore.h"
#include "Range.h"

#include <string>

const std::string kPIC32CXDescription{"ALPACA PIC32CX Debug Board"};
PIC32CXSerialTableModel* gPIC32CXTACSerialTableModel{nullptr};

PIC32CXDevice::~PIC32CXDevice()
{
}

uint32_t PIC32CXDevice::updateAlpacaDevices()
{
	if (gPIC32CXTACSerialTableModel == nullptr)
		gPIC32CXTACSerialTableModel = new PIC32CXSerialTableModel;

	if (gPIC32CXTACSerialTableModel == nullptr)
		return 0;

	gPIC32CXTACSerialTableModel->refresh();

	int deviceCount = gPIC32CXTACSerialTableModel->rowCount(QModelIndex());
	for (auto deviceIndex : range(deviceCount))
	{
		AlpacaDevice alpacaDevice;

		std::string portName = gPIC32CXTACSerialTableModel->rowData(deviceIndex, kPortNameCol);
		HashType hash = ::arrayHash(portName);

		alpacaDevice = _AlpacaDevice::findAlpacaDevice(hash);
		if (alpacaDevice == nullptr)
		{
			PIC32CXDevice* pic32cxDevice = new PIC32CXDevice;

			pic32cxDevice->_portName = portName;
			pic32cxDevice->_hash = hash;

			pic32cxDevice->_active = true;
			pic32cxDevice->_boardType = ePIC32CXAuto;
			pic32cxDevice->_platformID = ALPACA_PIC32CX_ID;
			pic32cxDevice->_description = kPIC32CXDescription;

			std::string serialNumber = gPIC32CXTACSerialTableModel->rowData(deviceIndex, kSerialNumberCol);
			pic32cxDevice->_serialNumber = serialNumber;

			size_t xxPos = serialNumber.find("XX");
			if (xxPos != std::string::npos)
			{
				pic32cxDevice->_usbDescriptor = serialNumber.substr(0, xxPos);

				if (_PlatformConfiguration::containsUSBDescriptor(pic32cxDevice->_usbDescriptor) == true)
					pic32cxDevice->_platformID = _PlatformConfiguration::getUSBDescriptor(pic32cxDevice->_usbDescriptor);
			}

			TACPlatformEntry platformEntry = _PlatformConfiguration::getEntry(pic32cxDevice->_platformID);

			pic32cxDevice->_description = platformEntry._platformEntry->_description;
			pic32cxDevice->_platformConfiguration = platformEntry.getConfiguration();

			_AlpacaDevice::_alpacaDevices.push_back(std::shared_ptr<PIC32CXDevice>(pic32cxDevice));

			AppCore::writeToApplicationLog("Found a PIC32CX device with port name: '" + portName + "'\n");
		}
		else
		{
			alpacaDevice->setActive();
		}
	}

	return static_cast<uint32_t>(_AlpacaDevice::_alpacaDevices.size());
}

bool PIC32CXDevice::open()
{
	bool result{false};

	if (_driveThread == nullptr)
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
		_pic32cxPlatformConfiguration = static_cast<_PIC32CXPlatformConfiguration*>(_platformConfiguration.get());

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
			tacCommand._tabName = pin._tabName;
			tacCommand._groupName = CommandGroup::toString(pin._commandGroup);

			tacCommand._cellLocation = pin._cellLocation;

			_commands[tacCommand._command] = tacCommand;
			_commandList.push_back(tacCommand);
		}
	}

	buildQuickSettings();
}
