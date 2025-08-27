qt_add_executable(TACDump
    main.cpp
)

target_include_directories(TACDump PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ../../libraries/qcommon-console
)

target_compile_definitions(TACDump PRIVATE
    QTAC_STATIC
    QT_DISABLE_DEPRECATED_UP_TO=0x060600
    $<$<CONFIG:Debug>:DEBUG;_DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG;_NDEBUG>
)

target_link_libraries(TACDump PRIVATE
    Qt::Core
    Qt::SerialPort
    $<$<CONFIG:Debug>:QcommonConsoled;ftd2xx>
    $<$<NOT:$<CONFIG:Debug>>:QcommonConsole;ftd2xx>
)

if(WIN32)
    target_link_libraries(TACDump PRIVATE
        SetupAPI
        Winusb
    )

    target_compile_options(TACDump PRIVATE
        $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/Zi>
    )
endif()

if(UNIX)
    target_compile_options(TACDump PRIVATE
        -Wno-comment
        -Wno-conversion-null
        -Wno-reorder
        -Wno-unknown-pragmas
        -Wno-unused-function
        -Wno-unused-result
        -Wno-write-strings
    )
endif()

install(TARGETS TACDump
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET TACDump
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
