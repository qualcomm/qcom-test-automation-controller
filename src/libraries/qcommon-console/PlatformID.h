#ifndef PLATFORMID_H
#define PLATFORMID_H
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
// Copyright 2021 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
