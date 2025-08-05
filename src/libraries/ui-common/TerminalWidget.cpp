// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
