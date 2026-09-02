// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "STM32Device.h"

#include "private/TACSTM32DriveThread.h"

// QCommonConsole
#include "AppCore.h"
#include "STM32PlatformConfiguration.h"

// Qt
#include <QSettings>
#include <QThread>

// hidapi
#include <hidapi.h>

const quint16 kSTM32VendorID{0x2341};
const quint16 kSTM32ProductID{0xB001};
const quint16 kSTM32UsagePage{0xFF60};
const quint16 kSTM32Usage{0x0061};

static bool hidapiInitialized{false};


bool isSTMInterface(const hid_device_info& info)
{
	return info.usage_page == kSTM32UsagePage && info.usage == kSTM32Usage;
}

STM32Device::~STM32Device()
{

}

bool STM32Device::programDevice(AlpacaDevice alpacaDevice, PlatformID platformID, QByteArray& errorMessage)
{
	Q_UNUSED(alpacaDevice)
	Q_UNUSED(platformID)

	errorMessage = "STM32 device does not support firmware programming";

	return false;
}

STM32HIDDeviceInfoList STM32Device::enumerateHIDDevices()
{
	if (hidapiInitialized == false)
	{
		hid_init();
		hidapiInitialized = true;
	}

	STM32HIDDeviceInfoList result;

	struct hid_device_info* devices = hid_enumerate(kSTM32VendorID, kSTM32ProductID);

	for (struct hid_device_info* current = devices; current != Q_NULLPTR; current = current->next)
	{
		if (isSTMInterface(*current) == false)
			continue;

		STM32HIDDeviceInfo deviceInfo;

		QByteArray devicePath = QByteArray(current->path);
		QString serialNumber = QString::fromWCharArray(current->serial_number);

		if (devicePath.isEmpty() == false && serialNumber.isEmpty() == false)
		{
			deviceInfo._devicePath = devicePath;
			deviceInfo._serialNumber = serialNumber.toLatin1();

			QByteArray hashSource = deviceInfo._serialNumber.isEmpty() == false ? deviceInfo._serialNumber : deviceInfo._devicePath;
			deviceInfo._hash = ::arrayHash(hashSource);

			result.append(deviceInfo);
		}
	}

	hid_free_enumeration(devices);

	return result;
}

QByteArray STM32Device::vtpPortName
(
	const QByteArray& serialNumber
)
{
	QByteArray result;

	if (serialNumber.isEmpty() == false)
	{
		QSettings settings(QSettings::UserScope, "Qualcomm, Inc.", "Alpaca");

		settings.beginGroup("VTPs");
		uint nextPort = settings.value("NextPortNumber", 1).toUInt();

		QString vtpName = settings.value(serialNumber, QString()).toString();
		if (vtpName.isEmpty())
		{
			vtpName = QString("VTP%1").arg(nextPort);
			settings.setValue(serialNumber, vtpName);

			nextPort++;

			settings.setValue("NextPortNumber", nextPort);
		}

		settings.endGroup();

		result = vtpName.toLatin1();
	}

	return result;
}

quint32 STM32Device::updateAlpacaDevices()
{
	STM32HIDDeviceInfoList deviceInfoList = enumerateHIDDevices();

	for (const auto& deviceInfo: std::as_const(deviceInfoList))
	{
		AlpacaDevice alpacaDevice = _AlpacaDevice::findAlpacaDevice(deviceInfo._hash);
		if (alpacaDevice.isNull())
		{
			STM32Device* stm32Device = new STM32Device;

			stm32Device->_active = true;
			stm32Device->_portName = vtpPortName(deviceInfo._serialNumber);
			stm32Device->_hash = deviceInfo._hash;
			stm32Device->_chipVersion = 2;
			stm32Device->_serialNumber = deviceInfo._serialNumber;
			stm32Device->_platformID = ALPACA_STM32_ID;
			stm32Device->_devicePath = deviceInfo._devicePath;

			TACPlatformEntry platformEntry = _PlatformConfiguration::getEntry(stm32Device->_platformID);
			stm32Device->_description = platformEntry._platformEntry->_description.toLatin1();
			stm32Device->_boardType = platformEntry._platformEntry->_boardtype;

			stm32Device->_platformConfiguration = platformEntry.getConfiguration();

			alpacaDevice = AlpacaDevice(stm32Device);
			_AlpacaDevice::_alpacaDevices.push_back(alpacaDevice);

			AppCore::writeToApplicationLog("Found a STM32 device with path: '" + stm32Device->portName() + "' and USB descriptor string: '" + stm32Device->usbDescriptor() + "'\n");
		}
		else
		{
			alpacaDevice->setActive();
		}
	}

	return _AlpacaDevice::_alpacaDevices.count();
}

