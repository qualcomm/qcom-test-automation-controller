// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef PIC32CXEditorView_H
#define PIC32CXEditorView_H

#include "ui_PIC32CXEditorView.h"
#include "EditorView.h"

#include "PIC32CXPlatformConfiguration.h"

// Qt
#include <QWidget>

class PIC32CXEditorView :
	public EditorView,
	private Ui::PIC32CXEditorView
{
	Q_OBJECT

public:
	explicit PIC32CXEditorView(QWidget *parent = Q_NULLPTR);
	~PIC32CXEditorView();

	virtual void setPlatformConfiguration(PlatformConfiguration platformConfiguration);
	virtual void resetPlatform();

protected:
	virtual bool read();

private slots:
	void onCustomContextMenuRequested();
	void onEnableCheckChanged(bool newState);
	void onInvertCheckChanged(bool newState);
	void onGroupChanged(const QString& newText);
	void onTabsChanged(const QString& newText);
	void onTableItemChanged(QTableWidgetItem *twi);

private:
	void setupColumnProperties();

	_PIC32CXPlatformConfiguration*          _pic32cxPlatformConfig{Q_NULLPTR};
};

#endif // PIC32CXEditorView_H
