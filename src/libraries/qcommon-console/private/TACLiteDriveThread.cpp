// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "TACLiteDriveThread.h"

#include "TACCommands.h"
#include "AlpacaDevice.h"
#include "TacCommandHashes.h"
#include "TACLiteCommand.h"

// QCommon
#include "AppCore.h"

// Qt
#include <QDir>
#include <QDateTime>
#include <QTextStream>

#define on true
#define off false

const QString kTACLiteDriveTrainName{QStringLiteral("TAC Lite Drive Train")};

TacLiteDriveThread::TacLiteDriveThread
(
	uint hash
) :
	TACDriveThread(hash)
{
	_driveTrainName = kTACLiteDriveTrainName;
	setProtocolInterface(&_tacProtocol);
	_tacProtocol.setTacDriveTrain(this);

	AlpacaDevice alpacaDevice = _AlpacaDevice::findAlpacaDevice(hash);

	if (alpacaDevice->active())
	{
		_ftdiChipset = _FTDIChipset::getDevice(alpacaDevice->portName());
		if (_ftdiChipset.isNull() == false)
		{
			_portName = _ftdiChipset->portName();
			_platformID = _ftdiChipset->platformID();
			_name = _ftdiChipset->serialNumber();

			_versionString = _ftdiChipset->versionString().toLatin1();
			_firmwareString = _ftdiChipset->firmwareString().toLatin1();
			_firmwareMajor = 1;
			_firmwareChip = 10000;
			_firmwareMinor = 1;
			_firmwareRevision = 1;
			_description = "VTP Port";
			_uuid = "FTDI - No UUID";
			_hardwareType = eFTDI;

			setSerialNumber(_ftdiChipset->serialNumber());
			setPortName(_ftdiChipset->portName());

			connect(this, &TACDriveThread::deviceDisconnected, alpacaDevice.data(), &_AlpacaDevice::onDeviceDisconnect, Qt::DirectConnection);
			connect(this, &TACDriveThread::progress, alpacaDevice.data(), &_AlpacaDevice::progress);
		}
	}
}

TacLiteDriveThread::~TacLiteDriveThread()
{
}

bool TacLiteDriveThread::openFTDIDevice()
{
	bool result{false};

	if (_ftdiChipset.isNull() == false)
	{
		if (_ftdiChipset->isOpen() == false)
		{
			if (_ftdiChipset->open(_pinsets))
			{
				AppCore::writeToApplicationLogLine(QString("Opened port %1\n").arg(_ftdiChipset->portName().data()));

				result = true;
			}
			else
			{
				AppCore::writeToApplicationLogLine("TacLiteDriveThread::openFTDIDevice _ftdiDevice->open(pinsets) failed");
				emit errorOnOpen("FTDI device open failed. Check the application log");
			}
		}
		else
		{
			AppCore::writeToApplicationLogLine("TacLiteDriveThread::openFTDIDevice _ftdiDevice->isOpen() == false");
			emit errorOnOpen("FTDI device open failed. Check the application log");
		}
	}
	else
	{
		AppCore::writeToApplicationLogLine("TacLiteDriveThread::openFTDIDevice _ftdiDevice.isNull() == false");
		emit errorOnOpen("FTDI device open failed. Check the application log");
	}

	return result;
}

void TacLiteDriveThread::externalPowerControl
(
	bool state
)
{
	{
		TacLiteCommand tacCommand(this, this);

		tacCommand.externalPowerControl(state);
	}

	waitForCompletion();
}

void TacLiteDriveThread::setPinState
(
	quint16 pin,
	bool state
)
{
	{
		TacLiteCommand tacCommand(this, this);

		tacCommand.setPinState(pin, state);
	}

	waitForCompletion();
}

