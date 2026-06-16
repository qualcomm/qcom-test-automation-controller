#ifndef TACPREVIEWWINDOW_H
#define TACPREVIEWWINDOW_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

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
	TACPreviewWindow(QWidget* parent = Q_NULLPTR);
	~TACPreviewWindow();

	void setPlatformConfiguration(PlatformConfiguration platformConfiguration);

signals:
	void startNotification(const QString& message, NotificationLevel level);

protected:
	 virtual void resizeEvent(QResizeEvent* event);

private:
	void onNotificationStarted(const QString &message, NotificationLevel level);

	PlatformConfiguration          			_platformConfig;
};

#endif // TACPREVIEWWINDOW_H
