// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "SMTPClient.h"
#include "AppCore.h"

#include <QFile>
#include <QFileInfo>
#include <QTcpSocket>

#include <regex>

SMTPClient::~SMTPClient()
{
}

std::string SMTPClient::getHost()
{
	return _hostName;
}

void SMTPClient::setHost(const std::string& hostName)
{
	_hostName = hostName;
}

uint16_t SMTPClient::getPort()
{
	return _portNumber;
}

void SMTPClient::setPort(const uint16_t portNumber)
{
	_portNumber = portNumber;
}

uint16_t SMTPClient::getTimeout()
{
	return _timeout;
}

void SMTPClient::setTimeout(const uint16_t timeout)
{
	_timeout = timeout;
}

void SMTPClient::setMimeFiles(const std::vector<std::string>& mimeFiles)
{
	_mimeFiles = mimeFiles;
}

void SMTPClient::send(const std::string& senderAddress, const std::string& receiverAddress, const std::string& subject, const std::string& body)
{
	if (!isMailAddressValid(senderAddress) || !isMailAddressValid(receiverAddress))
		throw SMTPClientException("The supplied email address is invalid. Please use a valid email address.");

	QTcpSocket socket;

	socket.connectToHost(QString::fromStdString(_hostName), _portNumber);

	if (!socket.waitForConnected(_timeout))
		throw SMTPClientException(socket.errorString().toStdString());

	if (!socket.isValid())
		throw SMTPClientException("The TCP Socket is invalid.");

	socket.write("EHLO 22.03.19.98\r\n");
	socket.flush();

	if (socket.waitForReadyRead())
		AppCore::writeToApplicationLogLine(socket.readAll().toStdString());

	std::string mailFrom = "mail from: " + senderAddress + "\r\n";
	socket.write(mailFrom.c_str(), static_cast<qint64>(mailFrom.length()));
	socket.flush();

	if (socket.waitForReadyRead())
		AppCore::writeToApplicationLogLine(socket.readAll().toStdString());

	std::string rcptTo = "rcpt to: " + receiverAddress + " NOTIFY=success,failure\r\n";
	socket.write(rcptTo.c_str(), static_cast<qint64>(rcptTo.length()));
	socket.flush();

	if (socket.waitForReadyRead())
		AppCore::writeToApplicationLogLine(socket.readAll().toStdString());

	socket.write("data\r\n");
	socket.flush();

	if (socket.waitForReadyRead())
		AppCore::writeToApplicationLogLine(socket.readAll().toStdString());

	std::string subjectLine = "Subject: " + subject + "\r\n";
	socket.write(subjectLine.c_str(), static_cast<qint64>(subjectLine.length()));
	socket.flush();

	std::string message;

	message = "FROM: " + senderAddress + "\r\n";
	message += "TO: " + receiverAddress + "\r\n";

	for (const auto& ccUser : _ccAddresses)
	{
		message += "CC: " + ccUser + "\r\n";
	}

	// Initiate multipart MIME with cutting boundary "alpaca-boundary"
	message += "MIME-Version: 1.0\r\n";
	message += "Content-Type: multipart/mixed;boundary=alpaca-boundary\r\n\r\n";

	socket.write(message.c_str(), static_cast<qint64>(message.length()));
	socket.flush();

	/*
	 * Sample MIME message format:
	 *
	 * From: Some One <someone@example.com>
	 * MIME-Version: 1.0
	 * Content-Type: multipart/mixed; boundary="XXXXboundary text"
	 *
	 * --XXXXboundary text
	 * Content-Type: text/plain
	 * this is the body text
	 *
	 * --XXXXboundary text
	 *
	 * Content-Type: text/plain;
	 * Content-Disposition: attachment;filename="test.txt"
	 * this is the attachment text
	 * --XXXXboundary text--
	 */

	message = "--alpaca-boundary\r\n";
	message += "Content-Type: text/plain\r\n";
	message += body;
	message += "\r\n--alpaca-boundary\r\n";

	socket.write(message.c_str(), static_cast<qint64>(message.length()));
	socket.flush();

	if (!_mimeFiles.empty())
	{
		for (const auto& filePath : _mimeFiles)
		{
			QFile file(QString::fromStdString(filePath));

			if (file.exists() && file.open(QIODevice::ReadOnly))
			{
				std::string fileName = QFileInfo(file).fileName().toStdString();

				message = "Content-Type: " + contentType(fileName) + "\r\nContent-Disposition: attachment; filename=" + fileName + "\r\nMIME-Version: 1.0\r\nContent-Transfer-Encoding: base64\r\n\r\n";
				socket.write(message.c_str(), static_cast<qint64>(message.length()));
				socket.flush();

				std::string base64Data = file.readAll().toBase64().toStdString();

				size_t offset = 0;
				while (offset < base64Data.length())
				{
					std::string chunk = base64Data.substr(offset, 76);
					socket.write(chunk.c_str(), static_cast<qint64>(chunk.length()));
					socket.write("\r\n");
					socket.flush();
					offset += 76;
				}
			}
			else
			{
				//TODO: Add to application log
			}

			message = "\r\n\r\n--alpaca-boundary\r\n";
			socket.write(message.c_str(), static_cast<qint64>(message.length()));
			socket.flush();
		}
	}

	if (socket.waitForReadyRead())
		AppCore::writeToApplicationLogLine(socket.readAll().toStdString());

	socket.write(".\r\nQUIT\r\n");
	socket.flush();

	if (socket.waitForReadyRead())
		AppCore::writeToApplicationLogLine(socket.readAll().toStdString());

	socket.disconnectFromHost();
	socket.close();
}

bool SMTPClient::isMailAddressValid(const std::string& mailAddress)
{
	static const std::regex re(R"(\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,4}\b)", std::regex::icase);
	return std::regex_search(mailAddress, re);
}

static bool endsWithCaseSensitive(const std::string& value, const std::string& suffix)
{
	if (suffix.size() > value.size())
		return false;

	return value.compare(value.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::string SMTPClient::contentType(const std::string &fileName)
{
	std::string result{"application/octet-stream"};

	if (endsWithCaseSensitive(fileName, ".png"))
		result = "image/png";

	else if (endsWithCaseSensitive(fileName, ".txt"))
		result = "text/plain";

	return result;
}
