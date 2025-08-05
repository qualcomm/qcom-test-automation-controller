#ifndef QCOMMONGLOBAL_H
#define QCOMMONGLOBAL_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#ifndef QTAC_STATIC
	#if defined(QCOMMONCONSOLE_LIBRARY)
		#define QCOMMONCONSOLE_EXPORT Q_DECL_EXPORT
	#else
		#define QCOMMONCONSOLE_EXPORT Q_DECL_IMPORT
	#endif
#else
	#define QCOMMONCONSOLE_EXPORT
#endif

#endif // QCOMMONGLOBAL_H
