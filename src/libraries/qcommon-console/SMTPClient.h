#ifndef SMTPCLIENT_H
#define SMTPCLIENT_H
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
// Copyright 2013-2021 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
