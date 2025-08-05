#ifndef HOVERAWAREQWINDOW_H
#define HOVERAWAREQWINDOW_H

/*
	Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries. 
	 
	Redistribution and use in source and binary forms, with or without
	modification, are permitted (subject to the limitations in the
	disclaimer below) provided that the following conditions are met:
	 
		* Redistributions of source code must retain the above copyright
		  notice, this list of conditions and the following disclaimer.
	 
		* Redistributions in binary form must reproduce the above
		  copyright notice, this list of conditions and the following
		  disclaimer in the documentation and/or other materials provided
		  with the distribution.
	 
		* Neither the name of Qualcomm Technologies, Inc. nor the names of its
		  contributors may be used to endorse or promote products derived
		  from this software without specific prior written permission.
	 
	NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
	GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
	HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
	IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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
