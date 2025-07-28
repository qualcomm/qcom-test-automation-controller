#ifndef KINGFISHERCODER_H
#define KINGFISHERCODER_H
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
// Copyright 2013-2021 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// QCommon
#include "FrameCoder.h"

// Qt
#include <QByteArray>

class KingFisherCoder :
	public FrameCoder
{
public:
	KingFisherCoder();
	virtual ~KingFisherCoder();

	void Reset(void);

	virtual void Decode(const QByteArray& decodeMe);
	virtual QByteArray Encode(const QByteArray& encodeMe)
	{
		return QByteArray();
	}
	static QByteArray EncodeFrame(const QByteArray& encodeMe)
	{
		return QByteArray();
	}

private:
	Q_DISABLE_COPY(KingFisherCoder)

	enum ParserState
	{
		eAA1,
		eAA2,
		eAA3,
		eAB,
		eCollect
	} _parserState;
	
	void HandleStateAA1(char decodeChar);
	void HandleStateAA2(char decodeChar);
	void HandleStateAA3(char decodeChar);
	void HandleStateAB(char decodeChar);
	void HandleStateCollect(char decodeChar);

	void CompleteTheFrame(const QByteArray& debugPortion);
};

#endif // KINGFISHERCODER_H
