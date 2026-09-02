// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef PSOCEDITORVIEW_H
#define PSOCEDITORVIEW_H

#include "ui_PSOCEditorView.h"
#include "EditorView.h"

#include "PSOCPlatformConfiguration.h"

// Qt
#include <QHeaderView>
#include <QWidget>

struct ColumnResizeSpec
{
	int							column;
	int							width;	// -1 => resizeColumnToContents
	QHeaderView::ResizeMode		mode;
};

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

	void onI2CEnableCheckChanged(bool newState);
	void onI2CInvertCheckChanged(bool newState);
	void onI2CGroupChanged(QString newText);
	void onI2CTabsChanged(QString newText);
	void onI2CTableItemChanged(QTableWidgetItem* twi);

protected:
	virtual bool read();

protected slots:
	void onCustomContextMenuRequested(const QPoint &pos);

private:
	void clearRow();
	void resetToDefault();
	void setupColumnProperties();
	QTableWidget* createI2CTableFromTemplate(QWidget* parent);
	void populateI2CTable(QTableWidget* table, const QList<PSOCI2CData>& entries);
	void applyColumnResizeSpecs(QTableWidget* table, const ColumnResizeSpec* specs, int count);

	_PSOCPlatformConfiguration*          _psocPlatformConfig{Q_NULLPTR};
	QList<QTableWidget*>                 _slaveI2CTables;
};

#endif // PSOCEDITORVIEW_H
