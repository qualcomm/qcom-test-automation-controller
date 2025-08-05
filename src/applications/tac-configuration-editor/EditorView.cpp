// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
