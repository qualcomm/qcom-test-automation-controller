#ifndef TERMINALWIDGET_H
#define TERMINALWIDGET_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "UIGlobalLib.h"

// QCommonconsole
#include "FramePackage.h"

namespace Ui
{
	class TerminalWidget;
}

// Qt
#include <QWidget>

class UILIB_EXPORT TerminalWidget : public QWidget
{
	Q_OBJECT

public:
	TerminalWidget(QWidget* parent = Q_NULLPTR);
	~TerminalWidget();

public slots:
	void on_consoleMessage(const QList<QByteArray>& messages);

private:
	void addConsoleText(const QByteArray& consoleText);
	void handleConsoleResponse(const FramePackage& framePackage);

	Ui::TerminalWidget*			_ui{Q_NULLPTR};

signals:
	void consoleMessagesReady(const QList<QByteArray>& consoleMessages);
};

#endif // TERMINALWIDGET_H
