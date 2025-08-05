// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "TACCommand.h"

QString TacCommand::command()
{
	return _command;
}

bool TacCommand::contains
(
	const QString &command,
	const TacCommands &tacCommands
)
{
	bool result{false};

	for (const auto& tacCommand: tacCommands)
	{
		if (tacCommand._command == command)
		{
			result = true;
			break;
		}
	}

	return result;
}

TacCommand TacCommand::find(const QString &command, const TacCommands &tacCommands)
{
	TacCommand result;

	for (const auto& tacCommand: tacCommands)
	{
		if (tacCommand._command == command)
		{
			result = tacCommand;
			break;
		}
	}

	return result;
}

TacCommand TacCommand::find
(
	PinID pin,
	const TacCommands& tacCommands
)
{
	TacCommand result;

	for (const auto& tacCommand: tacCommands)
	{
		if (tacCommand._pin == pin)
		{
			result = tacCommand;
			break;
		}
	}

	return result;
}
