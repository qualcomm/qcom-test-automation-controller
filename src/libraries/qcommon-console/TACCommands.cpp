// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "TACCommands.h"
#include "TacCommandHashes.h"

// QCommon
#include "StringUtilities.h"

// Qt
#include <QMap>
#include <QVariant>

QMap<HashType, oldCommandEntry> gCommandMap;
QMap<QByteArray, HashType> gAliasMap;

static void InitializeCommandMap();
static QByteArray CommandToCompressedCommand(const QByteArray& compressMe);
static oldCommandEntry makeCommandEntry(HashType hash, const QByteArray& longCommand, const QByteArray& alias, const QByteArray& helpText, bool setter = false, QMetaType::Type _argType = QMetaType::UnknownType);

HashType AddCommandToEntries
(
	const QByteArray& commandToAdd,
	const QByteArray& alias,
	const QByteArray& helpText
)
{
	HashType result{0};

	if (gCommandMap.count() == 0)
		InitializeCommandMap();

	QByteArray compressedCommand = CommandToCompressedCommand(commandToAdd);

	result = CommandStringToHash(commandToAdd);
	if (result == 0)
	{
		HashType hash = arrayHash(compressedCommand);

		gCommandMap[hash] = makeCommandEntry(hash, commandToAdd, alias, helpText);
	}

	return result;
}

QByteArray CommandToCompressedCommand(const QByteArray& compressMe)
{
	QString compressString{compressMe};

	compressString = compressString.remove(QString(" "), Qt::CaseInsensitive);
	compressString = compressString.trimmed().toLower();

	return compressString.toLatin1();
}

HashType CommandStringToHash(const QByteArray& commandString)
{
	HashType result{0};

	if (gCommandMap.count() == 0)
		InitializeCommandMap();

	if (gAliasMap.contains(commandString))
	{
		result = gAliasMap[commandString];
	}
	else
	{
		QByteArray compressedCommand = CommandToCompressedCommand(commandString);
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
	const QByteArray& longCommand,
	const QByteArray& alias,
	const QByteArray& helpText,
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

	gAliasMap[alias] = hash;

	return commandEntry;
}

void InitializeCommandMap()
{
	gCommandMap[kVersionCommandHash] = makeCommandEntry(kVersionCommandHash, kVersionCommand, "version",
		"Returns the QTAC board's version information");

	gCommandMap[kGetNameCommandHash] = makeCommandEntry(kGetNameCommandHash, kGetNameCommand, "getname",
		"Returns the name of the QTAC board");
	gCommandMap[kSetNameCommandHash] = makeCommandEntry(kSetNameCommandHash, kSetNameCommand, "setname",
		"Sets the name of the QTAC board ex. setname foobar", true, QMetaType::QByteArray);

	gCommandMap[kGetUUIDCommandHash] = makeCommandEntry(kGetUUIDCommandHash, kGetUUIDCommand, "uuid",
		"Returns the UUID of the QTAC board");

	gCommandMap[kGetPlatformIDCommandHash] = makeCommandEntry(kGetPlatformIDCommandHash, kGetPlatformIDCommand, "platid",
		"Returns the Platform ID of the QTAC board");

	gCommandMap[kGetResetCountCommandHash] = makeCommandEntry(kGetResetCountCommandHash, kGetResetCountCommand, "resetcnt",
		"Returns the Reset Count");

	gCommandMap[kClearResetCountCommandHash] = makeCommandEntry(kClearResetCountCommandHash, kClearResetCountCommand, "clrresetcnt",
		"Clears the Reset Count");

	gCommandMap[kI2CReadRegisterCommandHash] = makeCommandEntry(kI2CReadRegisterCommandHash, kI2CReadRegisterCommand, "rdreg",
		"Reads an I2C register");

	gCommandMap[kI2CReadRegisterValueCommandHash] = makeCommandEntry(kI2CReadRegisterValueCommandHash, kI2CReadRegisterValueCommand, "",
		"Internal, not visible");

	gCommandMap[kI2CWriteRegisterCommandHash] = makeCommandEntry(kI2CWriteRegisterCommandHash, kI2CWriteRegisterCommand, "wrreg",
		"Writes an I2C register");

	gCommandMap[kSetPinCommandHash] = makeCommandEntry(kSetPinCommandHash, kSetPinCommand, "setpin",
		"setpin nn on|off - Toggles a hardware pin nn, nn being the pin number");

	gCommandMap[kPIC32CXClearBufferHash] = makeCommandEntry(kPIC32CXClearBufferHash, kPIC32CXClearBufferCommand, "\n",
		"\n - Clear buffer stream");

	gCommandMap[kPIC32CXVersionCommandHash] = makeCommandEntry(kPIC32CXVersionCommandHash, kPIC32CXPlatformIDCommand, "*IDN?",
		"*IDN? - Requests debug board to identify itself");

	gCommandMap[kPIC32CXSetPinCommandHash] = makeCommandEntry(kPIC32CXSetPinCommandHash, kPIC32CXSetPinCommand, "CONF:DIG:ON",
		"CONF:DIG:ON on|off (@pin)- Toggles a hardware pin nn, nn being the pin number");
}
