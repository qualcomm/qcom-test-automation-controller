#ifndef COMMANDBUTTON_H
#define COMMANDBUTTON_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

// Author: msimpson

#include <QPushButton>

class CommandButton :
	public QPushButton
{
Q_OBJECT

public:
	CommandButton(QWidget *parent = Q_NULLPTR);

	void setCommand(const QString& command)
	{
		_command = command;
	}

signals:
	void commandTriggered(const QString& command);

private slots:
	void on_released();

private:
	QString						_command;
};

#endif // COMMANDBUTTON_H
