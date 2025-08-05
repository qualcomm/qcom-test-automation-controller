#ifndef COMMANDGROUP_H
#define COMMANDGROUP_H
/*
	Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries. 
	 
	Redistribution and use in source and binary forms, with or without
	modification, are permitted (subject to the limitations in the
	disclaimer below) provided that the following conditions are met:
	 
		* Redistributions of source code must retain the above copyright
		  notice, this list of conditions and the following disclaimer.
	 
		* Redistributions in binary form must reproduce the above
		  copyright notice, this list of conditions and the following
		  disclaimer in the documentation and/or other materials provided
		  with the distribution.
	 
		* Neither the name of Qualcomm Technologies, Inc. nor the names of its
		  contributors may be used to endorse or promote products derived
		  from this software without specific prior written permission.
	 
	NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
	GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
	HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
	IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "QCommonConsoleGlobal.h"

#include <QString>
#include <QStringList>

enum CommandGroups
{
	eUnknownCommandGroup = 0,
	eConnectionGroup,
	eButtonGroup,
	eSwitchGroup,
	eQuickSettingsGroup
};

class QCOMMONCONSOLE_EXPORT CommandGroup
{
public:
	static QString toString(CommandGroups commandGroup)
	{
		switch (commandGroup)
		{
		case eConnectionGroup: return "Connections";
		case eButtonGroup: return "Buttons";
		case eSwitchGroup: return "Switches";
		case eQuickSettingsGroup: return "Quick Settings";
		default:;
		}

		return QString();
	}

	static QStringList toStringList()
	{
		QStringList result;

		result.append("<Select Pin Group>");
		result.append(toString(eConnectionGroup));
		result.append(toString(eButtonGroup));
		result.append(toString(eSwitchGroup));
		result.append(toString(eQuickSettingsGroup));

		return result;
	}

	static CommandGroups fromString(const QString commandGroupStr)
	{
		QString temp{commandGroupStr.toLower()};

		if (temp == "connections")
			return eConnectionGroup;

		if (temp == "buttons")
			return eButtonGroup;

		if (temp == "switches")
			return eSwitchGroup;

		if (temp == "quick settings")
			return eQuickSettingsGroup;

		return eUnknownCommandGroup;
	}
};

#endif // COMMANDGROUP_H
