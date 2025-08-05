#ifndef PLATFORMID_H
#define PLATFORMID_H
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

#include "DebugBoardType.h"
#include "FTDIPinSet.h"

#include <QMap>
#include <QSharedPointer>
#include <QString>

typedef quint32 PlatformID;

const PlatformID ALPACA_LITE_ID{13}; // Needed for hard coded classic devices

struct _PlatformEntry
{
	_PlatformEntry()
	{
		for (int i{0}; i < kMaxPinSetCount; i++)
			_pinSets[i] = NoOptions;
	}
	_PlatformEntry(PlatformID platformID, DebugBoardType boardType, const QString& description, const QString& path = QString(), const QByteArray& usbDescriptor = QByteArray());
	~_PlatformEntry() = default;

	PlatformID					_platformID{0};
	DebugBoardType				_boardtype{eUnknownDebugBoard};
	QString						_description;
	QByteArray					_usbDescriptor;
	QString						_path;
	FTDIPinSets					_pinSets[kMaxPinSetCount];
};

typedef QSharedPointer<_PlatformEntry> PlatformEntry;
typedef QList<PlatformEntry> PlatformIDList;
typedef QMap<PlatformID, PlatformEntry> PlatformIDs;
typedef PlatformIDs::const_iterator PlatformIDIterator;

class QCOMMONCONSOLE_EXPORT PlatformContainer
{
public:
	PlatformContainer() = delete;
	PlatformContainer(const PlatformContainer& copyMe) = delete;
	~PlatformContainer() = delete;

	static void initialize();
	static PlatformIDList getEntries();
	static void addEntry(PlatformEntry platformEntry);

	static QString toString(PlatformID platformID);

	static PlatformID fromUSBDescriptor(const QByteArray& usbDescriptor);

	static DebugBoardType getDebugBoardType(PlatformID platformID);
	static PlatformIDList getDebugBoards();
	static PlatformIDList getDebugBoardsOfType(DebugBoardType debugBoardType);

private:
	static void initializeDynamic();

	static PlatformIDs			_platformIds;

};

#endif // PLATFORMID_H
