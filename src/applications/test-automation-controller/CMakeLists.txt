# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

include(${CMAKE_SOURCE_DIR}/src/libraries/qcommon/Common.cmake)

set(APP_NAME TAC)

qt_add_resources(TAC_RESOURCES
    resources/TAC.qrc
)

qt_add_executable(${APP_NAME} WIN32
    PreferencesDialog.cpp PreferencesDialog.h PreferencesDialog.ui
    TACApplication.cpp TACApplication.h
    TACDeviceSelection.cpp TACDeviceSelection.h TACDeviceSelection.ui
    TACWindow.cpp TACWindow.h TACWindow.ui
    main.cpp

    ${TAC_RESOURCES}
)

if(WIN32)
    set_target_properties(${APP_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
    target_sources(${APP_NAME} PRIVATE resources/TAC.rc)
elseif(UNIX AND NOT APPLE)
    install(FILES resources/TAC.desktop DESTINATION share/applications)
    install(FILES resources/TAC.png DESTINATION share/icons/hicolor/48x48/apps RENAME tac.png)
endif()

target_include_directories(${APP_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src/libraries/qcommon-console
    ${CMAKE_SOURCE_DIR}/src/libraries/qcommon
    ${CMAKE_SOURCE_DIR}/src/libraries/ui-common
)

target_link_directories(${APP_NAME} PRIVATE
    ${STATIC_LIBPATH}
)

target_compile_definitions(${APP_NAME} PRIVATE
    QTAC_STATIC
    QT_DEPRECATED_WARNINGS
    QT_DISABLE_DEPRECATED_UP_TO=0x060900
    $<$<CONFIG:Debug>:DEBUG;_DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG;_NDEBUG>
)

target_link_libraries(${APP_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Multimedia
    Qt6::MultimediaWidgets
    Qt6::SerialPort
    Qt6::Widgets
    ftd2xx

    $<$<CONFIG:Debug>:UId;QCommond;QCommonConsoled>
    $<$<NOT:$<CONFIG:Debug>>:UI;QCommon;QCommonConsole>
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

if(UNIX AND NOT APPLE)
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

if(WIN32 AND WINDEPLOYQT_EXECUTABLE)
    add_custom_command(TARGET ${APP_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Deploying Qt libraries..."
        COMMAND ${WINDEPLOYQT_EXECUTABLE} --no-translations --compiler-runtime --dir $<TARGET_FILE_DIR:${APP_NAME}> $<TARGET_FILE:${APP_NAME}>
        COMMENT "Running windeployqt..."
    )
endif()
