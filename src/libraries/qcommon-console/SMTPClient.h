#ifndef SMTPCLIENT_H
#define SMTPCLIENT_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
