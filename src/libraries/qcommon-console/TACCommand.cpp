// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "TACCommand.h"

QString TACCommand::command()
{
	return _command;
}

bool TACCommand::contains
(
	const QString &command,
	const TACCommands &tacCommands
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

TACCommand TACCommand::find(const QString &command, const TACCommands &tacCommands)
{
	TACCommand result;

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

TACCommand TACCommand::find
(
	PinID pin,
	const TACCommands& tacCommands
)
{
	TACCommand result;

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
