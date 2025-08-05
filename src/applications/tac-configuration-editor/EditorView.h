#ifndef EDITORVIEW_H
#define EDITORVIEW_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

// libTAC
#include "Tabs.h"
#include "PlatformConfiguration.h"

// QT
#include <QComboBox>
#include <QFrame>
#include <QTableWidgetItem>
#include <QWidget>

const QByteArray kHash(QByteArrayLiteral("hash"));
const QByteArray kChipIndex(QByteArrayLiteral("chipIndex"));

const QString kDefaultTabString(QStringLiteral("General"));

const QString kDefaultPinLabel(QStringLiteral("<type a label name>"));
const QString kDefaultPinTooltip(QStringLiteral("<add a tooltip>"));
const QString kDefaultPinCommand(QStringLiteral("<type a command>"));
const QString kDefaultPinPriority(QStringLiteral("-1"));
const QString kDefaultClassicAction(QStringLiteral("<type an action>"));
const QString kDefaultCellLocation(QStringLiteral("-1,-1"));

class EditorView:
	public QWidget
{
	Q_OBJECT
public:
	EditorView(QWidget* parent = Q_NULLPTR);
	~EditorView();

	virtual void setPlatformConfiguration(PlatformConfiguration platformConfiguration);
	virtual void resetPlatform() = 0;

protected:
	virtual bool read();
	QString sanitizeText(const QString& inputText);

	Tabs						_configurableTabs;

private:
	PlatformConfiguration		_platformConfiguration;
};

#endif // EDITORVIEW_H
