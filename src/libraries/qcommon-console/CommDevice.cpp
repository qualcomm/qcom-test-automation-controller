// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "CommDevice.h"

// QCommon
#include "AlpacaSettings.h"
#include "Serial/SerialPreferences.h"
#include "StringUtilities.h"

// Qt
#include <QDebug>

#include <filesystem>
#include <string>

CommDevice::CommDevice() :
	QObject(nullptr)
{

}

CommDevice::~CommDevice()
{
	Close();
}

bool CommDevice::IsOpen()
{
	return _serialPort && _serialPort->isOpen();
}

bool CommDevice::Open()
{
	bool result(false);
	AlpacaSettings settings;

	if (_serialPort == nullptr)
	{
		_serialPort = ISerialPort::create();

		std::string name = _portName;
		if (name.empty() && !_serialPortInfo.isNull())
			name = _serialPortInfo.portName().toStdString();

		_serialPort->setPortName(name);

		SerialPortSettings portSettings;
		portSettings._baudRate = static_cast<int32_t>(SerialBaudRate::Baud115200);
		portSettings._dataBits = SerialDataBits::Data8;
		portSettings._parity = SerialParity::NoParity;
		portSettings._stopBits = SerialStopBits::OneStop;
		portSettings._flowControl = SerialFlowControl::NoFlowControl;
		_serialPort->setSerialPortSettings(portSettings);

		settings.beginGroup(kComSettingsGroup);
		_logSerialData = settings.value(kDumpComPortData, false).toBool();

		if (_logSerialData == true)
		{
			std::string path = settings.value(kDumpComPortPath, "").toString();
			if (path.empty())
				_logSerialData = false;
			else
			{
				std::error_code ec;
				std::filesystem::create_directories(path, ec);

				path = (std::filesystem::path(path) / ("Serial_" + formatDateTime("%m%d_%H%M%S") + ".ssn")).string();
				_logSerialPath = path;

				_logFile.open(path, std::ios::out | std::ios::binary);

				if (!_logFile.is_open())
				{
					qDebug() << "Not able to open serial log file " << path.c_str() << " turning logging off";
					_logSerialData = false;
				}
			}
		}

		result = _serialPort->open();
	}
	else
	{
		result = _serialPort->open();
	}

	return result;
}

void CommDevice::Close()
{
	if (_logFile.is_open())
		_logFile.close();

	if (_serialPort != nullptr)
	{
		if (_serialPort->isOpen())
		{
			_serialPort->close();
		}

		_serialPort.reset();
	}
}

bool CommDevice::Send
(
	const std::string& sendData
)
{
	bool result(false);

	if (_serialPort != nullptr && _serialPort->isOpen())
	{
		int64_t bytesSent = _serialPort->write(sendData);
		result = (bytesSent == static_cast<int64_t>(sendData.size()));
	}

	return result;
}

uint64_t CommDevice::Read
(
	std::string& incomingBytes
)
{
	uint64_t bytesAvailable(0);

	const uint64_t kMaxToRead(0xFFFFF);

	if (_serialPort != nullptr && _serialPort->isOpen())
	{
		int64_t avail = _serialPort->bytesAvailable();
		if (avail > 0)
		{
			bytesAvailable = static_cast<uint64_t>(avail) > kMaxToRead ? kMaxToRead : static_cast<uint64_t>(avail);

			std::string readData = _serialPort->read(static_cast<size_t>(bytesAvailable));

			incomingBytes.append(readData);

			if (_logSerialData == true && _logFile.is_open())
				_logFile.write(readData.data(), readData.size());
		}
	}

	return bytesAvailable;
}

void CommDevice::on_ReadyRead()
{
}
