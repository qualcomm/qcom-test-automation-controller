#ifndef EDITORVIEW_H
#define EDITORVIEW_H
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
// Copyright 2022 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
