include(${CMAKE_CURRENT_SOURCE_DIR}/Common.cmake)

set(LIBRARY_NAME QCommonConsole)

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
    ConsoleInterface.h
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
    LicenseFailEvent.cpp LicenseFailEvent.h
    Notification.h
    PinID.h
    PlatformConfiguration.cpp PlatformConfiguration.h
    PlatformConfigurationException.cpp PlatformConfigurationException.h
    PlatformID.cpp PlatformID.h
    PreferencesBase.cpp PreferencesBase.h
    ProtocolInterface.cpp ProtocolInterface.h
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
    StringProof.cpp StringProof.h
    StringUtilities.cpp StringUtilities.h
    SystemInformation.h
    SystemInformationWin.cpp
    SystemUsername.cpp SystemUsername.h
    TACCommand.cpp TACCommand.h
    TACCommands.cpp TACCommands.h
    TACPreferences.cpp TACPreferences.h
    Tabs.cpp Tabs.h
    TacCommandHashes.h
    TacException.h
    ThreadedLog.cpp ThreadedLog.h
    Throttle.h
    TickCount.cpp TickCount.h
    USBDescriptors.cpp USBDescriptors.h
    mymemcpy.cpp mymemcpy.h
    private/TACDriveThread.cpp private/TACDriveThread.h
    private/TACLiteCoder.cpp private/TACLiteCoder.h
    private/TACLiteCommand.cpp private/TACLiteCommand.h
    private/TACLiteDriveThread.cpp private/TACLiteDriveThread.h
    private/TACLiteProtocol.cpp private/TACLiteProtocol.h
    version.h
)

set_target_properties(${LIBRARY_NAME} PROPERTIES
    VERSION ALPACA_VERSION
)

QCommonConsoleSettings(${LIBRARY_NAME})

target_compile_definitions(${LIBRARY_NAME} PUBLIC
    QCOMMONCONSOLE_LIBRARY
    QTAC_STATIC
    QT_DISABLE_DEPRECATED_UP_TO=0x060600
    $<$<CONFIG:Debug>:DEBUG;_DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG;_NDEBUG>
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

qt_add_resources(${LIBRARY_NAME}
    resources/QCommonConsole.qrc
)
