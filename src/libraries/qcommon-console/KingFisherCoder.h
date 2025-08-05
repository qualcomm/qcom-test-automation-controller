#ifndef KINGFISHERCODER_H
#define KINGFISHERCODER_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
