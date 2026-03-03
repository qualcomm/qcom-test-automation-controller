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
// Copyright 2013-2018 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "TACPSOCDriveThread.h"

#include "AlpacaDevice.h"
#include "TACCommands.h"
#include "TACCommandHashes.h"
#include "TACPSOCCommand.h"

// QCommon
#include "AppCore.h"
#include "PSOCSerialTableModel.h"
#include "StringUtilities.h"

// Qt
#include <QDir>
#include <QDateTime>
#include <QTextStream>

#define on true
#define off false

const QString kTACSerialDriveTrainName(QStringLiteral("TAC Serial Drive Train"));

TACPSOCDriveThread::TACPSOCDriveThread
(
	uint hash
) :
	TACDriveThread(hash)
{
	_driveTrainName = kTACSerialDriveTrainName;
	setProtocolInterface(&_tacProtocol);
	_tacProtocol.setTACDriveTrain(this);

	AlpacaDevices alpacaDevices;

	AlpacaDevice alpacaDevice = _AlpacaDevice::findAlpacaDevice(hash);
	if (alpacaDevice->active())
	{
		_portName = alpacaDevice->portName();
		_serialNumber = alpacaDevice->serialNumber();
		_description = alpacaDevice->description();

		connect(this, &TACDriveThread::pinStateChanged, alpacaDevice.data(), &_AlpacaDevice::on_pinStateChanged);
		connect(this, &TACDriveThread::progress, alpacaDevice.data(), &_AlpacaDevice::progress);
	}
}

TACPSOCDriveThread::~TACPSOCDriveThread()
{
}

bool TACPSOCDriveThread::openSerialDevice()
{
	bool result(false);

	PSOCSerialTableModel tableModel;

	_tacPortInfo = tableModel.getSerialPortFromName(_portName);

	if (_tacPortInfo.isNull())
		_tacPortInfo = tableModel.getSerialPortFromSerialNumber(_portName);

	if (_tacPortInfo.isNull() == false)
	{
		SerialPortSettings serialSettings;

		_serialPort = new SerialPort(_tacPortInfo);

		serialSettings = _serialPort->getSerialPortSettings();
		serialSettings._baudRate = QSerialPort::Baud115200;
		_serialPort->setSerialPortSettings(serialSettings);

		if (_serialPort->open(QIODevice::ReadWrite) == true)
		{		
			setSerialNumber(_tacPortInfo.serialNumber());
			setPortName(_tacPortInfo.portName().toLatin1());

			emit serialNumUpdate(_serialNumber);

			AppCore::writeToApplicationLogLine(QString("Device %1 opened").arg(_tacPortInfo.serialNumber()));
			AppCore::writeToApplicationLogLine(QString("Com port %1\n").arg(_tacPortInfo.portName()));

			result = true;
		}
		else
		{
			_lastErrorMessage = "Unable to open TAC Port. " + serialPortError().toLatin1();

			AppCore::writeToApplicationLogLine(_lastErrorMessage);

			emit errorOnOpen("PSOC Device Open Failed, Check the Application Log");

			delete _serialPort;
			_serialPort = Q_NULLPTR;
		}
	}

	return result;
}

void TACPSOCDriveThread::sendCommand
(
	const QByteArray& command,
	bool console, // = false
	ReceiveInterface* receiveInterface, // = Q_NULLPTR
	bool shouldStore // = true
)
{
	if (command.startsWith(kHelpCommand) == false)
	{
		Arguments args;
		QByteArray decodedCommand = decodeCommand(command, args);

		send(decodedCommand, args, console, receiveInterface, shouldStore);
	}
}

void TACPSOCDriveThread::endTransaction
(
	ReceiveInterface* receiveInterface
)
{
	_tacProtocol.endTransaction(receiveInterface);
}

void TACPSOCDriveThread::setPinState
(
	quint16 pin,
	bool state
)
{
	{
		TACPSOCCommand tacCommand(this, this);

		tacCommand.setPinState(pin, state);
	}

	waitForCompletion();
}

