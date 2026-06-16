# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

include(${CMAKE_SOURCE_DIR}/src/libraries/qcommon-console/Common.cmake)

set(APP_NAME UpdateDeviceList)

qt_add_executable(${APP_NAME}
    UDLCommandLine.cpp UDLCommandLine.h
    UpdateDeviceList.cpp UpdateDeviceList.h
    main.cpp
)

target_include_directories(${APP_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src/libraries/qcommon-console
    ${CMAKE_SOURCE_DIR}/src/libraries/qcommon
)

target_link_directories(${APP_NAME} PRIVATE
    ${STATIC_LIBPATH}
)

target_link_libraries(${APP_NAME} PRIVATE
    Qt6::Core
    Qt6::SerialPort
    Qt6::Xml

    $<$<CONFIG:Debug>:QCommonConsoled>
    $<$<NOT:$<CONFIG:Debug>>:QCommonConsole>
)

if(WIN32)
    target_compile_options(${APP_NAME} PRIVATE
        $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/Zi>
    )
endif()

if(UNIX AND NOT APPLE)
    target_link_libraries(${APP_NAME} PRIVATE
        dl
    )
endif()

install(TARGETS ${APP_NAME}
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET ${APP_NAME}
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
