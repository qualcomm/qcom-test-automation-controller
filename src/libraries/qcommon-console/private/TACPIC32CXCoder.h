#ifndef TACPIC32CXCODER_H
#define TACPIC32CXCODER_H

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
// Copyright 2024 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "QCommonConsoleGlobal.h"

// QCommon
#include "FrameCoder.h"

// Qt
#include <QByteArray>
#include <QVariant>

const QByteArray kPIC32CXCommandError("Error!!! port >");
const QByteArray kPIC32CXCommandNotRecognized("*** Command Processor: unknown command. ***");
const quint8 kValidPIC32CXResponseSize(40);


class QCOMMONCONSOLE_EXPORT TACPIC32CXCoder :
	public FrameCoder
{
public:
	TACPIC32CXCoder();
	virtual ~TACPIC32CXCoder();

	virtual void reset();

	virtual void decode(const QByteArray& decodeMe);

	virtual QByteArray encode(const QByteArray& encodeMe, const Arguments& arguments);

private:
	Q_DISABLE_COPY(TACPIC32CXCoder)
	QByteArray					_recieveBuffer;
};

#endif // TACPIC32CXCODER_H
