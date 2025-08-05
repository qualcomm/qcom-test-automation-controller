// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "ui_HoverAwareQWindow.h"
#include "HoverAwareQWindow.h"
#include "ColorConversion.h"

#include <QClipboard>
#include <QCursor>
#include <QListWidget>
#include <QMenu>
#include <QPropertyAnimation>


const quint32 kNoticeTime(3000);
const quint16 kMaxNotifications(4);
const QSize kLabelSize(kNotificationLabelWidth, kNotificationLabelHeight);


HoverAwareQWindow::HoverAwareQWindow(QWidget* parent, Qt::WindowFlags flags):
	QMainWindow(parent, flags),
	_ui(new Ui::HoverAwareQWindow)
{
	_ui->setupUi(this);
	setWindowFlags(Qt::Popup | Qt::Window | Qt::FramelessWindowHint);
	_ui->_notificationListContainer->setLayoutMode(QListView::Batched);

	connect(&_timer, &QTimer::timeout, this, &HoverAwareQWindow::onTimerTimeout);
	connect(this, &HoverAwareQWindow::clearAll, this, &HoverAwareQWindow::onNotificationCleared);

	_timer.setTimerType(Qt::VeryCoarseTimer);
	_timer.setInterval(kNoticeTime);
	_timer.setSingleShot(true);
}

HoverAwareQWindow::~HoverAwareQWindow()
{
	delete _ui;
}

void HoverAwareQWindow::insertNotification(const QString &message, const NotificationLevel notificationLevel)
{
	Notification newNotice(message, notificationLevel);
	_notifications.insert(0, newNotice);
}

void HoverAwareQWindow::setWindowLocation(const QSize& windowSize, const QPoint& windowLoc)
{
	// the bottom right position of the main window
	int mainWindowXPos = windowSize.width() + windowLoc.x();
	int mainWindowYPos = windowSize.height() + windowLoc.y();

	buildListView();

	_ui->_notificationListContainer->setFixedSize(362, kNotificationLabelHeight*maxNotificationView());

	int x = mainWindowXPos - 380;
	int y = mainWindowYPos - kNotificationLabelHeight*maxNotificationView() - 60;

	QPoint loc{0,0};

	loc.setX(x);
	loc.setY(y);
	move(loc);

	setupNotificationTimer();
	show();
}

void HoverAwareQWindow::onTimerTimeout()
{
	QPoint pos = QCursor::pos();

	if (geometry().contains(pos) == false)
	{
		if (_winAnim == Q_NULLPTR)
		{
			_winAnim = new QPropertyAnimation(this, "windowOpacity");

			connect(_winAnim, &QPropertyAnimation::finished, this, &HoverAwareQWindow::fadeOutAnimComplete);

			_winAnim->setDuration(kNoticeTime);
			_winAnim->setStartValue(1.0);
			_winAnim->setEndValue(0.0);

			_winAnim->setEasingCurve(QEasingCurve::OutBack);
			_winAnim->start(QPropertyAnimation::DeleteWhenStopped);
		}

		if (_winAnim != Q_NULLPTR)
			_winAnim->start(QPropertyAnimation::DeleteWhenStopped);
	}
	else
		setupNotificationTimer();
}

void HoverAwareQWindow::buildListView()
{
	quint16 notificationCount = _notifications.size();

	if (notificationCount > 0)
	{
		for (const Notification& notification : _notifications)
		{
			QListWidgetItem* lwi = new QListWidgetItem(_ui->_notificationListContainer);
			lwi->setText(notification.getMessage());
			lwi->setSizeHint(kLabelSize);

			QColor labelColor = ColorConversion::getLabelColor(notification.getLevel());

			QBrush brushColor(labelColor);
			lwi->setBackground(brushColor);

			_ui->_notificationListContainer->addItem(lwi);
		}
	}
}

void HoverAwareQWindow::onNotificationCleared()
{
	_notifications.clear();
	clearFrame();
	hide();
}

void HoverAwareQWindow::fadeOutAnimComplete()
{
	hide();
	clearFrame();
	setWindowOpacity(1.0);

	delete _winAnim;
	_winAnim = Q_NULLPTR;
}

void HoverAwareQWindow::setupNotificationTimer()
{
	if (_timer.isActive() == false)
		_timer.start();
}

quint16 HoverAwareQWindow::maxNotificationView()
{
	if (_notifications.size() < kMaxNotifications)
		return _notifications.size();
	else
		return kMaxNotifications;
}

void HoverAwareQWindow::clearFrame()
{
	QListWidgetItem* wgt{Q_NULLPTR};

	while ((wgt=_ui->_notificationListContainer->takeItem(0)) != Q_NULLPTR)
		delete wgt;

	_ui->_notificationListContainer->clear();
	resize(kNotificationLabelWidth, kNotificationLabelHeight);
}

void HoverAwareQWindow::on__clearAllLabel_linkActivated(const QString &link)
{
	Q_UNUSED(link);
	emit clearAll();
}


void HoverAwareQWindow::on__notificationListContainer_customContextMenuRequested(const QPoint &pos)
{
	QListWidget* lw = qobject_cast<QListWidget*>(sender());

	if (lw != Q_NULLPTR)
	{
		QListWidgetItem* lwi = lw->itemAt(pos);

		if (lwi)
		{
			QMenu menu;
			QAction* copyAction = menu.addAction("Copy text");
			QAction* result = menu.exec(lw->mapToGlobal(pos));

			if (result != Q_NULLPTR)
			{
				if (result == copyAction)
				{
					QClipboard *clipboard = QGuiApplication::clipboard();
					if (clipboard != Q_NULLPTR)
					{
						clipboard->setText(lwi->text());
					}
				}
			}
		}
	}
}
