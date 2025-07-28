#ifndef FRAMEPACKAGE_H
#define FRAMEPACKAGE_H
// Confidential and Proprietary – Qualcomm Technologies, Inc.

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
// Copyright 2018-2021 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

// Author: msimpson

// QCommon
class ReceiveInterface;
#include "QCommonConsoleGlobal.h"

// Qt
#include <QByteArray>
#include <QList>
#include <QSharedPointer>
#include <QVariant>

class QCOMMONCONSOLE_EXPORT _FramePackage
{
public:
	_FramePackage() = default;
	~_FramePackage() = default;

	QVariant getArgument(quint32 index)
	{
		QVariant result;

		if (_arguments.count() > static_cast<int>(index))
			result = _arguments.at(index).toByteArray();

		return result;
	}

	QByteArray                  _lastError;
	QByteArray					_request;
	uint						_requestHash{0};
	QList<QVariant>				_arguments;
	QByteArray					_synonym;
	QByteArray					_codedRequest;
	QList<QByteArray>			_responses;
	quint32						_packetID{0};
	QByteArray					_comment;

	bool						_endTransaction{false};
	bool						_valid{true};
	bool						_console{false};
	bool						_shouldStore{false};
	ReceiveInterface*			_recieveInterface{Q_NULLPTR};
	quint64						_tickcount{0};
	quint32						_delayInMilliSeconds{0};

private:
	Q_DISABLE_COPY(_FramePackage)
};

typedef QSharedPointer<_FramePackage> FramePackage;
typedef QList<FramePackage> FramePackageList;


#endif // FRAMEPACKAGE_H
