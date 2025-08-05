#ifndef STRINGPROOF_H
#define STRINGPROOF_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include <QByteArray>

void initializeStringProof();

QByteArray licenseCheckFailed();
QByteArray licenseManagerFailed();

 #endif // STRINGPROOF_H
