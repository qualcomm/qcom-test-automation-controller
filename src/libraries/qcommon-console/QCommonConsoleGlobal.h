#ifndef QCOMMONCONSOLEGLOBAL_H
#define QCOMMONCONSOLEGLOBAL_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef QTAC_STATIC
	#if defined(_WIN32)
		#if defined(QCOMMONCONSOLE_LIBRARY)
			#define QCOMMONCONSOLE_EXPORT __declspec(dllexport)
		#else
			#define QCOMMONCONSOLE_EXPORT __declspec(dllimport)
		#endif
	#else
		#define QCOMMONCONSOLE_EXPORT __attribute__((visibility("default")))
	#endif
#else
	#define QCOMMONCONSOLE_EXPORT
#endif

#endif // QCOMMONCONSOLEGLOBAL_H
