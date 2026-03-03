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
// Copyright 2024 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "TACPIC32CXDriveThread.h"
#include "AlpacaDevice.h"

// QCommon
#include "AppCore.h"
#include "PIC32CXSerialTableModel.h"
#include "StringUtilities.h"
#include "TACPIC32CXCommand.h"
#include "TACCommandHashes.h"

// Qt
#include <QDir>
#include <QDateTime>
#include <QTextStream>

const QString kTACSerialDriveTrainName(QStringLiteral("TAC PIC32CX Drive Train"));


TACPIC32CXDriveThread::TACPIC32CXDriveThread(uint hash) :
	TACDriveThread(hash)
{
	_driveTrainName = kTACSerialDriveTrainName;
	setProtocolInterface(&_tacProtocol);
	_tacProtocol.setTACDriveTrain(this);

	AlpacaDevices alpacaDevices;

	AlpacaDevice alpacaDevice = _AlpacaDevice::findAlpacaDevice(hash);
	if (alpacaDevice->active())
	{
		_hardwareType = ePIC32CXAuto;

		_firmwareMajor = 1;
		_firmwareMinor = 1;
		_firmwareRevision = 1;
		_description = "PIC32CX (Automotive) Board";
		_uuid = "PIC32CX - No UUID";

		_portName = alpacaDevice->portName();
		_serialNumber = alpacaDevice->serialNumber();
		_description = alpacaDevice->description();

		connect(this, &TACDriveThread::pinStateChanged, alpacaDevice.data(), &_AlpacaDevice::on_pinStateChanged);
		connect(this, &TACDriveThread::progress, alpacaDevice.data(), &_AlpacaDevice::progress);
	}
}

TACPIC32CXDriveThread::~TACPIC32CXDriveThread()
{
}

bool TACPIC32CXDriveThread::openSerialDevice()
{
	bool result(false);

	PIC32CXSerialTableModel tableModel;

	_tacPortInfo = tableModel.getSerialPortFromName(_portName);

	if (_tacPortInfo.isNull())
		_tacPortInfo = tableModel.getSerialPortFromSerialNumber(_portName);

	if (_tacPortInfo.isNull() == false)
	{
		SerialPortSettings serialSettings;

		_serialPort = new SerialPort(_tacPortInfo);

		serialSettings = _serialPort->getSerialPortSettings();
		serialSettings._baudRate = QSerialPort::Baud115200;
		serialSettings._timeout = 500;

		_serialPort->setSerialPortSettings(serialSettings);

		if (_serialPort->open(QIODevice::ReadWrite) == true)
		{
			setSerialNumber(_tacPortInfo.serialNumber());
			setPortName(_tacPortInfo.portName().toLatin1());

			emit serialNumUpdate(_serialNumber);

			AppCore::writeToApplicationLogLine(QString("Device %1 opened").arg(_tacPortInfo.serialNumber()));
			AppCore::writeToApplicationLogLine(QString("Com port %1\n").arg(_tacPortInfo.portName()));

			{
				TACPIC32CXCommand tacCommand(this, this);
				tacCommand.clearBuffer();
			}

			result = true;
		}
		else
		{
			_lastErrorMessage = "Unable to open TAC Port. " + serialPortError().toLatin1();

			AppCore::writeToApplicationLogLine(_lastErrorMessage);

			emit errorOnOpen("PIC32CX Device Open Failed. Check the Application Log");

			delete _serialPort;
			_serialPort = Q_NULLPTR;
		}
	}

	return result;
}

void TACPIC32CXDriveThread::sendCommand
(
	const QByteArray& command,
	bool console, // = false
	ReceiveInterface* receiveInterface, // = Q_NULLPTR
	bool shouldStore // = true
)
{
	Arguments args;
	QByteArray decodedCommand = decodeCommand(command, args);

	send(decodedCommand, args, console, receiveInterface, shouldStore);
}

void TACPIC32CXDriveThread::setPinState(quint16 pin, bool state)
{
	{
		TACPIC32CXCommand tacCommand(this, this);

		tacCommand.setPinState(pin, state);
	}

	waitForCompletion();
}

void TACPIC32CXDriveThread::endTransaction
(
	ReceiveInterface* receiveInterface
)
{
	_tacProtocol.endTransaction(receiveInterface);
}

