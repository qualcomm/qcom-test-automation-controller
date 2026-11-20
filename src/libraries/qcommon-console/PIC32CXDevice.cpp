// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

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

			// Without a loaded configuration for this platform the entry is
			// empty. List the device anyway, but leave its configuration unset
			// so a later open fails cleanly instead of crashing here.
			if (platformEntry._platformEntry.isNull() == false)
			{
				pic32cxDevice->_description = platformEntry._platformEntry->_description.toLatin1();
				pic32cxDevice->_platformConfiguration = platformEntry.getConfiguration();
			}
			else
			{
				AppCore::writeToApplicationLogLine(QString("PIC32CXDevice: no configuration found for platform %1").arg(pic32cxDevice->_platformID));
			}

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

	// No configuration was loaded for this board when it was enumerated, so
	// its pins and commands are unknown. Report that instead of crashing in
	// buildMapping().
	if (_platformConfiguration.isNull())
	{
		_lastError = QString("No configuration found for platform %1. "
							 "Check that the board configuration files are installed.")
						 .arg(_platformID).toLatin1();
		AppCore::writeToApplicationLogLine("PIC32CXDevice::open(): " + _lastError);
		return false;
	}

	bool startedDriveThread{false};

	if (_driveThread == Q_NULLPTR)
	{
		startedDriveThread = true;
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

		TACPIC32CXDriveThread* pic32cxDriveThread = dynamic_cast<TACPIC32CXDriveThread*>(_driveThread);
		result = false;

		for (int i{0}; i < 50; i++)
		{
			if (pic32cxDriveThread != Q_NULLPTR && pic32cxDriveThread->isConnected())
			{
				result = true;
				break;
			}

			if (_driveThread->isRunning() == false && _driveThread->weAreRunning() == false)
				break;

			QThread::msleep(100);
		}

		if (result == false && _lastError.isEmpty())
		{
			const QByteArray portError = _driveThread->lastErrorMessage();
			if (portError.isEmpty() == false)
				_lastError = portError;
			else
				_lastError = "The board did not respond to identification request";

			AppCore::writeToApplicationLogLine(QString("%1: %2").arg(QString::fromLatin1(_portName), QString::fromLatin1(_lastError)));
		}
	}
	else
	{
		result = _driveThread->weAreRunning();
	}

	if (result == false && startedDriveThread == true)
	{
		_chipVersion = _driveThread->chipVersion();
		close();
		return false;
	}

	if (result == true)
	{
		_pic32cxPlatformConfiguration = static_cast<_PIC32CXPlatformConfiguration*>(_platformConfiguration.data());

		buildMapping();

		connect(_driveThread, &TACPIC32CXDriveThread::pinStateChanged, this, &PIC32CXDevice::on_pinStateChanged, Qt::DirectConnection);
	}

	if (_driveThread != Q_NULLPTR)
	{
		_macAddress = _driveThread->macAddress();
		_chipVersion = _driveThread->chipVersion();
		_driveThread->setThreadDelay(0);
	}

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