void TacLiteDriveThread::sendCommandSequence
(
	CommandEntries& commandEntries
)
{
	{
		TacLiteCommand tacCommand(this, this);

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
				tacCommand.addLogComment(commandEntry->_arguement.toString().toLatin1());
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

	waitForCompletion();
}

int TacLiteDriveThread::getResetCount()
{
	return 0;
}

void TacLiteDriveThread::clearResetCount()
{
}

void TacLiteDriveThread::i2CReadRegister
(
	quint32 addr,
	quint32 reg
)
{
	Q_UNUSED(addr)
	Q_UNUSED(reg)
}

void TacLiteDriveThread::i2CWriteRegister(quint32 addr, quint32 reg, quint32 data)
{
	Q_UNUSED(addr)
	Q_UNUSED(reg)
	Q_UNUSED(data)
}

void TacLiteDriveThread::setName
(
	const QByteArray& newName
)
{
	Q_UNUSED(newName)
}

quint32 TacLiteDriveThread::send
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

bool TacLiteDriveThread::ready()
{
	return _tacProtocol.queueSize() == 0;
}

void TacLiteDriveThread::receive(FramePackage &framePackage)
{
	if (framePackage->_console == true)
	{
		if (_consoleInterface != Q_NULLPTR)
		{
			_consoleInterface->handleConsoleResponse(framePackage);
		}
	}

	if (framePackage->_endTransaction == true)
	{
		clearWaitForCompletion();

		emit transactionEnded();
		emit progress(kProgressMax);
	}
	else if (framePackage->_valid == true)
	{
		switch (framePackage->_requestHash)
		{
		case kVersionCommandHash:
			handleVersionResponse(framePackage);
			break;

		case kGetPlatformIDCommandHash:
			handlePlatformID(framePackage);
			break;

		case kGetUUIDCommandHash:
			handleUUIDResponse(framePackage);
			break;

		case kSetNameCommandHash:
			handleSetName(framePackage);
			break;

		case kSetPinCommandHash:
			handleSetPin(framePackage);
			break;

		default:
			break;
		}
	}
	else
	{
		clearWaitForCompletion();

		if (framePackage->_lastError.isEmpty() == false)
			emit errorOnOpen(framePackage->_lastError);
	}

	log(framePackage);

	_protocolInterface->clearPendingFrame();
}

void TacLiteDriveThread::run()
{
	AppCore::writeToApplicationLogLine("TacLiteDriveThread::run()");

	if (openFTDIDevice() == true)
	{
		AppCore::writeToApplicationLogLine("TacLiteDriveThread::run() openFTDIDevice() == true");
		emit deviceOpen();
		startRunning();
	}

	if (weAreRunning())
	{
		emit hardwareTypeUpdate(_versionString);
		emit hardwareVersionUpdate("0.1.1");	
		emit firmwareVersionUpdate(_firmwareString);

		emit nameUpdate(_name);
		emit uuidUpdate(_uuid);
		emit serialNumUpdate(_serialNumber);
		emit platformIDUpdate(_platformID);

		emit deviceOpen();
		emit deviceStatusChange(tr("Starting"));

		AppCore::writeToApplicationLogLine("setupConnected()");

		setupConnected();
	}
	else
	{
		AppCore::writeToApplicationLogLine("TacLiteDriveThread::run() emit deviceDisconnected()");
		emit deviceDisconnected();
	}

	AppCore::writeToApplicationLogLine(QString("Hardware: %1\n").arg(_hardwareType));
	AppCore::writeToApplicationLogLine(QString("Platform ID: %1(%2)\n").arg(PlatformContainer::toString(_platformID)).arg(static_cast<int>(_platformID)));

	if (weAreRunning())
	{
		bool loopFinished{false};

		AppCore::writeToApplicationLogLine("TacLiteDriveThread::run() starting loop");

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

					int pin = framePackage->_codedRequest.toInt();
					bool state = getElectrialPinValue(framePackage->_requestHash, framePackage->_arguments);

					AppCore::writeToApplicationLogLine("TacLiteDriveThread::run() write " + framePackage->_codedRequest + " state:" + (state ? "on" : "off"));

					if (_ftdiChipset.isNull() == false)
					{
						if (_ftdiChipset->write(pin, state) == false)
						{
							QString logMessage = QString("TacLiteDriveThread::run() write failure pin %1").arg(pin);

							AppCore::writeToApplicationLogLine(logMessage);

							stopRunning();
						}
						else
						{
							QByteArray argument;

							if (framePackage->_arguments.at(0).typeId() == QMetaType::Bool)
							{
								argument = (framePackage->_arguments.at(0).toBool() == true) ? "on" : "off";
							}
							else
							{
								argument = framePackage->_arguments.at(0).toByteArray();
							}

							framePackage->_responses.push_back(framePackage->_request + " " + argument);

							receive(framePackage);
						}
					}
					else
					{
						AppCore::writeToApplicationLogLine("TacLiteDriveThread::run() ftdiChipSet is null.");
						stopRunning();
					}
				}
			}
			else
			{
				if (weAreRunning() == false)
				{
					AppCore::writeToApplicationLogLine("TacLiteDriveThread::run() loopFinished == true\n");
					loopFinished = true;
				}
				else
				{
					msleep(100);
				}
			}
		}
	}

	AppCore::writeToApplicationLogLine("TacLiteDriveThread::run() _ftdiDevice->close()");

	if (_ftdiChipset.isNull() == false)
		_ftdiChipset->close();

	_connected = false;

	emit deviceDisconnected();

	shutdownLogging();
}

void TacLiteDriveThread::sendCommand
(
	const QByteArray &command,
	bool console,
	ReceiveInterface *receiveInterface,
	bool shouldStore
)
{
	if (command.startsWith(kHelpCommand))
	{
		_consoleInterface->addConsoleText(_helpText.toLatin1());
	}
	else
	{
		QList<QVariant> args;
		QByteArray decodedCommand = decodeCommand(command, args);

		send(decodedCommand, args, console, receiveInterface, shouldStore);
	}
}

bool TacLiteDriveThread::getElectrialPinValue
(
	quint32 kCommandHash,
	const Arguments& arguments
)
{
	bool result{0};
	bool argValue = arguments.at(0).toBool();
	switch (kCommandHash)
	{
	case kSetPinCommandHash:
		result = argValue;
	}

	return result;
}

void TacLiteDriveThread::handleSetName
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

void TacLiteDriveThread::handleSetPin
(
	FramePackage &framePackage
)
{
	QVariant state{framePackage->getArgument(0)};
	QVariant pin{framePackage->getArgument(1)};

	framePackage->_synonym = "Set Pin " + pin.toByteArray() + " " + state.toByteArray();

	emit pinStateChanged(pin.toULongLong(), state.toBool());
}

void TacLiteDriveThread::handleUUIDResponse
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

void TacLiteDriveThread::handleVersionResponse
(
	FramePackage& framePackage
)
{
	Q_UNUSED(framePackage)
	emit deviceStatusChange(tr("TAC Version Good"));

	setupConnected();
}

void TacLiteDriveThread::handlePlatformID
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

void TacLiteDriveThread::setupConnected()
{
	if (_connected == false)
	{
		_connected = true;

		emit deviceConnected();
	}
}

void TacLiteDriveThread::setupDiscovery()
{
		// nothing to be done here
}
