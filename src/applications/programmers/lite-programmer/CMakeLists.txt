qt_add_executable(LITEProgrammer
    ProgrammerCommandLine.cpp ProgrammerCommandLine.h
    main.cpp
)
target_include_directories(LITEProgrammer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ../../../../src/libraries/qcommon-console/../../../__Builds//ui/QCommon
)

target_compile_definitions(LITEProgrammer PRIVATE
    QTAC_STATIC
    QT_DISABLE_DEPRECATED_UP_TO=0x060600
)

target_link_libraries(LITEProgrammer PRIVATE
    # Remove: L${CMAKE_CURRENT_SOURCE_DIR}/../../../__Builds//lib
    Qt::Core
    Qt::SerialPort
    ftd2xx
)

if(CMAKE_BUILD_TYPE STREQUAL Debug AND WIN32)
    target_compile_options(LITEProgrammer
        /Zi
    )
endif()

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_definitions(LITEProgrammer PRIVATE
        DEBUG
        _DEBUG
    )

    target_link_libraries(LITEProgrammer PRIVATE
        # Remove: L${CMAKE_CURRENT_SOURCE_DIR}/../../../__Builds//lib
        QCommon-consoled
    )
endif()

if(NOT CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_definitions(LITEProgrammer PRIVATE
        NDEBUG
        _NDEBUG
    )

    target_link_libraries(LITEProgrammer PRIVATE
        # Remove: L${CMAKE_CURRENT_SOURCE_DIR}/../../../__Builds//lib
        QCommon-console
    )
endif()

if((CONFIG(x32)) AND (WIN32))
    target_compile_definitions(LITEProgrammer PRIVATE
        _WIN32
    )
endif()

if((CONFIG(x32)) AND (LINUX))
    target_compile_definitions(LITEProgrammer PRIVATE
        __i386__
    )
endif()

if((NOT (CONFIG(x32))) AND (LINUX))
    target_compile_definitions(LITEProgrammer PRIVATE
        __X86_64__
    )
endif()

if(UNIX)
    target_compile_options(LITEProgrammer
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

install(TARGETS LITEProgrammer
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET LITEProgrammer
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
