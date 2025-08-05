#ifndef SMTPCLIENT_H
#define SMTPCLIENT_H
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
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "QCommonConsoleGlobal.h"

// QT
#include <QException>
#include <QByteArray>
#include <QByteArrayList>

// The default SMTP port
const quint16 kDefaultPort = 25;
const quint32 kDefaultTimeout = 30000;

class QCOMMONCONSOLE_EXPORT SMTPClient
{

public:
	SMTPClient() = default;
	~SMTPClient();

	QString getHost();
	void setHost(const QByteArray& hostName);

	quint16 getPort();
	void setPort(const quint16 portNumber);

	quint16 getTimeout();
	void setTimeout(const quint16 timeout);

	void setCCAddresses(const QByteArrayList& ccAddresses)
	{
		_ccAddresses = ccAddresses;
	}

	void setMimeFiles(const QByteArrayList& mimeFiles);

	void send(const QByteArray& senderAddress, const QByteArray& receiverAddress, const QByteArray &subject, const QByteArray& body);

private:
	bool isMailAddressValid(const QByteArray& mailAddress);
	QByteArray contentType(const QByteArray& fileName);

	QByteArray					_hostName;
	quint16						_portNumber{kDefaultPort};
	quint32						_timeout{kDefaultTimeout};
	QByteArrayList				_ccAddresses;
	QByteArrayList				_mimeFiles;
};

class QCOMMONCONSOLE_EXPORT SMTPClientException : QException
{
public:
	SMTPClientException(QString const& message) :
		  _message(message)
	{

	}

	~SMTPClientException()
	{

	}

	void raise() const
	{
		throw *this;
	}

	SMTPClientException* clone() const
	{
		return new SMTPClientException(*this);
	}

	QString getMessage() const
	{
		return _message;
	}

private:
	QString                 _message;
};

#endif // SMTPCLIENT_H
