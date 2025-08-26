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
    QT_DISABLE_DEPRECATED_BEFORE=0x060000
)

target_link_libraries(TAC PRIVATE
    # Remove: L${CMAKE_CURRENT_SOURCE_DIR}/../../../__Builds//lib
    Qt::Core
    Qt::Gui
    Qt::SerialPort
    Qt::Widgets
    ftd2xx
)


# Resources:
set(TAC_resource_files
    "resources/About.txt"
    "resources/BugWriter.png"
    "resources/NotificationBellRing.png"
    "resources/NotificationBellSilent.png"
    "resources/TAC.bmp"
    "resources/TAC.png"
    "resources/TAC_BS.png"
    "resources/Warning.drawio.png"
)

qt_add_resources(TAC "TAC"
    PREFIX
        "/"
    BASE
        "resources"
    FILES
        ${TAC_resource_files}
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

    target_compile_options(TAC
        /Zi
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

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_definitions(TAC PRIVATE
        DEBUG
        _DEBUG
    )

    target_link_libraries(TAC PRIVATE
        # Remove: L${CMAKE_CURRENT_SOURCE_DIR}/../../../__Builds//lib
        QCommon-consoled
        QCommond
        UId
    )
endif()

if(NOT CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_definitions(TAC PRIVATE
        NDEBUG
        _NDEBUG
    )

    target_link_libraries(TAC PRIVATE
        # Remove: L${CMAKE_CURRENT_SOURCE_DIR}/../../../__Builds//lib
        QCommon
        QCommon-console
        UI
    )
endif()

if(UNIX)
    target_link_libraries(TAC PRIVATE
        # Remove: rdynamic
    )

    target_compile_options(TAC
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

if(APPLE)
    target_link_libraries(TAC PRIVATE
        "-framework CoreFoundation"
        "-framework IOKit"
        objc
    )
endif()

if((CONFIG(x32)) AND (WIN32))
    target_compile_definitions(TAC PRIVATE
        _WIN32
    )
endif()

if((CONFIG(x32)) AND (LINUX))
    target_compile_definitions(TAC PRIVATE
        __i386__
    )
endif()

if((NOT (CONFIG(x32))) AND (LINUX))
    target_compile_definitions(TAC PRIVATE
        __X86_64__
    )
endif()

install(TARGETS TAC
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET TAC
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
