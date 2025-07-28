#ifndef HOVERAWAREQWINDOW_H
#define HOVERAWAREQWINDOW_H

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

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "Notification.h"

#include <QPropertyAnimation>
#include <QMainWindow>
#include <QTimer>


namespace Ui {
	class HoverAwareQWindow;
} // namespace Ui


class HoverAwareQWindow : public QMainWindow
{
	Q_OBJECT
public:
	HoverAwareQWindow(QWidget *parent = Q_NULLPTR, Qt::WindowFlags flags = Qt::FramelessWindowHint);
	~HoverAwareQWindow();

	void insertNotification(const QString& message, const NotificationLevel notificationLevel = eInfoNotification);

	void setWindowLocation(const QSize &windowSize, const QPoint &windowLoc);

public slots:
	void onTimerTimeout();
	void onNotificationCleared();

private slots:
	void fadeOutAnimComplete();
	void on__clearAllLabel_linkActivated(const QString &link);

	void on__notificationListContainer_customContextMenuRequested(const QPoint &pos);

signals:
	void clearAll();

private:
	void setupNotificationTimer();
	quint16 maxNotificationView();

	void buildListView();
	void clearFrame();

	Ui::HoverAwareQWindow*      _ui{Q_NULLPTR};
	QList<Notification>         _notifications;
	QTimer                      _timer;
	QPropertyAnimation*         _winAnim{Q_NULLPTR};
};

#endif // HOVERAWAREQWINDOW_H
