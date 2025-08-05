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
