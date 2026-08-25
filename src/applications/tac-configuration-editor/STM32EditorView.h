#ifndef STM32EDITORVIEW_H
#define STM32EDITORVIEW_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "EditorView.h"

#include "ui_STM32EditorView.h"

class _STM32PlatformConfiguration;

class STM32EditorView :
	public EditorView,
	private Ui::STM32EditorView
{
	Q_OBJECT
public:
	explicit STM32EditorView(QWidget *parent = Q_NULLPTR);
	~STM32EditorView();

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

	_STM32PlatformConfiguration*			_stm32PlatformConfig{Q_NULLPTR};
};

#endif // STM32EDITORVIEW_H
