#ifndef PSOCEDITORVIEW_H
#define PSOCEDITORVIEW_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "ui_PSOCEditorView.h"
#include "EditorView.h"

// libTAC
#include "PSOCPlatformConfiguration.h"

// QT
#include <QWidget>

class PSOCEditorView :
	public EditorView,
	public Ui::PSOCEditorView
{
Q_OBJECT

public:
	explicit PSOCEditorView(QWidget *parent = Q_NULLPTR);
	~PSOCEditorView();

	virtual void setPlatformConfiguration(PlatformConfiguration platformConfiguration);
	virtual void resetPlatform();

	void setRowEnabled(int rowId, bool state);

private slots:
	void onEnableCheckChanged(bool newState);
	void onInitialPinValueChanged(bool newState);
	void onInvertCheckChanged(bool newState);
	void onGroupChanged(QString newText);
	void onTabsChanged(QString newText);
	void onTableItemChanged(QTableWidgetItem* twi);

protected:
	virtual bool read();

protected slots:
	void onCustomContextMenuRequested(const QPoint &pos);

private:
	void setupColumnProperties();
	void clearRow();
	void resetToDefault();

	_PSOCPlatformConfiguration*          _psocPlatformConfig{Q_NULLPTR};
};

#endif // PSOCEDITORVIEW_H
