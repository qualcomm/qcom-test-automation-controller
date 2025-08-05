#ifndef RECEIVEINTERFACE_H
#define RECEIVEINTERFACE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// QCommon
#include "FramePackage.h"
#include "QCommonConsoleGlobal.h"

class QCOMMONCONSOLE_EXPORT ReceiveInterface
{
public:
	ReceiveInterface() {}
	virtual ~ReceiveInterface() {}

	virtual void receive(FramePackage& framePackage) = 0;
};

#endif // RECEIVEINTERFACE_H

