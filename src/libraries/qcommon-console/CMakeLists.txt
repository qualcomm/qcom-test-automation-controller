qt_add_library(qcommon-console
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
    Hexify.h
    KeyState.h
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
target_include_directories(qcommon-console PUBLIC
    ../../../__Builds//ui/QCommon
)

target_compile_definitions(qcommon-console PUBLIC
    QCOMMONCONSOLE_LIBRARY
    QTAC_STATIC
    QT_DISABLE_DEPRECATED_UP_TO=0x060600
)

target_link_libraries(qcommon-console PUBLIC
    Qt::Concurrent
    Qt::Core
    Qt::SerialPort
    Qt::Xml
)


# Resources:
set(qcommon-console_resource_files
    "resources/about.txt"
)

qt_add_resources(qcommon-console "qcommon-console"
    PREFIX
        "/"
    BASE
        "resources"
    FILES
        ${qcommon-console_resource_files}
)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_definitions(qcommon-console PUBLIC
        DEBUG
        _DEBUG
    )
endif()

if(NOT CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_definitions(qcommon-console PUBLIC
        NDEBUG
        _NDEBUG
    )
endif()

if(WIN32)
    target_compile_definitions(qcommon-console PUBLIC
        FTD2XX_STATIC
    )
endif()

if((CONFIG(x32)) AND (WIN32))
    target_compile_definitions(qcommon-console PUBLIC
        _WIN32
    )
endif()

if((CONFIG(x32)) AND (LINUX))
    target_compile_definitions(qcommon-console PUBLIC
        __i386__
    )
endif()

if((NOT (CONFIG(x32))) AND (LINUX))
    target_compile_definitions(qcommon-console PUBLIC
        __X86_64__
    )
endif()

if(CMAKE_BUILD_TYPE STREQUAL Debug AND WIN32)
    target_compile_options(qcommon-console
        /Zi
    )
endif()

if(UNIX)
    target_compile_options(qcommon-console
        -Werror
        -Wno-comment
        -Wno-conversion-null
        -Wno-reorder
        -Wno-unknown-pragmas
        -Wno-unused-function
        -Wno-unused-result
        -Wno-write-strings
        -std=c++0x
    )
endif()

install(TARGETS qcommon-console
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
