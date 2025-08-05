#ifndef QCOMMONCONSOLEGLOBAL_H
#define QCOMMONCONSOLEGLOBAL_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#ifndef QTAC_STATIC
	#if defined(QCOMMON_LIBRARY)
		#define QCOMMON_EXPORT Q_DECL_EXPORT
	#else
		#define QCOMMON_EXPORT Q_DECL_IMPORT
	#endif
#else
	#define QCOMMON_EXPORT
#endif

#endif // QCOMMONGLOBAL_H
