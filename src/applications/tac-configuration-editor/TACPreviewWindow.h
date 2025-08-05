#ifndef TACPREVIEWWINDOW_H
#define TACPREVIEWWINDOW_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "ui_TACPreviewWindow.h"

// libTAC
#include "PlatformConfiguration.h"

// QT
#include <QWidget>


class TACPreviewWindow :
	public QWidget,
	private Ui::TACPreview
{
Q_OBJECT
public:
	TACPreviewWindow(PlatformConfiguration platformConfiguration, QWidget* parent = Q_NULLPTR);
	~TACPreviewWindow();

protected:
	 virtual void resizeEvent(QResizeEvent* event);

private:
    PlatformConfiguration          			_platformConfig;
};

#endif // TACPREVIEWWINDOW_H
