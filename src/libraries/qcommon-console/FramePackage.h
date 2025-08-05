#ifndef FRAMEPACKAGE_H
#define FRAMEPACKAGE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
