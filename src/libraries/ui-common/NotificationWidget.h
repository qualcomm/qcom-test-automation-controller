#ifndef NOTIFICATIONWIDGET_H
#define NOTIFICATIONWIDGET_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
