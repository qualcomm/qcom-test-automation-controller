#ifndef NOTIFICATIONWIDGET_H
#define NOTIFICATIONWIDGET_H

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
// Copyright 2024-2025 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "UIGlobalLib.h"

#include "HoverAwareQWindow.h"
#include "Notification.h"

// Qt
#include <QWidget>
#include <QTimer>


namespace Ui
{
	class NotificationWidgetClass;
} // namespace Ui


class UILIB_EXPORT NotificationWidget :
	public QWidget
{
	Q_OBJECT
public:
	NotificationWidget(QWidget *parent = Q_NULLPTR);
	~NotificationWidget();

	void setSilent(bool status);
	bool isSilent();

	void insertNotification(const QString& message, const NotificationLevel notificationLevel = eInfoNotification);

signals:
	void notificationAdded(const QString& message, const NotificationLevel notificationLevel);
	void progress(const quint8 value=10, NotificationLevel level=eInfoNotification);

private slots:
	void onNotificationAdded(const QString& message, const NotificationLevel notificationLevel);
	void onNotificationCleared();
	void onNotificationButtonClicked();
	void onProgressUpdated(const quint8 newValue, NotificationLevel level);
	void onTimerTimeout();

private:
	void makeNotificationLabel(const QString &message, const NotificationLevel notificationLevel);

	Ui::NotificationWidgetClass*            _ui{Q_NULLPTR};
	HoverAwareQWindow*                      _notificationWindow{Q_NULLPTR};
	QTimer									_timer;
	bool                                    _silent;
};

#endif // NOTIFICATIONWIDGET_H
