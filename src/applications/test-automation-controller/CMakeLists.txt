qt_add_executable(TAC WIN32 MACOSX_BUNDLE
    PreferencesDialog.cpp PreferencesDialog.h PreferencesDialog.ui
    TACApplication.cpp TACApplication.h
    TACDeviceSelection.cpp TACDeviceSelection.h TACDeviceSelection.ui
    TACWindow.cpp TACWindow.h TACWindow.ui
    main.cpp
)

target_include_directories(TAC PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ../../libraries/qcommon/../../../__Builds//ui/QCommon
    ../qcommon-console
    ../ui-common
)

target_compile_definitions(TAC PRIVATE
    QTAC_STATIC
    QT_DEPRECATED_WARNINGS
    QT_DISABLE_DEPRECATED_UP_TO=0x060600
    $<$<CONFIG:Debug>:DEBUG;_DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG;_NDEBUG>
)

target_link_libraries(TAC PRIVATE
    # Remove: L${CMAKE_CURRENT_SOURCE_DIR}/../../../__Builds//lib
    Qt::Core
    Qt::Gui
    Qt::SerialPort
    Qt::Widgets
    ftd2xx
    $<$<CONFIG:Debug>:QcommonConsoled;QCommond;UId>
    $<$<NOT:$<CONFIG:Debug>>:QCommon;QCommon-console;UI>
)

qt_add_resources(TAC "TAC"
    PREFIX "/"
    BASE "resources"
    FILES ${TAC_resource_files}
)

set(TAC_resource_files
    "About.txt"
    "BugWriter.png"
    "NotificationBellRing.png"
    "NotificationBellSilent.png"
    "TAC.bmp"
    "TAC.png"
    "Warning.drawio.png"
)

if(WIN32)
    target_link_libraries(TAC PRIVATE
        Advapi32
        Ole32
        SetupAPI
        Shell32
        Winusb
        Wtsapi32
    )

    target_compile_options(TAC PRIVATE
        $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/Zi>
    )
endif()

if(LINUX)
    target_link_libraries(TAC PRIVATE
        # Remove: L/usr/local/lib
        dl
        rt
        uuid
    )
endif()

if(UNIX)
    target_link_libraries(TAC PRIVATE
        # Remove: rdynamic
    )
    target_compile_options(TAC PRIVATE
        -Wno-comment
        -Wno-conversion-null
        -Wno-reorder
        -Wno-unknown-pragmas
        -Wno-unused-function
        -Wno-unused-result
        -Wno-write-strings
    )
endif()

if(APPLE)
    target_link_libraries(TAC PRIVATE
        "-framework CoreFoundation"
        "-framework IOKit"
        objc
    )
endif()

install(TARGETS TAC
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET TAC
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
