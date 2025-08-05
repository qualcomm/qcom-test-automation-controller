#ifndef HOVERAWAREQWINDOW_H
#define HOVERAWAREQWINDOW_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
