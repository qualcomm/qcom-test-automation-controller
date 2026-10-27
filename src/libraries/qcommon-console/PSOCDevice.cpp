// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "PSOCDevice.h"
#include "private/TACPSOCDriveThread.h"

// QCommonConsole
#include "AppCore.h"
#include "PSOCPlatformConfiguration.h"
#include "Range.h"
#include "PSOCSerialTableModel.h"

// Qt
#include <QDebug>

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
	bool startedDriveThread{false};

	if (_driveThread == Q_NULLPTR)
	{
		startedDriveThread = true;
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
			if (platformID == MICRO_EPM_BOARD_ID_UNKNOWN && _driveThread->oldFirmware())
			{
				// Old firmware does not implement the board ID query, but earlier
				// releases still drove these boards. Identify the board from the
				// hardware name in its version reply and keep working, with a warning.
				platformID = _driveThread->platformIDFromHardwareVersion();
				if (platformID == MICRO_EPM_BOARD_ID_UNKNOWN)
				{
					_lastError = "Firmware version is too old and the board could not be identified";
					AppCore::writeToApplicationLogLine(QString("%1: %2").arg(QString::fromLatin1(_portName), QString::fromLatin1(_lastError)));
					break;
				}

				_driveThread->setPlatformID(platformID);

				// The firmware reply may already end with '.', avoid printing "1.4.9.0."
				QString firmwareVersion = _driveThread->firmwareVersion().trimmed();
				while (firmwareVersion.endsWith('.'))
					firmwareVersion.chop(1);

				const QString warning = QString("Warning: %1 firmware %2 is out of date and does not report its board ID; "
					"identified as '%3' from its hardware version. Please update the board firmware.")
					.arg(QString::fromLatin1(_portName), firmwareVersion, PlatformContainer::toString(platformID));
				AppCore::writeToApplicationLogLine(warning);
				qWarning().noquote() << warning;
			}

			if (platformID == MICRO_EPM_BOARD_ID_UNKNOWN)
			{
				QThread::msleep(100);
			}
			else
			{
				_platformID = platformID;
				if (platformID != MICRO_EPM_BOARD_ID_UNKNOWN)
				{
					TACPlatformEntry platformEntry = _PlatformConfiguration::getEntry(_platformID);

					// The board identified itself, but no configuration for its
					// platform was loaded, typically because the configuration
					// folder could not be found. Fail the open with a reason
					// rather than dereferencing the missing entry.
					if (platformEntry._platformEntry.isNull())
					{
						_lastError = QString("No configuration found for platform %1. "
											 "Check that the board configuration files are installed.")
										 .arg(_platformID).toLatin1();
						AppCore::writeToApplicationLogLine("PSOCDevice::open(): " + _lastError);
						result = false;
						break;
					}

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

		if (result == false && _lastError.isEmpty())
		{
			if (_driveThread->platformID() == MICRO_EPM_BOARD_ID_UNKNOWN)
			{
				// The board answered, but its stored board ID is 0 (not set). This happens,
				// for example, when a firmware update does not keep the board ID.
				_lastError = "The board did not report a board ID (board ID is not set). "
							 "Reprogram the board ID with the Device Catalog, then reconnect the board.";
			}
			else
			{
				_lastError = "The board could not be opened";
			}
			AppCore::writeToApplicationLogLine(QString("%1: %2").arg(QString::fromLatin1(_portName), QString::fromLatin1(_lastError)));
		}
	}

	if (result == false && startedDriveThread == true)
	{
		// Release the drive thread and serial port so a later open() can retry.
		_chipVersion = _driveThread->chipVersion();
		close();

		return result;
	}

	if (result == true)
	{
		// Legacy firmware support (see private/TACPSOCLegacyCommands.h). Must run before any pin is sent.
		TACPSOCDriveThread* psocDriveThread = dynamic_cast<TACPSOCDriveThread*>(_driveThread);
		if (psocDriveThread != Q_NULLPTR)
		{
			psocDriveThread->legacyCommands()->configure(_psocPlatformConfiguration,
				psocDriveThread->firmwareVersion(), psocDriveThread->majorVersion(),
				psocDriveThread->minorVersion(), _portName);
		}

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

	if (_driveThread != Q_NULLPTR)
	{
		_chipVersion = _driveThread->chipVersion();
	}

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