bool STM32Device::open()
{
	const int maxInterations{50};

	bool result{false};

	if (_platformConfiguration.isNull() == false)
	{
		_stm32PlatformConfiguration = static_cast<_STM32PlatformConfiguration*>(_platformConfiguration.data());

		buildMapping();

		if (_driveThread == Q_NULLPTR)
		{
			TACSTM32DriveThread* driveThread = new TACSTM32DriveThread(_hash);

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
				connect(driveThread, &TACSTM32DriveThread::pinStateChanged, this, &STM32Device::on_pinStateChanged, Qt::DirectConnection);
		}
	}

	return result;
}

void STM32Device::buildMapping()
{
	Q_ASSERT(_stm32PlatformConfiguration != Q_NULLPTR);

	buildCommandList();
	buildQuickSettings();
}

void STM32Device::setPinState(PinID pin, bool state)
{
	bool physicalState = state;

	if (_stm32PlatformConfiguration != Q_NULLPTR && _stm32PlatformConfiguration->getPinInvertedState(pin))
		physicalState = !state;

	_AlpacaDevice::setPinState(pin, physicalState);
}

void STM32Device::buildCommandList()
{
	if (_commands.isEmpty() == true)
	{
		if (_stm32PlatformConfiguration != Q_NULLPTR)
		{
			STM32PinList stm32PinList = _stm32PlatformConfiguration->getActivePins();

			for (const auto& stm32Pin: std::as_const(stm32PinList))
			{
				TACCommand tacCommand;

				tacCommand._pin = stm32Pin._pin;
				tacCommand._command = stm32Pin._pinCommand;
				tacCommand._helpText = stm32Pin._pinTooltip;
				tacCommand._isInverted = stm32Pin._inverted;
				tacCommand._tabName = stm32Pin._tabName.toLatin1();
				tacCommand._groupName = CommandGroup::toString(stm32Pin._commandGroup).toLatin1();

				QByteArray cellLocation = QString::number(stm32Pin._cellLocation.x()).toLatin1() + "," + QString::number(stm32Pin._cellLocation.y()).toLatin1();
				tacCommand._cellLocation = cellLocation;

				_commands[tacCommand._command] = tacCommand;
			}
		}
	}
}

bool STM32Device::openTransport()
{
	bool result{false};

	if (_devicePath.isEmpty() == false)
	{
		if (_hidHandle == Q_NULLPTR)
		{
			_hidHandle = hid_open_path(_devicePath.constData());

			if (_hidHandle != Q_NULLPTR)
			{
				AppCore::writeToApplicationLogLine(QString("Opened Arduino BugHopper V2 device %1\n").arg(_devicePath.data()));

				result = true;
			}
			else
			{
				AppCore::writeToApplicationLogLine("STM32Device::openTransport() hid_open_path() failed");
			}
		}
		else
		{
			result = true;
		}
	}
	else
	{
		AppCore::writeToApplicationLogLine("STM32Device::openTransport() _devicePath is empty");
	}

	return result;
}

bool STM32Device::write
(
	const QByteArray& report
)
{
	bool result{false};

	if (_hidHandle != Q_NULLPTR)
	{
		result = hid_write(static_cast<hid_device*>(_hidHandle), reinterpret_cast<const unsigned char*>(report.constData()), report.size()) >= 0;
	}

	return result;
}

void STM32Device::close()
{
	if (_hidHandle != Q_NULLPTR)
	{
		hid_close(static_cast<hid_device*>(_hidHandle));
		_hidHandle = Q_NULLPTR;
	}
}
