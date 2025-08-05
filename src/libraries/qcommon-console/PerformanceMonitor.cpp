// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: msimpson
*/

#include "PerformanceMonitor.h"

#include <QtCore/QtAlgorithms>
#include <QtCore/QDebug>
#include <QtCore/QMap>
#include <QtCore/QMutex>
#include <QtCore/QString>

static QMap<QString, quint64> gPerformanceStats;
static QMutex gMutex;

class PerformancePrinter
{
public:
	PerformancePrinter() {};
	~PerformancePrinter()
	{
		qDebug() << "Performance Report";

		QList<QString> keys = gPerformanceStats.keys();

		qSort(keys);

		QList<QString>::iterator key = keys.begin();
		while (key != keys.end())
		{
			qDebug() << "   " << *key << ": " << (gPerformanceStats[*key] / 1000);
			key++;
		}
	}

} PerformancePrinter;

PerformanceMonitor::~PerformanceMonitor()
{
	quint64 elapsed = ::tickCount() - _tickCount;
	QString key = QString("%1:%2:(%3)").arg(_file).arg(_function).arg(_line);

	QMutexLocker lock(&gMutex);

	QMap<QString, quint64>::iterator statIter = gPerformanceStats.find(key);
	if (statIter == gPerformanceStats.end())
		gPerformanceStats[key] = elapsed;
	else
		statIter.value() += elapsed;
}
