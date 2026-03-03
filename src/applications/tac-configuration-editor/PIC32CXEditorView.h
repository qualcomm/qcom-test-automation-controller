#ifndef PIC32CXEditorView_H
#define PIC32CXEditorView_H
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
// Copyright 2024 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

// Author:	Biswajit Roy <biswroy@qti.qualcomm.com>

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
