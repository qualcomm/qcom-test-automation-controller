qt_add_executable(UpdateDeviceList
    FTDITemplateCompiler.cpp FTDITemplateCompiler.h
    UDLCommandLine.cpp UDLCommandLine.h
    UpdateDeviceList.cpp UpdateDeviceList.h
    main.cpp
)
target_include_directories(UpdateDeviceList PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ../../libraries/qcommon-console/../../../__Builds//ui/QCommon
)

target_compile_definitions(UpdateDeviceList PRIVATE
    QTAC_STATIC
    QT_DISABLE_DEPRECATED_UP_TO=0x060600
)

target_link_libraries(UpdateDeviceList PRIVATE
    Qt::Core
    Qt::SerialPort
    Qt::Xml
)

if(CMAKE_BUILD_TYPE STREQUAL Debug AND WIN32)
    target_compile_options(UpdateDeviceList
        /Zi
    )
endif()

if(LINUX)
    target_link_libraries(UpdateDeviceList PRIVATE
        dl
    )
endif()

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_definitions(UpdateDeviceList PRIVATE
        DEBUG
        _DEBUG
    )

    target_link_libraries(UpdateDeviceList PRIVATE
        # Remove: L${CMAKE_CURRENT_SOURCE_DIR}/../../../__Builds//lib
        QCommon-consoled
    )
endif()

if(NOT CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_definitions(UpdateDeviceList PRIVATE
        NDEBUG
        _NDEBUG
    )

    target_link_libraries(UpdateDeviceList PRIVATE
        # Remove: L${CMAKE_CURRENT_SOURCE_DIR}/../../../__Builds//lib
        QCommon-console
    )
endif()

if((CONFIG(x32)) AND (WIN32))
    target_compile_definitions(UpdateDeviceList PRIVATE
        _WIN32
    )
endif()

if((CONFIG(x32)) AND (LINUX))
    target_compile_definitions(UpdateDeviceList PRIVATE
        __i386__
    )
endif()

if((NOT (CONFIG(x32))) AND (LINUX))
    target_compile_definitions(UpdateDeviceList PRIVATE
        __X86_64__
    )
endif()

if(UNIX)
    target_compile_options(UpdateDeviceList
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

install(TARGETS UpdateDeviceList
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET UpdateDeviceList
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
