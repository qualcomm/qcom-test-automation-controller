#ifndef FRAMECODER_H
#define FRAMECODER_H
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

// QCommon
#include "SendInterface.h"

// Qt
#include <QByteArray>
#include <QObject>
#include <QPair>
#include <QVariant>

typedef QPair<QString, QVariant> ErrorParameter;
typedef QList<ErrorParameter> ErrorParameters;

class ProtocolInterface;

typedef void (*FrameCompleteFunc)(const QByteArray& completedFrame, ProtocolInterface* userData);
typedef void (*BadFrameFunc)(const QByteArray& completedFrame, ProtocolInterface* userData);

class QCOMMONCONSOLE_EXPORT FrameCoder
{
public:
	FrameCoder() = default;
	virtual ~FrameCoder() = default;

	virtual void reset();

	void setupCallbackFunctions(ProtocolInterface* userData, FrameCompleteFunc frameFunc, BadFrameFunc badFrameFunc);

	virtual void decode(const QByteArray& decodeMe);
	virtual QByteArray encode(const QByteArray& encodeMe, const Arguments& arguments);

protected:
	Q_DISABLE_COPY(FrameCoder)

	QByteArray variantToBoolString(const QVariant& state) const;

	QByteArray					_frame;

	ProtocolInterface*			_protocolInterface{Q_NULLPTR};
	FrameCompleteFunc			_frameFunction{Q_NULLPTR};
	BadFrameFunc				_badFrameFunction{Q_NULLPTR};
};

#endif // FRAMECODER_H