void TACPIC32CXDriveThread::sendCommandSequence(CommandEntries& commandEntries)
{
	for (const auto& commandEntry: commandEntries)
	{
        {
            TACPIC32CXCommand tacCommand(this, this);

            switch (commandEntry->_commandAction)
            {
            case _CommandEntry::eNotSet:
                break;

            case _CommandEntry::eSetPin:
                tacCommand.setPinState(commandEntry->_pinID, commandEntry->_arguement.toBool());
                break;

            case _CommandEntry::eLog:
                tacCommand.addLogComment(commandEntry->_arguement.toString().toLatin1() + "\n");
                break;

            case _CommandEntry::eDelay:
                tacCommand.addDelay(commandEntry->_arguement.toUInt());
                break;

            case _CommandEntry::eBaseCommand:
                break;
            }
        }

		emit progress(kProgressActive);
	}
}

int TACPIC32CXDriveThread::getResetCount()
{
	waitForCompletion();

	return _resetCount;
}

void TACPIC32CXDriveThread::clearResetCount()
{
	waitForCompletion();
}

QString TACPIC32CXDriveThread::serialPortError()
{
	QString result;

	if (_serialPort != Q_NULLPTR)
	{
		switch (_serialPort->error())
		{
		case QSerialPort::NoError:
			result = "No error occurred.";
			break;

		case QSerialPort::DeviceNotFoundError:
			result = "Device is non-existant.";
			break;

		case QSerialPort::PermissionError:
			result = "The device is already open in another process or the user does not have sufficient security credentials.";
			break;

		case QSerialPort::OpenError:
			result = "This object already has an open device..";
			break;

		case QSerialPort::NotOpenError:
			result = "This error occurs when an operation is executed that can only be successfully performed if the device is open.";
			break;

		case QSerialPort::WriteError:
			result = "An I/O error occurred while writing the data.";
			break;

		case QSerialPort::ReadError:
			result = "An I/O error occurred while reading the data.";
			break;

		case QSerialPort::ResourceError:
			result = "An I/O error occurred when a resource becomes unavailable, e.g. when the device is unexpectedly removed from the system.";
			break;

		case QSerialPort::UnsupportedOperationError:
			result = "The requested device operation is not supported or prohibited by the running operating system.";
			break;

		case QSerialPort::TimeoutError:
			result = "A timeout error occurred.";
			break;

		case QSerialPort::UnknownError:
		default:
			result = QString("An unidentified serial port error %1 has occured occurred.").arg(_serialPort->error());
			break;
		}
	}

	return result;
}

void TACPIC32CXDriveThread::handleError(QSerialPort::SerialPortError error)
{
	switch(error)
	{
	case QSerialPort::DeviceNotFoundError:
		_lastErrorMessage = "Device Not Found.";
		break;
	case QSerialPort::PermissionError:
		_lastErrorMessage = "Permission Error.";
		break;
	case QSerialPort::OpenError:
		_lastErrorMessage = "Open Error.";
		break;
	case QSerialPort::WriteError:
		_lastErrorMessage = "Write Error.";
		break;
	case QSerialPort::ReadError:
		_lastErrorMessage = "Read Error.";
		break;
	case QSerialPort::ResourceError:
		_lastErrorMessage = "Resource Error.";
		break;
	case QSerialPort::UnsupportedOperationError:
		_lastErrorMessage = "Unsupported Operation Error.";
		break;
	case QSerialPort::UnknownError:
		_lastErrorMessage = "Unknown Error.";
		break;
	case QSerialPort::TimeoutError:
		_lastErrorMessage = "Timeout Error.";
		return;
	case QSerialPort::NotOpenError:
		_lastErrorMessage = "Not Open Error.";
		break;
	case QSerialPort::NoError:
		break;

	default:
		break;
	}
}

void TACPIC32CXDriveThread::on_readyRead()
{
	_readyRead = true;
}

void TACPIC32CXDriveThread::setName(const QByteArray& newName)
{
	if (newName.length() <= 32)
	{
		if (newName != _name)
		{
			if (isAlphaNumeric(newName) == true)
			{
				waitForCompletion();
			}
		}
	}
}

quint32 TACPIC32CXDriveThread::send(const QByteArray& sendMe, const Arguments& arguments, bool console, ReceiveInterface* recieveInterface, bool store)
{
	return _tacProtocol.sendCommand(sendMe, arguments, console, recieveInterface, store);
}

bool TACPIC32CXDriveThread::ready()
{
	return _tacProtocol.queueSize() == 0;
}

bool TACPIC32CXDriveThread::readSerialData()
{
	bool result(false);

	if (_readyRead == true)
	{
		_readyRead = false;

		_serialBuffer.clear();

		QByteArray chunk;

		while (true)
		{
			chunk = _serialPort->readAll();
			if (chunk.isEmpty() == false)
				_serialBuffer.append(chunk);
			else
				break;
		}

		if (_serialBuffer.isEmpty() == false)
		{
#ifdef DEBUG
			AppCore::writeToApplicationLogLine(_serialBuffer);
#endif
			_protocolInterface->handleRecievedData(_serialBuffer);
			result = true;
		}
	}

	return result;
}

