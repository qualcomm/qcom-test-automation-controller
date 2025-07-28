// Confidential and Proprietary – Qualcomm Technologies, Inc.

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
// Copyright 2013-2021 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
