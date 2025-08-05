# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause-Clear

# Author: msimpson, biswroy

include(../qcommon/Common.pri)

QT += core gui widgets serialport

TEMPLATE = lib
CONFIG += staticlib
DESTDIR = $$STATIC_LIBPATH

INCLUDEPATH += $$QCOMMON
INCLUDEPATH += $$QCOMMONCONSOLE

DEPENDPATH += $$QCOMMON
DEPENDPATH += $$QCOMMONCONSOLE
DEPENDPATH += $$LIBEPM

CONFIG(debug, debug|release) {
	TARGET = UId
} else {
	TARGET = UI
}

win32 {
	CONFIG(debug, debug|release) {
	        PRE_TARGETDEPS += $$STATIC_LIBPATH/qcommond.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/qcommon-consoled.lib
	} else {
	        PRE_TARGETDEPS += $$STATIC_LIBPATH/qcommon.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/qcommon-console.lib
	}
}

linux {

	CONFIG(debug, debug|release) {
	        PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommond.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommon-consoled.a
	} else {
	        PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommon.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommon-console.a
	}
}

SOURCES += \
	ColorConversion.cpp \
	CommandButton.cpp \
	CustomValidator.cpp \
	HoverAwareQWindow.cpp \
	I2CWidget.cpp \
	NotificationWidget.cpp \
	TACFrame.cpp \
	TerminalWidget.cpp \
	UserWidget.cpp \
	UILib.cpp \
	VariableInput.cpp

HEADERS += \
	ColorConversion.h \
	CommandButton.h \
	CustomValidator.h \
	HoverAwareQWindow.h \
	I2CWidget.h \
	NotificationWidget.h \
	TACFrame.h \
	TerminalWidget.h \
	UserWidget.h \
	UILib.h \
	UIGlobalLib.h \
	VariableInput.h

RESOURCES += \
	resources/ui-common.qrc

FORMS += \
	HoverAwareQWindow.ui \
	I2CWidget.ui \
	NotificationWidget.ui \
	TACFrame.ui \
	TerminalWidget.ui \
	UserWidget.ui \
	VariableInput.ui
