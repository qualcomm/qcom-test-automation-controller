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
