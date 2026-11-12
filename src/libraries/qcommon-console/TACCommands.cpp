// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "TACCommands.h"
#include "TACCommandHashes.h"

// QCommon
#include "StringUtilities.h"

// Qt
#include <QMap>
#include <QVariant>

QMap<HashType, oldCommandEntry> gCommandMap;
QMap<QByteArray, HashType> gAliasMap;

static void InitializeCommandMap();
static std::string CommandToCompressedCommand(const std::string& compressMe);
static oldCommandEntry makeCommandEntry(HashType hash, const std::string& longCommand, const std::string& alias, const std::string& helpText, bool setter = false, QMetaType::Type _argType = QMetaType::UnknownType);

HashType AddCommandToEntries
(
	const std::string& commandToAdd,
	const std::string& alias,
	const std::string& helpText
)
{
	HashType result{0};

	if (gCommandMap.count() == 0)
		InitializeCommandMap();

	std::string compressedCommand = CommandToCompressedCommand(commandToAdd);

	result = CommandStringToHash(commandToAdd);
	if (result == 0)
	{
		HashType hash = arrayHash(compressedCommand);

		gCommandMap[hash] = makeCommandEntry(hash, commandToAdd, alias, helpText);
	}

	return result;
}

std::string CommandToCompressedCommand(const std::string& compressMe)
{
	QString compressString{QString::fromStdString(compressMe)};

	compressString = compressString.remove(QString(" "), Qt::CaseInsensitive);
	compressString = compressString.trimmed().toLower();

	return compressString.toStdString();
}

HashType CommandStringToHash(const std::string& commandString)
{
	HashType result{0};

	if (gCommandMap.count() == 0)
		InitializeCommandMap();

	QByteArray commandStringBytes = QByteArray::fromStdString(commandString);

	if (gAliasMap.contains(commandStringBytes))
	{
		result = gAliasMap[commandStringBytes];
	}
	else
	{
		std::string compressedCommand = CommandToCompressedCommand(commandString);
		HashType hash = arrayHash(compressedCommand);
		if (gCommandMap.contains(hash))
			result = hash;
	}

	return result;
}

oldCommandEntry CommandHashToCommandEntry
(
	HashType commandHash
)
{
	oldCommandEntry result;

	if (gCommandMap.count() == 0)
		InitializeCommandMap();

	if (gCommandMap.contains(commandHash))
		result = gCommandMap[commandHash];

	return result;
}

oldCommandEntry makeCommandEntry
(
	HashType hash,
	const std::string& longCommand,
	const std::string& alias,
	const std::string& helpText,
	bool setter,
	QMetaType::Type _argType
)
{
	oldCommandEntry commandEntry;

	commandEntry._longCommand = longCommand;
	commandEntry._compressedCommand = CommandToCompressedCommand(longCommand);
	commandEntry._hash = hash;
	commandEntry._alias = alias;
	commandEntry._helpText = helpText;
	commandEntry._setter = setter;
	commandEntry._argType = _argType;

	gAliasMap[QByteArray::fromStdString(alias)] = hash;

	return commandEntry;
}

void InitializeCommandMap()
{
	gCommandMap[kVersionCommandHash] = makeCommandEntry(kVersionCommandHash, kVersionCommand.toStdString(), "version",
		"Returns the QTAC board's version information");

	gCommandMap[kGetNameCommandHash] = makeCommandEntry(kGetNameCommandHash, kGetNameCommand.toStdString(), "getname",
		"Returns the name of the QTAC board");
	gCommandMap[kSetNameCommandHash] = makeCommandEntry(kSetNameCommandHash, kSetNameCommand.toStdString(), "setname",
		"Sets the name of the QTAC board ex. setname foobar", true, QMetaType::QByteArray);

	gCommandMap[kGetUUIDCommandHash] = makeCommandEntry(kGetUUIDCommandHash, kGetUUIDCommand.toStdString(), "uuid",
		"Returns the UUID of the QTAC board");

	gCommandMap[kGetPlatformIDCommandHash] = makeCommandEntry(kGetPlatformIDCommandHash, kGetPlatformIDCommand.toStdString(), "platid",
		"Returns the Platform ID of the QTAC board");

	gCommandMap[kGetResetCountCommandHash] = makeCommandEntry(kGetResetCountCommandHash, kGetResetCountCommand.toStdString(), "resetcnt",
		"Returns the Reset Count");

	gCommandMap[kClearResetCountCommandHash] = makeCommandEntry(kClearResetCountCommandHash, kClearResetCountCommand.toStdString(), "clrresetcnt",
		"Clears the Reset Count");

	gCommandMap[kI2CReadRegisterCommandHash] = makeCommandEntry(kI2CReadRegisterCommandHash, kI2CReadRegisterCommand.toStdString(), "rdreg",
		"Reads an I2C register");

	gCommandMap[kI2CReadRegisterValueCommandHash] = makeCommandEntry(kI2CReadRegisterValueCommandHash, kI2CReadRegisterValueCommand.toStdString(), "",
		"Internal, not visible");

	gCommandMap[kI2CWriteRegisterCommandHash] = makeCommandEntry(kI2CWriteRegisterCommandHash, kI2CWriteRegisterCommand.toStdString(), "wrreg",
		"Writes an I2C register");

	gCommandMap[kSetPinCommandHash] = makeCommandEntry(kSetPinCommandHash, kSetPinCommand.toStdString(), "setpin",
		"setpin nn on|off - Toggles a hardware pin nn, nn being the pin number");

	gCommandMap[kPIC32CXClearBufferHash] = makeCommandEntry(kPIC32CXClearBufferHash, kPIC32CXClearBufferCommand.toStdString(), "\n",
		"\n - Clear buffer stream");

	gCommandMap[kPIC32CXVersionCommandHash] = makeCommandEntry(kPIC32CXVersionCommandHash, kPIC32CXPlatformIDCommand.toStdString(), "*IDN?",
		"*IDN? - Requests debug board to identify itself");

	gCommandMap[kPIC32CXSetPinCommandHash] = makeCommandEntry(kPIC32CXSetPinCommandHash, kPIC32CXSetPinCommand.toStdString(), "CONF:DIG:ON",
		"CONF:DIG:ON on|off (@pin)- Toggles a hardware pin nn, nn being the pin number");
}
