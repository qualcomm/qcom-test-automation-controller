#ifndef FRAMEPACKAGE_H
#define FRAMEPACKAGE_H
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
