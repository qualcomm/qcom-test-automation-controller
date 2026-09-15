// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "ui_HoverAwareQWindow.h"
#include "HoverAwareQWindow.h"
#include "ColorConversion.h"

#include <QClipboard>
#include <QCursor>
#include <QGraphicsDropShadowEffect>
#include <QListWidget>
#include <QMenu>
#include <QPropertyAnimation>

const quint32 kNoticeTime(3000);
const quint16 kMaxNotifications(4);
const quint16 kMaxNotificationHistory(100);
const QSize kLabelSize(kNotificationLabelWidth, kNotificationLabelHeight);


HoverAwareQWindow::HoverAwareQWindow(QWidget* parent):
	QMainWindow(parent),
	_ui(new Ui::HoverAwareQWindow)
{
	_ui->setupUi(this);

	QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(_ui->_centralWgt);
	shadow->setBlurRadius(24);
	shadow->setXOffset(0);
	shadow->setYOffset(4);
	shadow->setColor(QColor(0, 0, 0, 90));
	_ui->_centralWgt->setGraphicsEffect(shadow);

	connect(&_timer, &QTimer::timeout, this, &HoverAwareQWindow::onTimerTimeout);
	connect(this, &HoverAwareQWindow::clearAll, this, &HoverAwareQWindow::onNotificationCleared);

	_timer.setTimerType(Qt::VeryCoarseTimer);
	_timer.setInterval(kNoticeTime);
	_timer.setSingleShot(true);

	_winAnim = new QPropertyAnimation(this, "windowOpacity", this);
	_winAnim->setDuration(kNoticeTime);
	_winAnim->setStartValue(1.0);
	_winAnim->setEndValue(0.0);
	_winAnim->setEasingCurve(QEasingCurve::OutBack);
	connect(_winAnim, &QPropertyAnimation::finished, this, &HoverAwareQWindow::fadeOutAnimComplete);
}

HoverAwareQWindow::~HoverAwareQWindow()
{
	delete _ui;
}

void HoverAwareQWindow::insertNotification(const QString &message, const NotificationLevel notificationLevel)
{
	for (int i = 0; i < _notifications.size(); ++i)
	{
		if (_notifications.at(i).getLevel() == notificationLevel && _notifications.at(i).getMessage() == message)
		{
			Notification existing = _notifications.at(i);
			existing.addOccurrence();

			_notifications.removeAt(i);
			_notifications.insert(0, existing);
			return;
		}
	}

	Notification newNotice(message, notificationLevel, _nextNotificationId++);
	_notifications.insert(0, newNotice);

	while (_notifications.size() > kMaxNotificationHistory)
		_notifications.removeLast();
}

void HoverAwareQWindow::setWindowLocation(const QSize& windowSize, const QPoint& windowLoc)
{
	// the bottom right position of the main window
	int mainWindowXPos = windowSize.width() + windowLoc.x();
	int mainWindowYPos = windowSize.height() + windowLoc.y();

	_winAnim->stop();
	setWindowOpacity(1.0);

	buildListView();

	_ui->_notificationListContainer->setFixedHeight(visibleListHeight());

	int x = mainWindowXPos - 380;
	int y = mainWindowYPos - visibleListHeight() - 60;

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
		_winAnim->stop();
		_winAnim->start();
	}
	else
		setupNotificationTimer();
}

void HoverAwareQWindow::buildListView()
{
	for (const Notification& notification : _notifications)
	{
		QString text = notification.getMessage();

		if (notification.getOccurrenceCount() > 1)
			text += QString(" (x%1)").arg(notification.getOccurrenceCount());

		QListWidgetItem* lwi = new QListWidgetItem(_ui->_notificationListContainer);
		lwi->setText(text);
		lwi->setSizeHint(kLabelSize);
		lwi->setData(Qt::UserRole, notification.getId());

		QColor labelColor = ColorConversion::getLabelColor(notification.getLevel());
		lwi->setBackground(QBrush(labelColor));

		_ui->_notificationListContainer->addItem(lwi);
	}
}

void HoverAwareQWindow::removeNotification(quint64 id)
{
	for (int i = 0; i < _notifications.size(); ++i)
	{
		if (_notifications.at(i).getId() == id)
		{
			_notifications.removeAt(i);
			break;
		}
	}

	clearFrame();
	buildListView();

	_ui->_notificationListContainer->setFixedHeight(visibleListHeight());
	resize(kNotificationLabelWidth, visibleListHeight() + 33);

	if (_notifications.isEmpty())
		emit clearAll();
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

int HoverAwareQWindow::visibleListHeight()
{
	quint16 visibleRows = maxNotificationView();
	int totalHeight = 0;

	for (quint16 i = 0; i < visibleRows; ++i)
		totalHeight += _ui->_notificationListContainer->sizeHintForRow(i);

	return totalHeight;
}

void HoverAwareQWindow::clearFrame()
{
	QListWidgetItem* wgt{Q_NULLPTR};

	while ((wgt=_ui->_notificationListContainer->takeItem(0)) != Q_NULLPTR)
		delete wgt;

	_ui->_notificationListContainer->clear();
	resize(kNotificationLabelWidth, kNotificationLabelHeight);
}

void HoverAwareQWindow::on__clearAllBtn_clicked()
{
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
			quint64 notificationId = lwi->data(Qt::UserRole).toULongLong();

			QMenu menu;
			QAction* copyAction = menu.addAction("Copy text");
			QAction* removeAction = menu.addAction("Remove");
			QAction* result = menu.exec(lw->mapToGlobal(pos));

			if (result != Q_NULLPTR)
			{
				if (result == copyAction)
				{
					for (const Notification& notification : _notifications)
					{
						if (notification.getId() == notificationId)
						{
							QClipboard* clipboard = QGuiApplication::clipboard();
							if (clipboard != Q_NULLPTR)
								clipboard->setText(notification.getMessage());

							break;
						}
					}
				}
				else if (result == removeAction)
				{
					removeNotification(notificationId);
				}
			}
		}
	}
}