void TACPSOCDriveThread::sendCommandSequence
(
	CommandEntries& commandEntries
)
{
	TACPSOCCommand tacCommand(this, this);

	for (const auto& commandEntry: commandEntries)
	{
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

		emit progress(kProgressActive);
	}
}

int TACPSOCDriveThread::getResetCount()
{
	{
		TACPSOCCommand tacCommand(this, this);

		tacCommand.getResetCount();
	}

	waitForCompletion();

	return _resetCount;
}

void TACPSOCDriveThread::clearResetCount()
{
	{
		TACPSOCCommand tacCommand(this, this);

		tacCommand.clearResetCount();
	}

	waitForCompletion();
}

void TACPSOCDriveThread::i2CReadRegister
(
	quint32 addr,
	quint32 reg
)
{
	{
		TACPSOCCommand tacCommand(this, this);

		try
		{
			tacCommand.i2CReadRegister(addr, reg);
		}
		catch (...)
		{

		}
	}

	waitForCompletion();
}

void TACPSOCDriveThread::i2CWriteRegister
(
	quint32 addr,
	quint32 reg,
	quint32 data
)
{
	{
		TACPSOCCommand tacCommand(this, this);

		try
		{
			tacCommand.i2CWriteRegister(addr, reg, data);
		}
		catch (...)
		{

		}
	}

	waitForCompletion();
}

QString TACPSOCDriveThread::serialPortError()
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

void TACPSOCDriveThread::handleError
(
	QSerialPort::SerialPortError error
)
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

void TACPSOCDriveThread::on_readyRead()
{
	_readyRead = true;
}

void TACPSOCDriveThread::setName
(
	const QByteArray& newName
)
{
	if (newName.length() <= 32)
	{
		if (newName != _name)
		{
			if (isAlphaNumeric(newName) == true)
			{
				{
					TACPSOCCommand tacCommand(this, this);

					tacCommand.setName(newName);
				}

				waitForCompletion();
			}
		}
	}
}

quint32 TACPSOCDriveThread::send
(
	const QByteArray& sendMe,
	const Arguments& arguments,
	bool console,
	ReceiveInterface* recieveInterface,
	bool store
)
{
	return _tacProtocol.sendCommand(sendMe, arguments, console, recieveInterface, store);
}

bool TACPSOCDriveThread::ready()
{
	return _tacProtocol.queueSize() == 0;
}

bool TACPSOCDriveThread::readSerialData()
{
	bool result(false);

	if (_readyRead == true)
	{
		_readyRead = false;

		QByteArray buffer(_serialPort->readAll());

		if (buffer.isEmpty() == false)
		{
#ifdef DEBUG
			AppCore::writeToApplicationLogLine(buffer);
#endif
			_protocolInterface->handleRecievedData(buffer);
			result = true;
		}
	}

	return result;
}

void TACPSOCDriveThread::receive
(
	FramePackage& framePackage
)
{
	if (framePackage->_endTransaction == true)
	{
		clearWaitForCompletion();

		emit transactionEnded();
		emit progress(kProgressMax);
	}
	else
	{
		if (framePackage->_valid == true)
		{
			switch (framePackage->_requestHash)
			{
			case kVersionCommandHash: handleVersionResponse(framePackage); break;
			case kGetPlatformIDCommandHash: handlePlatformID(framePackage); break;
			case kGetUUIDCommandHash: handleUUIDResponse(framePackage); break;
			case kGetNameCommandHash: handleGetNameResponse(framePackage); break;
			case kSetNameCommandHash: handleSetName(framePackage); break;
			case kClearResetCountCommandHash: emit resetCountCleared(); break;
			case kI2CReadRegisterCommandHash:
			case kI2CReadRegisterValueCommandHash:
				handleI2CRead(framePackage); break;
			case kI2CWriteRegisterCommandHash: handleI2CWrite(framePackage); break;
			case kSetPinCommandHash: handleSetPin(framePackage); break;
			default:
				break;
			}
		}
		else
		{
			clearWaitForCompletion();  // Don't want sync to hang on an error

			switch (framePackage->_requestHash)
			{
			case kGetPlatformIDCommandHash:
			{
				TACPSOCCommand tacCommand(this, this);
				tacCommand.version();
				break;
			}
			case kI2CReadRegisterCommandHash:
			case kI2CReadRegisterValueCommandHash:
				handleI2CRead(framePackage); break;

			case kI2CWriteRegisterCommandHash:
				handleI2CWrite(framePackage);
				break;

			default:
				break;
			}
		}
	}

	log(framePackage);

	_protocolInterface->clearPendingFrame();
}

