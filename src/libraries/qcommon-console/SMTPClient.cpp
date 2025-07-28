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
// Copyright 2013-2024 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

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

QString SMTPClient::getHost()
{
	return _hostName;
}

void SMTPClient::setHost(const QByteArray& hostName)
{
	_hostName = hostName;
}

quint16 SMTPClient::getPort()
{
	return _portNumber;
}

void SMTPClient::setPort(const quint16 portNumber)
{
	_portNumber = portNumber;
}

quint16 SMTPClient::getTimeout()
{
	return _timeout;
}

void SMTPClient::setTimeout(const quint16 timeout)
{
	_timeout = timeout;
}

void SMTPClient::setMimeFiles(const QByteArrayList& mimeFiles)
{
	_mimeFiles = mimeFiles;
}

void SMTPClient::send(const QByteArray& senderAddress, const QByteArray& receiverAddress, const QByteArray& subject, const QByteArray& body)
{
	if (isMailAddressValid(senderAddress) == false || isMailAddressValid(receiverAddress) == false)
		throw SMTPClientException("The supplied email address is invalid. Please use a valid email address.");

	QTcpSocket socket;

	socket.connectToHost(_hostName, _portNumber);

	if (socket.waitForConnected(_timeout) == false)
		throw SMTPClientException(socket.errorString());

	if (socket.isValid() == false)
		throw SMTPClientException("The TCP Socket is invalid.");

	socket.write("EHLO 22.03.19.98\r\n");
	socket.flush();

	if (socket.waitForReadyRead())
		AppCore::writeToApplicationLogLine(socket.readAll());

	socket.write("mail from: " + senderAddress + "\r\n");
	socket.flush();

	if (socket.waitForReadyRead())
		AppCore::writeToApplicationLogLine(socket.readAll());

	socket.write("rcpt to: " + receiverAddress + " NOTIFY=success,failure\r\n");
	socket.flush();

	if (socket.waitForReadyRead())
		AppCore::writeToApplicationLogLine(socket.readAll());

	socket.write("data\r\n");
	socket.flush();

	if (socket.waitForReadyRead())
		AppCore::writeToApplicationLogLine(socket.readAll());

	socket.write("Subject: " + subject + "\r\n");
	socket.flush();

	QByteArray message;

	message = "FROM: " + senderAddress + "\r\n";
	message.append("TO: " + receiverAddress + "\r\n");

	for (auto& ccUser : _ccAddresses)
	{
		message.append("CC: " + ccUser + "\r\n");
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
	message.append(body);
	message.append("\r\n--alpaca-boundary\r\n");

	socket.write(message);
	socket.flush();

	if (_mimeFiles.isEmpty() == false)
	{
		for (auto& filePath : _mimeFiles)
		{
			QFile file(filePath);

			if (file.exists() && file.open(QIODevice::ReadOnly) == true)
			{                
				QByteArray fileName = QFileInfo(file).fileName().toLatin1();

				message = "Content-Type: " + contentType(fileName) + "\r\nContent-Disposition: attachment; filename=" + fileName + "\r\nMIME-Version: 1.0\r\nContent-Transfer-Encoding: base64\r\n\r\n";
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
		AppCore::writeToApplicationLogLine(socket.readAll());

	socket.write(".\r\nQUIT\r\n");
	socket.flush();

	if (socket.waitForReadyRead())
		AppCore::writeToApplicationLogLine(socket.readAll());

	socket.disconnectFromHost();
	socket.close();
}

bool SMTPClient::isMailAddressValid(const QByteArray& mailAddress)
{
	bool result {false};

	QRegularExpression re("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\\b", QRegularExpression::CaseInsensitiveOption);

	QRegularExpressionMatch match = re.match(mailAddress);
	if (match.hasMatch())
		result = true;

	return result;
}

QByteArray SMTPClient::contentType(const QByteArray &fileName)
{
	QByteArray result{"application/octet-stream"};

	if (fileName.endsWith(".png"))
		result = "image/png";

	else if (fileName.endsWith(".txt"))
		result = "text/plain";

	return result;
}
