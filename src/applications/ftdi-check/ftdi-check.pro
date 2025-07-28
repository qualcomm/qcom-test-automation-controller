# Qualcomm is a trademark of QUALCOMM Incorporated, registered in the United States and other countries. All
# QUALCOMM Incorporated trademarks are used with permission.

# This software may be subject to U.S. and international export, re-export, or transfer laws.  Diversion contrary to U.S.
# and international law is strictly prohibited.

# Qualcomm Technologies, Inc.
# 5775 Morehouse Drive
# San Diego, CA 92121 U.S.A.
# Copyright © 2023 Qualcomm Technologies, Inc.
# All rights reserved.
# Qualcomm Technologies Confidential and Proprietary
#
# Author: Biswajit Roy <biswroy@qti.qualcomm.com>
#
include(../../libraries/qcommon-console/Common.pri)

VERSION = $$ALPACA_VERSION

TARGET = FTDICheck

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
DESTDIR = $$BINPATH

RC_ICONS = $$PWD/resources/ftdi-check.ico

# for version info
INCLUDEPATH += $$QCOMMONCONSOLE
DEPENDPATH += $$QCOMMONCONSOLE

win32 {
	CONFIG(debug, debug|release) {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/qcommon-consoled.lib
	}
	else {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/qcommon-console.lib
	}
}

linux {
	CONFIG(debug, debug|release) {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommon-consoled.a
	}
	else {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommon-console.a
	}
}

SOURCES += \
	FTDICheckApplication.cpp \
	FTDICheckCommandLine.cpp \
	main.cpp

HEADERS += \
	FTDICheckApplication.h \
	FTDICheckCommandLine.h

CONFIG(debug, debug|release) {
	LIBS += -L$$STATIC_LIBPATH -lQCommon-consoled
} else {
	LIBS += -L$$STATIC_LIBPATH -lQCommon-console
}

win32 {
	LIBS += -lSetupAPI -lWinusb -lOle32 -lAdvapi32 -lShell32 -lWtsapi32 -lVersion -lPsapi
}

linux {
	LIBS += -ldl
}
