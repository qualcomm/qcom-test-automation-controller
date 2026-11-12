// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "SMTPClient.h"
#include"AppCore.h"

#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QString>
#include <QStringList>
#include <QTcpSocket>

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
	if (isMailAddressValid(senderAddress) == false || isMailAddressValid(receiverAddress) == false)
		throw SMTPClientException("The supplied email address is invalid. Please use a valid email address.");

	QTcpSocket socket;

	socket.connectToHost(QString::fromStdString(_hostName), _portNumber);

	if (socket.waitForConnected(_timeout) == false)
		throw SMTPClientException(socket.errorString().toStdString());

	if (socket.isValid() == false)
		throw SMTPClientException("The TCP Socket is invalid.");

	socket.write("EHLO 22.03.19.98\r\n");
	socket.flush();

	if (socket.waitForReadyRead())
		AppCore::writeToApplicationLogLine(socket.readAll().toStdString());

	socket.write(QByteArray::fromStdString("mail from: " + senderAddress + "\r\n"));
	socket.flush();

	if (socket.waitForReadyRead())
		AppCore::writeToApplicationLogLine(socket.readAll().toStdString());

	socket.write(QByteArray::fromStdString("rcpt to: " + receiverAddress + " NOTIFY=success,failure\r\n"));
	socket.flush();

	if (socket.waitForReadyRead())
		AppCore::writeToApplicationLogLine(socket.readAll().toStdString());

	socket.write("data\r\n");
	socket.flush();

	if (socket.waitForReadyRead())
		AppCore::writeToApplicationLogLine(socket.readAll().toStdString());

	socket.write(QByteArray::fromStdString("Subject: " + subject + "\r\n"));
	socket.flush();

	QByteArray message;

	message = QByteArray::fromStdString("FROM: " + senderAddress + "\r\n");
	message.append(QByteArray::fromStdString("TO: " + receiverAddress + "\r\n"));

	for (auto& ccUser : _ccAddresses)
	{
		message.append(QByteArray::fromStdString("CC: " + ccUser + "\r\n"));
	}

	// Intitiate multipart MIME with cutting boundary "alpaca-boundary"
	message.append("MIME-Version: 1.0\r\n");
	message.append("Content-Type: multipart/mixed;boundary=alpaca-boundary\r\n\r\n");

	socket.write(message);
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
	message.append("Content-Type: text/plain\r\n");
	message.append(QByteArray::fromStdString(body));
	message.append("\r\n--alpaca-boundary\r\n");

	socket.write(message);
	socket.flush();

	if (_mimeFiles.empty() == false)
	{
		for (auto& filePath : _mimeFiles)
		{
			QFile file(QString::fromStdString(filePath));

			if (file.exists() && file.open(QIODevice::ReadOnly) == true)
			{
				std::string fileName = QFileInfo(file).fileName().toStdString();

				message = QByteArray::fromStdString("Content-Type: " + contentType(fileName) + "\r\nContent-Disposition: attachment; filename=" + fileName + "\r\nMIME-Version: 1.0\r\nContent-Transfer-Encoding: base64\r\n\r\n");
				socket.write(message);
				socket.flush();

				message = file.readAll().toBase64();

				int idx = 0;
				while (true)
				{
					QByteArray contentData = message.mid(76*idx, 76);

					if (contentData.isEmpty() == false)
					{
						socket.write(contentData);
						socket.write("\r\n");
						socket.flush();
					}
					else
						break;

					idx += 1;
				}
			}
			else
			{
				//TODO: Add to application log
			}

			message = "\r\n\r\n--alpaca-boundary\r\n";
			socket.write(message);
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
	bool result {false};

	QRegularExpression re("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\\b", QRegularExpression::CaseInsensitiveOption);

	QRegularExpressionMatch match = re.match(QString::fromStdString(mailAddress));
	if (match.hasMatch())
		result = true;

	return result;
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
