#ifndef PAINTEREFFECTS_H
#define PAINTEREFFECTS_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Austin Simpson (austinsimpson.dev)
*/

#include "QCommonGlobal.h"

#include <QPainter>
#include <QPoint>
#include <QString>

void QCOMMON_EXPORT paintCenterRotatedText(QPainter& painter, const QPointF& center, const QString& text, qreal theta);
void QCOMMON_EXPORT paintLeftRotatedText(QPainter& painter, const QPointF& anchor, const QString& text, qreal theta);
void QCOMMON_EXPORT paintRightRotatedText(QPainter& painter, const QPointF& anchor, const QString& text, qreal theta);

#endif // PAINTEREFFECTS_H
