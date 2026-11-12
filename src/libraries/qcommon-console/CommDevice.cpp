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
	QObject(nullptr),
	_serialPort(nullptr)
{

}

CommDevice::~CommDevice()
{
	Close();
}

bool CommDevice::IsOpen()
{
	if (_serialPort != nullptr)
		return _serialPort->isOpen();

	return false;
}

bool CommDevice::Open()
{
	bool result(false);
	AlpacaSettings settings;

	if (_serialPort == nullptr)
	{
		_serialPort = new QSerialPort(this);

		_serialPort->setPort(_serialPortInfo);
		_serialPort->setBaudRate(QSerialPort::Baud115200, QSerialPort::AllDirections);
		_serialPort->setDataBits(QSerialPort::Data8);
		_serialPort->setParity(QSerialPort::NoParity);
		_serialPort->setStopBits(QSerialPort::OneStop);
		_serialPort->setFlowControl(QSerialPort::NoFlowControl);

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

				_logFile.setFileName(QString::fromStdString(path));

				if (_logFile.open(QIODevice::WriteOnly) == false)
				{
					qDebug() << "Not able to open serial log file " << path.c_str() << " turning logging off";
					_logSerialData = false;
				}
			}
		}

		result = _serialPort->open(QIODevice::ReadWrite);

		if (result == true)
		{
			connect(_serialPort, SIGNAL(readyRead()), this, SLOT(on_ReadyRead()));

			connect(_serialPort, SIGNAL(parityChanged(QSerialPort::Parity)), this, SLOT(on_parityChanged(QSerialPort::Parity)));
			connect(_serialPort, SIGNAL(baudRateChanged(qint32, QSerialPort::Directions)), this, SLOT(on_baudRateChanged(qint32, QSerialPort::Directions)));
			connect(_serialPort, SIGNAL(dataBitsChanged(QSerialPort::DataBits)), this, SLOT(on_dataBitsChanged(QSerialPort::DataBits)));
			connect(_serialPort, SIGNAL(stopBitsChanged(QSerialPort::StopBits)), this, SLOT(on_stopBitsChanged(QSerialPort::StopBits)));
			connect(_serialPort, SIGNAL(dataErrorPolicyChanged(QSerialPort::DataErrorPolicy)), this, SLOT(on_dataErrorPolicyChanged(QSerialPort::DataErrorPolicy)));

			connect(_serialPort, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(on_error(QSerialPort::SerialPortError)));
			connect(_serialPort, SIGNAL(dataTerminalReadyChanged(bool)), this, SLOT(on_dataTerminalReadyChanged(bool)));
			connect(_serialPort, SIGNAL(flowControlChanged(QSerialPort::FlowControl)), this, SLOT(on_flowControlChanged(QSerialPort::FlowControl)));
			connect(_serialPort, SIGNAL(requestToSendChanged(bool)), this, SLOT(on_requestToSendChanged(bool)));
		}
	}
	else
	{

	}

	return result;
}

void CommDevice::Close()
{
	if (_logFile.isOpen())
		_logFile.close();

	if (_serialPort != nullptr)
	{
		if (_serialPort->isOpen())
		{
			_serialPort->close();

			disconnect(_serialPort, 0, 0, 0);
		}

		delete _serialPort;
		_serialPort = nullptr;
	}
}

bool CommDevice::Send
(
	const std::string& sendData
)
{
	(void)sendData;

	bool result(false);

	if (_serialPort != nullptr && _serialPort->isOpen())
	{
		uint64_t bytesSent = _serialPort->write(sendData.data(), sendData.size());
		result = (bytesSent == sendData.size());
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

	if (_serialPort->isReadable())
	{
		bytesAvailable = _serialPort->bytesAvailable();
		bytesAvailable = bytesAvailable > kMaxToRead ? kMaxToRead : bytesAvailable;

		QByteArray readData;

		readData = _serialPort->read(bytesAvailable);

		incomingBytes.append(readData.constData(), readData.size()); // Don't presume the buffer is empty

		if (_logSerialData == true && _logFile.isOpen())
			_logFile.write(readData);
	}

	return bytesAvailable;
}

void CommDevice::on_error
(
	QSerialPort::SerialPortError serialPortError
)
{
	qDebug() << "on_error: " << serialPortError;
}

void CommDevice::on_dataTerminalReadyChanged(bool set)
{
	qDebug() << "on_dataTerminalReadyChanged: " << set;
}

void CommDevice::on_flowControlChanged(QSerialPort::FlowControl flow)
{
	qDebug() << "on_flowControlChanged: " << flow;
}

void CommDevice::on_parityChanged(QSerialPort::Parity parity)
{
	qDebug() << "on_parityChanged: " << parity;
}

void CommDevice::on_requestToSendChanged(bool set)
{
	qDebug() << "on_dataTerminalReadyChanged: " << set;
}

void CommDevice::on_baudRateChanged(qint32 baudRate, QSerialPort::Directions directions)
{
	qDebug() << "on_baudRateChanged: " << baudRate << ":" << directions;
}

void CommDevice::on_dataBitsChanged(QSerialPort::DataBits dataBits)
{
	qDebug() << "on_dataBitsChanged: " << dataBits;
}

void CommDevice::on_stopBitsChanged(QSerialPort::StopBits stopBits)
{
	qDebug() << "on_stopBitsChanged: " << stopBits;
}

void CommDevice::on_dataErrorPolicyChanged(QSerialPort::DataErrorPolicy policy)
{
	qDebug() << "on_dataErrorPolicyChanged: " << policy;
}