void TACPIC32CXDriveThread::receive(FramePackage& framePackage)
{
	if (framePackage->_endTransaction == true)
	{
		clearWaitForCompletion();
		emit transactionEnded();

		if (_protocolInterface->queueSize() == 0)
			emit progress(kProgressMax);
	}

	else
	{
		if (framePackage->_valid == true)
		{
			switch (framePackage->_requestHash)
			{
			case kPIC32CXVersionCommandHash: handleVersionResponse(framePackage); break;
			case kPIC32CXSetPinCommandHash: handleSetPin(framePackage); break;
			case kPIC32CXClearBufferHash: handleClearBuffer(framePackage); break;
			default:
				break;
			}
		}
		else
		{
			clearWaitForCompletion();

			switch (framePackage->_requestHash)
			{
				case kPIC32CXVersionCommandHash:
				{
					TACPIC32CXCommand tacCommand(this, this);
					tacCommand.platformID();
					break;
				}
				default:
					break;
			}
		}
	}

	log(framePackage);

	_protocolInterface->clearPendingFrame();
}

void TACPIC32CXDriveThread::i2CReadRegister(quint32 addr, quint32 reg)
{
	// do nothing
}

void TACPIC32CXDriveThread::i2CWriteRegister(quint32 addr, quint32 reg, quint32 data)
{
	// do nothing
}

void TACPIC32CXDriveThread::run()
{
	Q_ASSERT(_protocolInterface != Q_NULLPTR);

	AppCore::getAppCore()->setRunLogging(AppCore::getAppCore()->getPreferences()->loggingActive());

	if (openSerialDevice() == true)
	{
		emit deviceOpen();

		connect(_serialPort, &QSerialPort::errorOccurred, this, &TACPIC32CXDriveThread::handleError, Qt::DirectConnection);
		connect(_serialPort, &QSerialPort::readyRead, this, &TACPIC32CXDriveThread::on_readyRead, Qt::DirectConnection);

		startRunning();

		emit deviceStatusChange(tr("Starting"));

		_readyRead = false;
		_connected = false;

		{
			TACPIC32CXCommand tacCommand(this, this);
			tacCommand.platformID();
		}

		if (weAreRunning())
		{
			bool loopFinished{false};

			while (!loopFinished)
			{
				FramePackage framePackage = _protocolInterface->getNextFramePackage();
				if (framePackage.isNull() == false)
				{
					if (framePackage->_delayInMilliSeconds != 0 ||
						framePackage->_comment.isEmpty() == false ||
						framePackage->_endTransaction == true ||
						checkLocalStore(framePackage) == true)
						receive(framePackage);

					else
					{
#ifdef DEBUG
						AppCore::writeToApplicationLogLine("TACPIC32CXDriveTrain::run()::Write: " + framePackage->_codedRequest);
#endif

						// No funny stuff in serial buffer when executing this command
						if (framePackage->_requestHash == kPIC32CXVersionCommandHash)
						{
							bool status = _serialPort->clear(QSerialPort::AllDirections);

							if (status)
								AppCore::writeToApplicationLogLine("Buffer cleared before identifying PIC32CX board");
						}

						qint64 bytesWritten = _serialPort->write(framePackage->_codedRequest);
						if (bytesWritten == -1)
						{
							AppCore::writeToApplicationLogLine(QByteArray("TACPIC32CXDriveTrain::run()::bytesWritten == -1"));

							QString errorString = _serialPort->errorString();
							if (errorString.isEmpty() == false)
							{
								errorString = QString("Error on write ") + errorString;
								AppCore::writeToApplicationLogLine(errorString);
							}

							stopRunning();
						}
					}
				}
				else
				{
					if (weAreRunning() == false)
						loopFinished = true;
					else
						_serialPort->waitForReadyRead(10);
				}

				if (readSerialData() == true)
				{
					_protocolInterface->idle();
				}
			}
		}

		_serialPort->close();

		_connected = false;
	}
	else
	{
		emit errorOnOpen(lastErrorMessage());
	}

	shutdownLogging();
}

void TACPIC32CXDriveThread::handleSetPin(FramePackage& framePackage)
{
	QVariant pinVariant{framePackage->getArgument(1)};
	QVariant stateVariant{framePackage->getArgument(0)};

	if (pinVariant.isValid() && stateVariant.isValid())
	{
		QString pinString = QString("Pin %1 ").arg(pinVariant.toString());

		bool pinOn = stateVariant.toBool();
		if (pinOn)
			framePackage->_synonym = pinString.toLatin1() + "on";
		else
			framePackage->_synonym = pinString.toLatin1() + "off";

		emit pinStateChanged(static_cast<PinID>(pinVariant.toUInt()), pinOn);
	}
}

