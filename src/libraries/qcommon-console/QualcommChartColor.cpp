// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "QualcommChartColor.h"
#include "QualcommColors.h"

#include <QVector>

QVector<BasicColor> gChartColors;

const int kChartColorCount(34);

BasicColor qualcommChartColor(uint chartColorIndex)
{
	BasicColor result;

	if (gChartColors.isEmpty())
	{
		gChartColors.resize(kChartColorCount);

		gChartColors[0] = kPacific;
		gChartColors[1] = kSky;
		gChartColors[2] = kAtlantic;
		gChartColors[3] = kSpring;
		gChartColors[4] = kMint;
		gChartColors[5] = kTropical;
		gChartColors[6] = kAlgae;
		gChartColors[7] = kJungle;
		gChartColors[8] = kPine;
		gChartColors[9] = kRain;
		gChartColors[10] = kGrass;
		gChartColors[11] = kCarribean;
		gChartColors[12] = kSailor;
		gChartColors[13] = kGreen;
		gChartColors[14] = kMediterranean;
		gChartColors[15] = kRoyal;
		gChartColors[16] = kStormy;
		gChartColors[17] = kForest;
		gChartColors[18] = kHoney;
		gChartColors[19] = kMustard;
		gChartColors[20] = kYellow;
		gChartColors[21] = kGold;
		gChartColors[22] = kBrass;
		gChartColors[23] = kOrange;
		gChartColors[24] = kDahlia;
		gChartColors[25] = kCeramic;
		gChartColors[26] = kRedwood;
		gChartColors[27] = kCherry;
		gChartColors[28] = kRose;
		gChartColors[29] = kMagenta;
		gChartColors[30] = kImperial;
		gChartColors[31] = kSoil;
		gChartColors[32] = kBurgundy;
		gChartColors[33] = kGrape;
	}

	result = gChartColors[chartColorIndex % kChartColorCount];

	return result;
}
