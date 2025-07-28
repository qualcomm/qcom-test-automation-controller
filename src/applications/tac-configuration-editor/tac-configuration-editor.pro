# Confidential and Proprietary – Qualcomm Technologies, Inc.

# NO PUBLIC DISCLOSURE PERMITTED:  Please report postings of this software on public servers or websites
# to: DocCtrlAgent@qualcomm.com.

# RESTRICTED USE AND DISCLOSURE:
# This software contains confidential and proprietary information and is not to be used, copied, reproduced, modified
# or distributed, in whole or in part, nor its contents revealed in any manner, without the express written permission
# of Qualcomm Technologies, Inc.

# Qualcomm is a trademark of QUALCOMM Incorporated, registered in the United States and other countries. All
# QUALCOMM Incorporated trademarks are used with permission.

# This software may be subject to U.S. and international export, re-export, or transfer laws.  Diversion contrary to U.S.
# and international law is strictly prohibited.

# Qualcomm Technologies, Inc.
# 5775 Morehouse Drive
# San Diego, CA 92121 U.S.A.
# Copyright ©2023 Qualcomm Technologies, Inc.
# All rights reserved.
# Qualcomm Technologies Confidential and Proprietary

# Author: msimpson, biswroy

include(../../libraries/qcommon/Common.pri)

QT += core gui widgets serialport network xml

VERSION = $$TAC_CONFIG_VERSION
TARGET = TACConfigEditor
TEMPLATE = app
RC_ICONS = $$PWD/Resources/TACConfigEditor.ico

DESTDIR = $$BINPATH

INCLUDEPATH += $$QCOMMON
INCLUDEPATH += $$QCOMMONCONSOLE
INCLUDEPATH += $$LIBUI

DEPENDPATH += $$QCOMMON
DEPENDPATH += $$QCOMMONCONSOLE
DEPENDPATH += $$LIBUI

win32 {
	CONFIG(debug, debug|release) {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommond.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/qcommon-consoled.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/UId.lib
	} else {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommon.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/qcommon-console.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/UI.lib
	}
}

linux {
	CONFIG(debug, debug|release) {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommond.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommon-consoled.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libUId.a
	} else {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommon.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommon-console.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libUI.a
	}
}

SOURCES += \
	ButtonEditor.cpp \
	CodeEditor.cpp \
	ConfigEditorApplication.cpp \
	ConfigWindow.cpp \
	CreateConfigurationDialog.cpp \
	EditorView.cpp \
	FTDIEditorView.cpp \
	ManageTabsDialog.cpp \
	PineCommandLine.cpp \
	TACPreviewWindow.cpp \
	TabEditorDialog.cpp \
	main.cpp

HEADERS += \
	ButtonEditor.h \
	CodeEditor.h \
	ConfigEditorApplication.h \
	ConfigWindow.h \
	CreateConfigurationDialog.h \
	EditorView.h \
	FTDIEditorView.h \
	ManageTabsDialog.h \
	PineCommandLine.h \
	TACPreviewWindow.h \
	TabEditorDialog.h

FORMS += \
	ButtonEditor.ui \
	CodeEditor.ui \
	ConfigWindow.ui \
	CreateConfigurationDialog.ui \
	FTDIEditorView.ui \
	ManageTabsDialog.ui \
	TACPreviewWindow.ui \
	TabEditorDialog.ui

CONFIG(debug, debug|release) {
	LIBS += -L$$STATIC_LIBPATH -lUId -lQCommond -lQCommon-consoled
} else {
	LIBS += -L$$STATIC_LIBPATH -lUI -lQCommon -lQCommon-console
}

LIBS += -L$$STATIC_LIBPATH -lftd2xx

unix {
	DISTFILES += \
		../LinuxDeploy/TACConfigEditor.desktop

	LIBS += -rdynamic
	linux {
		LIBS += -L/usr/local/lib

		LIBS += -lrt
		LIBS += -ldl
		LIBS += -luuid
	}
	mac {
		QMAKE_LFLAGS += -F/System/Library/Frameworks/CoreFoundation.framework
		QMAKE_LFLAGS += -F/System/Library/Frameworks/IOKit.framework
		QMAKE_LFLAGS += -framework Carbon
		ICON = $$PWD/resources/qxdm.icns
		LIBS += -framework CoreFoundation
		LIBS += -framework IOKit
		LIBS += -lobjc

		QMAKE_POST_LINK = $$PWD/debug_plugin.sh $$TARGET $$BINPATH
	}
}

RESOURCES += \
	resources/TACConfigEditor.qrc