void TACPIC32CXDriveThread::handleVersionResponse(FramePackage& framePackage)
{
	if (framePackage->_responses.count() > 0)
	{
		bool okay{true};

		QStringList boardIdentity = QString(framePackage->_responses.at(0)).split(',', Qt::SkipEmptyParts);

		if (boardIdentity.size() < 7)
		{
			AppCore::writeToApplicationLogLine("Incomplete board response: " + boardIdentity.join(","));
		}
		else
		{
			_name = boardIdentity.at(0).toLatin1();
			AppCore::writeToApplicationLogLine(QString("PIC32CX board name: %1").arg(_name.data()));

			_firmwareString = boardIdentity.at(1).toLatin1();
			AppCore::writeToApplicationLogLine(QString("PIC32CX board firmware version: %1").arg(_firmwareString.data()));

			_macAddress = boardIdentity.at(3).toLatin1();
			AppCore::writeToApplicationLogLine(QString("PIC32CX board MAC: %1").arg(_macAddress.data()));

			int boardID = boardIdentity.at(4).toInt(&okay);
			if (okay)
				_platformID = static_cast<PlatformID>(boardID);
			else
				_platformID = ALPACA_PIC32CX_ID;

			AppCore::writeToApplicationLogLine(QString("Identified platform id: %1").arg(_platformID));

			_serialNumber = boardIdentity.at(5);
			AppCore::writeToApplicationLogLine(QString("PIC32CX board Serial Number: %1").arg(_serialNumber));

			_mcnNumber = boardIdentity.at(6);
			AppCore::writeToApplicationLogLine(QString("PIC32CX board MCN: %1").arg(_mcnNumber));
		}
	}

	emit platformIDUpdate(static_cast<int>(_platformID));

	setupConnected();
}

void TACPIC32CXDriveThread::handleClearBuffer(FramePackage &framePackage)
{
	if (framePackage->_responses.count() > 0)
	{
		QByteArrayList boardResponse = framePackage->_responses;

		AppCore::writeToApplicationLogLine("Buffer cleared. Board response: '" + boardResponse.join(",") + "'");
	}
}

void TACPIC32CXDriveThread::setupConnected()
{
	if (_connected == false)
	{
		emit hardwareVersionUpdate(hardwareVersionString());

		_connected = true;

		// okay, let send our initial requests
		AppCore::writeToApplicationLogLine("");

		TACPIC32CXCommand tacCommand(this, this);
		
		AppCore::writeToApplicationLogLine("tacCommand.platformID()");
		tacCommand.platformID();

		emit deviceConnected();
	}
}

void TACPIC32CXDriveThread::setupDiscovery()
{
	emit hardwareVersionUpdate(hardwareVersionString());

	{
		TACPIC32CXCommand tacCommand(this, this);
		tacCommand.platformID();
	}
}

void TACPIC32CXDriveThread::log(FramePackage& framePackage)
{
	QString logEntry;

	AppCore::writeToApplicationLogLine("");
	AppCore::writeToApplicationLogLine("Frame Package Start");

	AppCore::writeToApplicationLogLine(framePackage->_valid ? "Frame Valid" : "Frame Invalid");

	if (framePackage->_delayInMilliSeconds > 0)
	{
		logEntry = QString("Delay %1 in msecs").arg(framePackage->_delayInMilliSeconds);
		timeStampLogMessage(logEntry);
		AppCore::writeToApplicationLogLine(logEntry);
	}
	else if (framePackage->_comment.isEmpty() == false)
	{
		logEntry = QString("%1").arg(framePackage->_comment.data());
		timeStampLogMessage(logEntry);
		AppCore::writeToApplicationLogLine(logEntry);
	}
	else
	{
		if (framePackage->_console == true)
			logEntry = QString("Request from console: %1").arg(framePackage->_request.data());
		else
			logEntry = QString("Request: %1").arg(framePackage->_request.data());

		AppCore::writeToApplicationLogLine(logEntry);

		logEntry.clear();
		timeStampLogMessage(logEntry);
		AppCore::writeToApplicationLogLine(logEntry);

		if (framePackage->_synonym.isEmpty() == false)
		{
			logEntry = QString("Synonym: %1").arg(framePackage->_synonym.data());
			AppCore::writeToApplicationLogLine(logEntry);
		}

		AppCore::writeToApplicationLogLine("Responses");

		auto response = framePackage->_responses.begin();
		while (response != framePackage->_responses.end())
		{
			logEntry = QString("   %1").arg(response->data());
			AppCore::writeToApplicationLogLine(logEntry);
			response++;
		}

	}

	AppCore::writeToApplicationLogLine("Frame Package End");
}
