// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "PIC32CXDevice.h"

#include "private/TACPIC32CXDriveThread.h"
#include "PIC32CXSerialTableModel.h"

#include "AppCore.h"
#include "Range.h"

const std::string kPIC32CXDescription{"ALPACA PIC32CX Debug Board"};
PIC32CXSerialTableModel* gPIC32CXTACSerialTableModel{Q_NULLPTR};

PIC32CXDevice::~PIC32CXDevice()
{
}

uint32_t PIC32CXDevice::updateAlpacaDevices()
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

		std::string portName = gPIC32CXTACSerialTableModel->rowData(deviceIndex, kPortNameCol).toLatin1().toStdString();
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

			QString serialNumber = gPIC32CXTACSerialTableModel->rowData(deviceIndex, kSerialNumberCol);
			pic32cxDevice->_serialNumber = serialNumber.toLatin1().toStdString();

			QStringList serialNumberList = serialNumber.split("XX");

			if (serialNumberList.size() > 1)
			{
				pic32cxDevice->_usbDescriptor = serialNumberList.at(0).toLatin1().toStdString();

				QByteArray usbDescriptorBytes = QByteArray::fromStdString(pic32cxDevice->_usbDescriptor);
				if (_PlatformConfiguration::containsUSBDescriptor(usbDescriptorBytes) == true)
					pic32cxDevice->_platformID = _PlatformConfiguration::getUSBDescriptor(usbDescriptorBytes);
			}

			TACPlatformEntry platformEntry = _PlatformConfiguration::getEntry(pic32cxDevice->_platformID);

			pic32cxDevice->_description = platformEntry._platformEntry->_description;
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
			tacCommand._tabName = pin._tabName;
			tacCommand._groupName = CommandGroup::toString(pin._commandGroup);

			tacCommand._cellLocation = fromQPoint(pin._cellLocation);

			_commands[tacCommand._command] = tacCommand;
			_commandList.push_back(tacCommand);
		}
	}

	buildQuickSettings();
}
