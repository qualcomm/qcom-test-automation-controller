
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

#include "EditorView.h"

// QT
#include <QCheckBox>
#include <QHBoxLayout>
#include <QMessageBox>

EditorView::EditorView(QWidget *parent):
	QWidget(parent)
{

}

EditorView::~EditorView()
{
}

void EditorView::setPlatformConfiguration
(
	PlatformConfiguration platformConfiguration
)
{
	_platformConfiguration = platformConfiguration;
}

bool EditorView::read()
{
	bool result{false};

	if (_platformConfiguration != Q_NULLPTR)
	{
		Tabs tabs = _platformConfiguration->getTabs();

		_configurableTabs.clear();

		for (const auto& tab: tabs)
		{
			if (tab._configurable == true)
				_configurableTabs.push_back(tab);
		}

		result = true;
	}

	return result;
}

QString EditorView::sanitizeText(const QString &inputText)
{
	// TODO: Add more filters for escape sequences and unnecessary characters
	QString temp = inputText;
	temp.replace("\r", " ");
	temp.replace("\t", " ");
	temp.replace("\n", " ");
	temp.replace("  ", " ");

	return temp;
}
