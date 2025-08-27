include(${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/qcommon-console/Common.cmake)

set(APP_NAME UpdateDeviceList)

qt_add_executable(${APP_NAME}
    FTDITemplateCompiler.cpp FTDITemplateCompiler.h
    UDLCommandLine.cpp UDLCommandLine.h
    UpdateDeviceList.cpp UpdateDeviceList.h
    main.cpp
)

set_target_properties(${APP_NAME} PROPERTIES
    VERSION UPDATE_DEVICE_LIST_VERSION
)

target_include_directories(${APP_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src/libraries/qcommon-console
    ${CMAKE_SOURCE_DIR}/src/libraries/qcommon
)

target_link_directories(${APP_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/__Builds/x64/$<CONFIG>/lib
)

target_link_libraries(${APP_NAME} PRIVATE
    Qt::Core
    Qt::SerialPort
    Qt::Xml
    $<$<CONFIG:Debug>:QCommonConsole>
    # TODO: Compile debug binaries
    # $<$<CONFIG:Debug>:QCommonConsoled>
    # $<$<NOT:$<CONFIG:Debug>>:QCommonConsole>
)

if(WIN32)
    target_compile_options(${APP_NAME} PRIVATE
        $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/Zi>
    )
endif()

if(LINUX)
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
