qt_add_executable(TACDump
    main.cpp
)
target_include_directories(TACDump PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ../../libraries/qcommon-console/../../../__Builds//ui/QCommon
)

target_compile_definitions(TACDump PRIVATE
    QTAC_STATIC
    QT_DISABLE_DEPRECATED_UP_TO=0x060600
)

target_link_libraries(TACDump PRIVATE
    Qt::Core
    Qt::SerialPort
)

if(WIN32)
    target_link_libraries(TACDump PRIVATE
        SetupAPI
        Winusb
    )
endif()

if(CMAKE_BUILD_TYPE STREQUAL Debug AND WIN32)
    target_compile_options(TACDump
        /Zi
    )
endif()

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_definitions(TACDump PRIVATE
        DEBUG
        _DEBUG
    )

    target_link_libraries(TACDump PRIVATE
        # Remove: L${CMAKE_CURRENT_SOURCE_DIR}/../../../__Builds//lib
        QCommon-consoled
        ftd2xx
    )
endif()

if(NOT CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_definitions(TACDump PRIVATE
        NDEBUG
        _NDEBUG
    )

    target_link_libraries(TACDump PRIVATE
        # Remove: L${CMAKE_CURRENT_SOURCE_DIR}/../../../__Builds//lib
        QCommon-console
        ftd2xx
    )
endif()

if((CONFIG(x32)) AND (WIN32))
    target_compile_definitions(TACDump PRIVATE
        _WIN32
    )
endif()

if((CONFIG(x32)) AND (LINUX))
    target_compile_definitions(TACDump PRIVATE
        __i386__
    )
endif()

if((NOT (CONFIG(x32))) AND (LINUX))
    target_compile_definitions(TACDump PRIVATE
        __X86_64__
    )
endif()

if(UNIX)
    target_compile_options(TACDump
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

install(TARGETS TACDump
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET TACDump
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
