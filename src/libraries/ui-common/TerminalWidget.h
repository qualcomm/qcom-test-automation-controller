#ifndef TERMINALWIDGET_H
#define TERMINALWIDGET_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

// Author: msimpson

#include "UIGlobalLib.h"

namespace Ui
{
	class TerminalWidget;
}

// QCommon
#include "ConsoleInterface.h"

// QT
#include <QWidget>

class UILIB_EXPORT TerminalWidget :
	public QWidget,
	private ConsoleInterface
{
	Q_OBJECT

public:
	TerminalWidget(QWidget* parent = Q_NULLPTR);
	~TerminalWidget();

public slots:
	void on_consoleMessage(const QList<QByteArray>& messages);

private:
	// ConsoleInterface
	virtual void addConsoleText(const QByteArray& consoleText);
	virtual void handleConsoleResponse(const FramePackage& framePackage);

	Ui::TerminalWidget*			_ui{Q_NULLPTR};

signals:
	void consoleMessagesReady(const QList<QByteArray>& consoleMessages);
};

#endif // TERMINALWIDGET_H
