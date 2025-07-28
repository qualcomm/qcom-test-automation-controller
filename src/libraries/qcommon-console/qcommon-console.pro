#-------------------------------------------------
#
# Project created by QtCreator 2018-08-30T09:50:56  <<< --- Wow, this is the day I started the QTAC project
#
#-------------------------------------------------

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
# Copyright ©2018 Qualcomm Technologies, Inc.
# All rights reserved.
# Qualcomm Technologies Confidential and Proprietary

# Author: msimpson, biswroy

include(Common.pri)

QT += xml serialport concurrent network

CONFIG(debug, debug|release) {
        TARGET = QCommon-consoled
} else {
        TARGET = QCommon-console
}

TEMPLATE = lib
CONFIG += staticlib
DESTDIR = $$STATIC_LIBPATH
DEFINES += QCOMMONCONSOLE_LIBRARY

INCLUDEPATH += $$SERIAL
INCLUDEPATH += $$FTDIINC
INCLUDEPATH += $$LIBUSBINC

DEPENDPATH += $$SERIAL
DEPENDPATH += $$ALPACALIB
DEPENDPATH += $$LIBUSBINC

win32 {
	DEFINES += FTD2XX_STATIC
}

linux {
	INCLUDEPATH += $$LINUXFTDI
}

HEADERS += \
	AlpacaDefines.h \
	AlpacaDevice.h \
	AlpacaScript.h \
	AlpacaSettings.h \
	AlpacaSharedLibrary.h \
	AppCore.h \
	BasicColor.h \
	Button.h \
	ByteCountFormatter.h \
	CSV.h \
	CharBit.h \
	CommandGroup.h \
	CommandLineParser.h \
	ConsoleApplicationEnhancements.h \
	ConsoleInterface.h \
	CountingStream.h \
	DataPoints.h \
	DateCheckFailEvent.h \
	DebugBoardType.h \
	DriveThread.h \
	EncryptedString.h \
	ErrorParameter.h \
	FTDIChipset.h \
	FTDIDevice.h \
	FTDIPinSet.h \
	FTDIPlatformConfiguration.h \
	FrameCoder.h \
	FramePackage.h \
	Hexify.h \
	KeyState.h \
	LibUsbEngine.h \
	LibUsbInitializer.h \
	LicenseFailEvent.h \
	LicenseManager.h \
	LogInterface.h \
	Notification.h \
	PinID.h \
	PlatformConfiguration.h \
	PlatformConfigurationException.h \
	PlatformID.h \
	PluginInterface.h \
	PreferencesBase.h \
	PreferencesBase.h \
	ProcessUtilities.h \
	ProtocolInterface.h \
	QCommonConsole.h \
	QCommonConsoleGlobal.h \
	QualcommChartColor.h \
	QualcommColors.h \
	QuitAppEvent.h \
	Range.h \
	RangedContainer.h \
	ReceiveInterface.h \
	RecentFiles.h \
	SMTPClient.h \
	ScriptVariable.h \
	SendInterface.h \
	SerialPort.h \
	StringProof.h \
	StringUtilities.h \
	SubSystemInterface.h \
	SystemInformation.h \
	SystemUsername.h \
	TACCommand.h \
	TACCommands.h \
	TACPreferences.h \
	Tabs.h \
	TacCommandHashes.h \
	TacException.h \
	ThreadedLog.h \
	Throttle.h \
	TickCount.h \
	USBDescriptors.h \
	mymemcpy.h \
	private/TACDriveThread.h \
	private/TACLiteCoder.h \
	private/TACLiteCommand.h \
	private/TACLiteDriveThread.h \
	private/TACLiteProtocol.h \
	version.h

SOURCES += \
	AlpacaDefines.cpp \
	AlpacaDevice.cpp \
	AlpacaScript.cpp \
	AlpacaSettings.cpp \
	AlpacaSharedLibrary.cpp \
	AppCore.cpp \
	BasicColor.cpp \
	Button.cpp \
	CSV.cpp \
	CharBit.cpp \
	CommandLineParser.cpp \
	ConsoleApplicationEnhancements.cpp \
	CountingStream.cpp \
	DebugBoardType.cpp \
	DriveThread.cpp \
	EncryptedString.cpp \
	FTDIChipset.cpp \
	FTDIDevice.cpp \
	FTDIPlatformConfiguration.cpp \
	FrameCoder.cpp \
	LibUsbEngine.cpp \
	LibUsbInitializer.cpp \
	LicenseFailEvent.cpp \
	LicenseManager.cpp \
	PlatformConfiguration.cpp \
	PlatformConfigurationException.cpp \
	PlatformID.cpp \
	PreferencesBase.cpp \
	ProcessUtilities.cpp \
	ProtocolInterface.cpp \
	QCommonConsole.cpp \
	QualcommChartColor.cpp \
	QuitAppEvent.cpp \
	RecentFiles.cpp \
	SMTPClient.cpp \
	ScriptVariable.cpp \
	SerialPort.cpp \
	StringProof.cpp \
	StringUtilities.cpp \
	SystemInformationWin.cpp \
	SystemUsername.cpp \
	TACCommand.cpp \
	TACCommands.cpp \
	TACPreferences.cpp \
	Tabs.cpp \
	ThreadedLog.cpp \
	TickCount.cpp \
	USBDescriptors.cpp \
	mymemcpy.cpp \
	private/TACDriveThread.cpp \
	private/TACLiteCoder.cpp \
	private/TACLiteCommand.cpp \
	private/TACLiteDriveThread.cpp \
	private/TACLiteProtocol.cpp


RESOURCES += \
	resources/qcommon-console.qrc
