// Confidential and Proprietary Qualcomm Technologies, Inc.

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
// Copyright 2013-2018 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
