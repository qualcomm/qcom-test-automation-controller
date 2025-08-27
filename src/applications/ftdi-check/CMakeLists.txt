qt_add_executable(FTDICheck
    FTDICheckApplication.cpp FTDICheckApplication.h
    FTDICheckCommandLine.cpp FTDICheckCommandLine.h
    main.cpp
)

target_include_directories(FTDICheck PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ../../libraries/qcommon-console
)

target_compile_definitions(FTDICheck PRIVATE
    QTAC_STATIC
    QT_DISABLE_DEPRECATED_UP_TO=0x060600
    $<$<CONFIG:Debug>:DEBUG;_DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG;_NDEBUG>
)

target_link_libraries(FTDICheck PRIVATE
    Qt::Core
    # Config-specific libs (preserving your original naming)
    $<$<CONFIG:Debug>:QcommonConsoled>
    $<$<NOT:$<CONFIG:Debug>>:QcommonConsole>
)

if(WIN32)
    target_link_libraries(FTDICheck PRIVATE
        Advapi32
        Ole32
        Psapi
        SetupAPI
        Shell32
        Version
        Winusb
        Wtsapi32
    )

    target_compile_options(FTDICheck PRIVATE
        $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/Zi>
    )
endif()

if(LINUX)
    target_link_libraries(FTDICheck PRIVATE
        dl
    )
endif()

if(UNIX)
    target_compile_options(FTDICheck PRIVATE
        -Wno-comment
        -Wno-conversion-null
        -Wno-reorder
        -Wno-unknown-pragmas
        -Wno-unused-function
        -Wno-unused-result
        -Wno-write-strings
    )
endif()

install(TARGETS FTDICheck
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET FTDICheck
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
