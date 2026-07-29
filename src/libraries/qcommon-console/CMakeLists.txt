# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

include(${CMAKE_CURRENT_SOURCE_DIR}/Common.cmake)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(LIBRARY_NAME QCommonConsoled)
else()
    set(LIBRARY_NAME QCommonConsole)
endif()

qt_add_resources(QCOMMONCONSOLE_RESOURCES
    resources/QCCResource.qrc
)

qt_add_library(${LIBRARY_NAME} STATIC
    AlpacaDefines.cpp AlpacaDefines.h
    AlpacaDevice.cpp AlpacaDevice.h
    AlpacaScript.cpp AlpacaScript.h
    AlpacaSettings.cpp AlpacaSettings.h
    AlpacaSharedLibrary.cpp AlpacaSharedLibrary.h
    AppCore.cpp AppCore.h
    BasicColor.cpp BasicColor.h
    Button.cpp Button.h
    CharBit.cpp CharBit.h
    CommandGroup.h
    CommandLineParser.cpp CommandLineParser.h
    ConsoleApplicationEnhancements.cpp ConsoleApplicationEnhancements.h
    DateCheckFailEvent.h
    DebugBoardType.cpp DebugBoardType.h
    DriveThread.cpp DriveThread.h
    EncryptedString.cpp EncryptedString.h
    ErrorParameter.h
    FTDIChipset.cpp FTDIChipset.h
    FTDIDevice.cpp FTDIDevice.h
    FTDIPinSet.h
    FTDIPlatformConfiguration.cpp FTDIPlatformConfiguration.h
    FrameCoder.cpp FrameCoder.h
    FramePackage.h
    Notification.h
    PinID.h
    PlatformConfiguration.cpp PlatformConfiguration.h
    PlatformConfigurationException.cpp PlatformConfigurationException.h
    PlatformID.cpp PlatformID.h
    PreferencesBase.cpp PreferencesBase.h
    ProtocolInterface.cpp ProtocolInterface.h
    PIC32CXDevice.h PIC32CXDevice.cpp
	PIC32CXPlatformConfiguration.h PIC32CXPlatformConfiguration.cpp
	PIC32CXSerialTableModel.h PIC32CXSerialTableModel.cpp
	PSOCDevice.h PSOCDevice.cpp
	PSOCPlatformConfiguration.h PSOCPlatformConfiguration.cpp
	PSOCSerialTableModel.h PSOCSerialTableModel.cpp
    QCommonConsole.cpp QCommonConsole.h
    QCommonConsoleGlobal.h
    QualcommChartColor.cpp QualcommChartColor.h
    QualcommColors.h
    QuitAppEvent.cpp QuitAppEvent.h
    Range.h
    RangedContainer.h
    ReceiveInterface.h
    RecentFiles.cpp RecentFiles.h
    SMTPClient.cpp SMTPClient.h
    ScriptVariable.cpp ScriptVariable.h
    SendInterface.h
    SerialPort.cpp SerialPort.h
    SerialPortInfo.cpp SerialPortInfo.h
    SerialTableModel.h SerialTableModel.cpp
    StringProof.cpp StringProof.h
    StringUtilities.cpp StringUtilities.h
    SystemInformation.h
    SystemInformationWin.cpp
    SystemUsername.cpp SystemUsername.h
    TACCommand.cpp TACCommand.h
    TACCommands.cpp TACCommands.h
    TACPreferences.cpp TACPreferences.h
    Tabs.cpp Tabs.h
    TACCommandHashes.h
    TACException.h
    ThreadedLog.cpp ThreadedLog.h
    TickCount.cpp TickCount.h
    USBDescriptors.cpp USBDescriptors.h
    mymemcpy.cpp mymemcpy.h
    private/TACDriveThread.cpp private/TACDriveThread.h
    private/TACLiteCoder.cpp private/TACLiteCoder.h
    private/TACLiteCommand.cpp private/TACLiteCommand.h
    private/TACLiteDriveThread.cpp private/TACLiteDriveThread.h
    private/TACLiteProtocol.cpp private/TACLiteProtocol.h
	private/TACPIC32CXCoder.h private/TACPIC32CXCoder.cpp
	private/TACPIC32CXCommand.h private/TACPIC32CXCommand.cpp
	private/TACPIC32CXDriveThread.h private/TACPIC32CXDriveThread.cpp
	private/TACPIC32CXProtocol.h private/TACPIC32CXProtocol.cpp
	private/TACPSOCCoder.h private/TACPSOCCoder.cpp
	private/TACPSOCCommand.h private/TACPSOCCommand.cpp
	private/TACPSOCDriveThread.h private/TACPSOCDriveThread.cpp
	private/TACPSOCProtocol.h private/TACPSOCProtocol.cpp
    version.h

    ${QCOMMONCONSOLE_RESOURCES}
)

set_target_properties(${LIBRARY_NAME} PROPERTIES
    VERSION ${QTAC_VERSION}
)

QCommonConsoleSettings(${LIBRARY_NAME})

target_compile_definitions(${LIBRARY_NAME} PUBLIC
    QCOMMONCONSOLE_LIBRARY
    QTAC_STATIC
    QT_DISABLE_DEPRECATED_UP_TO=0x060900
    $<$<CONFIG:Debug>:DEBUG;_DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG;_NDEBUG;RELEASE>
    $<$<PLATFORM_ID:Windows>:FTD2XX_STATIC>
)

target_link_libraries(${LIBRARY_NAME} PUBLIC
    Qt6::Concurrent
    Qt6::Core
    Qt6::Network
    Qt6::SerialPort
    Qt6::Xml
    ftd2xx
)
