#ifndef SMTPCLIENT_H
#define SMTPCLIENT_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

// QT
#include <QException>

#include <cstdint>
#include <string>
#include <vector>

// The default SMTP port
const uint16_t kDefaultPort = 25;
const uint32_t kDefaultTimeout = 30000;

class QCOMMONCONSOLE_EXPORT SMTPClient
{

public:
	SMTPClient() = default;
	~SMTPClient();

	std::string getHost();
	void setHost(const std::string& hostName);

	uint16_t getPort();
	void setPort(const uint16_t portNumber);

	uint16_t getTimeout();
	void setTimeout(const uint16_t timeout);

	void setCCAddresses(const std::vector<std::string>& ccAddresses)
	{
		_ccAddresses = ccAddresses;
	}

	void setMimeFiles(const std::vector<std::string>& mimeFiles);

	void send(const std::string& senderAddress, const std::string& receiverAddress, const std::string &subject, const std::string& body);

private:
	bool isMailAddressValid(const std::string& mailAddress);
	std::string contentType(const std::string& fileName);

	std::string					_hostName;
	uint16_t						_portNumber{kDefaultPort};
	uint32_t						_timeout{kDefaultTimeout};
	std::vector<std::string>	_ccAddresses;
	std::vector<std::string>	_mimeFiles;
};

class QCOMMONCONSOLE_EXPORT SMTPClientException : QException
{
public:
	SMTPClientException(std::string const& message) :
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

	std::string getMessage() const
	{
		return _message;
	}

private:
	std::string             _message;
};

#endif // SMTPCLIENT_H