void TACPSOCDriveThread::run()
{
	Q_ASSERT(_protocolInterface != Q_NULLPTR);

	AppCore::getAppCore()->setRunLogging(AppCore::getAppCore()->getPreferences()->loggingActive());

	if (openSerialDevice() == true)
	{
		emit deviceOpen();

		connect(_serialPort, &QSerialPort::errorOccurred, this, &TACPSOCDriveThread::handleError, Qt::DirectConnection);
		connect(_serialPort, &QSerialPort::readyRead, this, &TACPSOCDriveThread::on_readyRead, Qt::DirectConnection);

		startRunning();

		emit deviceStatusChange(tr("Starting"));

		_readyRead = false;
		_connected = false;

		{
			TACPSOCCommand tacCommand(this, this);
			tacCommand.platformID();
			tacCommand.version();
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
					{
						receive(framePackage);
					}
					else
					{
#ifdef DEBUG
						AppCore::writeToApplicationLogLine("TACPSOCDriveThread::run()::Write: " + framePackage->_codedRequest);
#endif

						qint64 bytesWritten = _serialPort->write(framePackage->_codedRequest);
						if (bytesWritten == -1)
						{
							AppCore::writeToApplicationLogLine(QByteArray("TACPSOCDriveThread::run()::bytesWritten == -1"));

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
		AppCore::writeToApplicationLogLine("TACPSOCDriveThread::run(): Serial port closed");

		_connected = false;
	}
	else
	{
		emit errorOnOpen(lastErrorMessage());
	}

	shutdownLogging();
}

void TACPSOCDriveThread::handleGetNameResponse
(
	FramePackage& framePackage
)
{
	if (framePackage->_responses.count() > 1)
	{
		QByteArray response = framePackage->_responses.at(1);
		_name = response;

		emit nameUpdate(_name);
	}
}

void TACPSOCDriveThread::handleGetResetCount
(
	FramePackage& framePackage
)
{
	if (framePackage.isNull() == false)
	{
		if (framePackage->_responses.count() >= 3)
		{
			if (framePackage->_responses.at(0) == "getresetcount")
			{
				if (framePackage->_responses.at(2) == "ok")
				{
					QByteArray value = framePackage->_responses.at(1);
					_resetCount = value.toUInt();
				}
			}
		}
	}

	emit resetCountUpdate(_resetCount);
}

void TACPSOCDriveThread::handleI2CRead
(
	FramePackage& framePackage
)
{
	if (framePackage->_requestHash == kI2CReadRegisterValueCommandHash)
	{
		if (framePackage->_valid)
		{
			QByteArray response;

			QList<QByteArray> responses = framePackage->_responses;
			responses.pop_back();

			if (responses.empty() == false)
				 response = responses.last();
			else
				response = framePackage->_responses.last();

			emit i2CReadResult(response, true);
		}
		else
		{
			emit i2CReadResult(framePackage->_responses.last(), false);
		}
	}
}

void TACPSOCDriveThread::handleI2CWrite
(
	FramePackage& framePackage
)
{
	if (framePackage->_valid)
	{
		QByteArray successResult = framePackage->_request + " " + framePackage->getArgument(0).toByteArray() + " Successful";
		emit i2CWriteResult(successResult);
	}
	else
	{
		QByteArray result = "\"" + framePackage->_responses.last() + "\" Are the parameters correct?";
		emit i2CWriteResult(result);
	}
}

void TACPSOCDriveThread::handleSetPin
(
	FramePackage& framePackage
)
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

void TACPSOCDriveThread::handleSetName
(
	FramePackage& framePackage
)
{
	QVariant variant{framePackage->getArgument(0)};

	if (variant.isValid())
	{
		_name = variant.toByteArray();
		framePackage->_synonym = "Set Name " + _name;
	}

	emit nameUpdate(_name);
}

void TACPSOCDriveThread::handleUUIDResponse
(
	FramePackage& framePackage
)
{
	if (framePackage->_responses.count() > 1)
	{
		QByteArray response = framePackage->_responses.at(1);
		_uuid = response;

		emit uuidUpdate(_uuid);
	}
}

void TACPSOCDriveThread::handleVersionResponse
(
	FramePackage& framePackage
)
{
	static int retryCount{1};

	if (framePackage->_responses.count() > 1)
	{
		bool isEPM{false};
		QByteArray versionString = framePackage->_responses.at(0);
		if (versionString.contains("EPM"))
			isEPM = true;
		else
		{
			versionString = framePackage->_responses.at(1);
			if (versionString.contains("EPM"))
				isEPM = true;
		}

		_versionString = versionString;
		AppCore::writeToApplicationLogLine(QString("Version String: %1").arg(_versionString.data()));

		retryCount = 1;

		if (isEPM)
		{
			_hardwareType = ePSOC;
		}
		else
		{
			_hardwareType = eSpiderBoard;
		}

		emit hardwareTypeUpdate(debugBoardTypeString());

		QStringList versionParts = QString(_versionString).split(" ", Qt::SkipEmptyParts);
		if (versionParts.count() >= 7)
		{
			int fwIndex = versionParts.indexOf("FW");
			if (fwIndex != -1)
			{
				try
				{
					QString firmwareString = versionParts.at(fwIndex + 2);

					_firmwareString = firmwareString.toLatin1();

					QStringList firmwareVersionParts = firmwareString.split(".", Qt::SkipEmptyParts);
					if (firmwareVersionParts.count() >= 4)
					{
						_firmwareMajor = firmwareVersionParts.at(0).toUInt();
						_firmwareChip = firmwareVersionParts.at(1).toUInt();
						_firmwareMinor = firmwareVersionParts.at(2).toUInt();
						_firmwareRevision = firmwareVersionParts.at(3).toUInt();

						if (_firmwareMinor < 15)
							_oldFirmware = true;
					}

					emit firmwareVersionUpdate(firmwareString);
				}
				catch (...)
				{
					AppCore::writeToApplicationLogLine("Version Parts Failed");
				}
			}
		}

		emit deviceStatusChange(tr("TAC Version Good"));

		setupConnected();
	}
	else if (retryCount < 4)
	{
		AppCore::writeToApplicationLogLine(QString("Version Failed Retry %1").arg(retryCount));
		TACPSOCCommand tacCommand(this, this);

		tacCommand.version();

		retryCount++;
	}
}

void TACPSOCDriveThread::handlePlatformID
(
	FramePackage& framePackage
)
{
	if (framePackage->_responses.count() >= 2)
	{
		bool okay{false};

		int boardID = framePackage->_responses.at(1).toInt(&okay);
		if (okay)
		{
			_platformID = static_cast<PlatformID>(boardID);
		}
	}

	emit platformIDUpdate(static_cast<int>(_platformID));

	setupConnected();
}

void TACPSOCDriveThread::setupConnected()
{
	if (_connected == false)
	{
		emit hardwareVersionUpdate(hardwareVersionString());

		_connected = true;

		// okay, let send our initial requests
		AppCore::writeToApplicationLogLine("");
		TACPSOCCommand tacCommand(this, this);

		AppCore::writeToApplicationLogLine("tacCommand.uuid()");
		tacCommand.uuid();

		AppCore::writeToApplicationLogLine("tacCommand.name()");
		tacCommand.name();

		emit deviceConnected();
	}
}

void TACPSOCDriveThread::setupDiscovery()
{
	emit hardwareVersionUpdate(hardwareVersionString());

	TACPSOCCommand tacCommand(this, this);
	tacCommand.uuid();
	tacCommand.name();
	tacCommand.platformID();
}

void TACPSOCDriveThread::log
(
	FramePackage& framePackage
)
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
