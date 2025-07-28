// Confidential and Proprietary – Qualcomm Technologies, Inc.

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
// Copyright ©2022 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

// Author: msimpson

#include "TerminalWidget.h"

#include "ui_TerminalWidget.h"

// Qt
#include <QScrollBar>

TerminalWidget::TerminalWidget
(
	QWidget* parent
) :
	QWidget(parent),
	_ui(new Ui::TerminalWidget)
{
	_ui->setupUi(this);

	// Get messages out of the worker thread to the main thread for UI
	connect(this, &TerminalWidget::consoleMessagesReady, this, &TerminalWidget::on_consoleMessage);
}

TerminalWidget::~TerminalWidget()
{
	delete _ui;
}

void TerminalWidget::addConsoleText
(
	const QByteArray& consoleText
)
{
	_ui->_commandText->append(consoleText);
	_ui->_commandText->append(" ");
}

void TerminalWidget::on_consoleMessage
(
	const QList<QByteArray>& messages
)
{
	auto response = messages.begin();
	while (response != messages.end())
	{
		QByteArray message = (*response).trimmed();
		if (message.isEmpty() == false)
			_ui->_commandText->append(message);
		response++;
	}

	_ui->_commandText->append(" ");
	_ui->_commandText->verticalScrollBar()->setValue(_ui->_commandText->verticalScrollBar()->maximum());
}

void TerminalWidget::handleConsoleResponse
(
	const FramePackage& framePackage
)
{
	emit consoleMessagesReady(framePackage->_responses);
}
