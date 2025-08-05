#ifndef NOTIFICATIONWIDGET_H
#define NOTIFICATIONWIDGET_H

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
