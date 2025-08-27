qt_add_executable(LITEProgrammer
    ProgrammerCommandLine.cpp ProgrammerCommandLine.h
    main.cpp
)

target_include_directories(LITEProgrammer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ../../../libraries/qcommon-console
)

target_compile_definitions(LITEProgrammer PRIVATE
    QTAC_STATIC
    QT_DISABLE_DEPRECATED_UP_TO=0x060600
    $<$<CONFIG:Debug>:DEBUG;_DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG;_NDEBUG>
)

target_link_libraries(LITEProgrammer PRIVATE
    Qt::Core
    Qt::SerialPort
    ftd2xx
    # Config-specific libs (preserving your original names)
    $<$<CONFIG:Debug>:QcommonConsoled>
    $<$<NOT:$<CONFIG:Debug>>:QcommonConsole>
)

target_compile_options(LITEProgrammer PRIVATE
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/Zi>
)

if(UNIX)
    target_compile_options(LITEProgrammer PRIVATE
        -Wno-comment
        -Wno-conversion-null
        -Wno-reorder
        -Wno-unknown-pragmas
        -Wno-unused-function
        -Wno-unused-result
        -Wno-write-strings
    )
endif()

install(TARGETS LITEProgrammer
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET LITEProgrammer
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
