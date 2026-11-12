#ifndef FRAMEPACKAGE_H
#define FRAMEPACKAGE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QCommon
class ReceiveInterface;
#include "QCommonConsoleGlobal.h"
#include "SendInterface.h"

// Qt
#include <QByteArray>
#include <QList>
#include <QSharedPointer>

#include <cstdint>

class QCOMMONCONSOLE_EXPORT _FramePackage
{
public:
	_FramePackage() = default;
	~_FramePackage() = default;

	Argument getArgument(uint32_t index)
	{
		Argument result;

		if (_arguments.size() > index)
			result = _arguments.at(index);

		return result;
	}

	QByteArray                  _lastError;
	QByteArray					_request;
	uint						_requestHash{0};
	Arguments					_arguments;
	QByteArray					_synonym;
	QByteArray					_codedRequest;
	QList<QByteArray>			_responses;
	uint32_t					_packetID{0};
	QByteArray					_comment;

	bool						_endTransaction{false};
	bool						_valid{true};
	bool						_console{false};
	bool						_shouldStore{false};
	ReceiveInterface*			_recieveInterface{Q_NULLPTR};
	uint64_t					_tickcount{0};
	uint32_t					_delayInMilliSeconds{0};

private:
	Q_DISABLE_COPY(_FramePackage)
};

typedef QSharedPointer<_FramePackage> FramePackage;
typedef QList<FramePackage> FramePackageList;

#endif // FRAMEPACKAGE_H
