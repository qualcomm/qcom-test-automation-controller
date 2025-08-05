# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries. 
#  
# Redistribution and use in source and binary forms, with or without
# modification, are permitted (subject to the limitations in the
# disclaimer below) provided that the following conditions are met:
#  
# 	* Redistributions of source code must retain the above copyright
# 	  notice, this list of conditions and the following disclaimer.
#  
# 	* Redistributions in binary form must reproduce the above
# 	  copyright notice, this list of conditions and the following
# 	  disclaimer in the documentation and/or other materials provided
# 	  with the distribution.
#  
# 	* Neither the name of Qualcomm Technologies, Inc. nor the names of its
# 	  contributors may be used to endorse or promote products derived
# 	  from this software without specific prior written permission.
#  
# NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
# GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
# HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
# GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
# IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
# IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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

DEPENDPATH += $$SERIAL
DEPENDPATH += $$ALPACALIB

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
	CharBit.h \
	CommandGroup.h \
	CommandLineParser.h \
	ConsoleApplicationEnhancements.h \
	ConsoleInterface.h \
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
	LicenseFailEvent.h \
	LicenseManager.h \
	Notification.h \
	PinID.h \
	PlatformConfiguration.h \
	PlatformConfigurationException.h \
	PlatformID.h \
	PreferencesBase.h \
	PreferencesBase.h \
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
	CharBit.cpp \
	CommandLineParser.cpp \
	ConsoleApplicationEnhancements.cpp \
	DebugBoardType.cpp \
	DriveThread.cpp \
	EncryptedString.cpp \
	FTDIChipset.cpp \
	FTDIDevice.cpp \
	FTDIPlatformConfiguration.cpp \
	FrameCoder.cpp \
	LicenseFailEvent.cpp \
	LicenseManager.cpp \
	PlatformConfiguration.cpp \
	PlatformConfigurationException.cpp \
	PlatformID.cpp \
	PreferencesBase.cpp \
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
