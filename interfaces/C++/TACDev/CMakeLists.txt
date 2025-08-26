qt_add_library(TACDev
    TACDev.cpp TACDev.h
    TACDevCore.cpp TACDevCore.h
)
target_include_directories(TACDev PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ../../../src/libraries/qcommon-console/../../../__Builds//ui/QCommon
)

target_compile_definitions(TACDev PUBLIC
    QTAC_STATIC
    QT_DISABLE_DEPRECATED_UP_TO=0x060600
    TACDEV_LIBRARY
)

target_link_libraries(TACDev PUBLIC
    # Remove: L${CMAKE_CURRENT_SOURCE_DIR}/../../../__Builds//lib
    Qt::Core
    Qt::SerialPort
    ftd2xx
)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_definitions(TACDev PUBLIC
        DEBUG
        _DEBUG
    )

    target_link_libraries(TACDev PUBLIC
        # Remove: L${CMAKE_CURRENT_SOURCE_DIR}/../../../__Builds//lib
        QCommon-consoled
    )
endif()

if(NOT CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_definitions(TACDev PUBLIC
        NDEBUG
        _NDEBUG
    )

    target_link_libraries(TACDev PUBLIC
        # Remove: L${CMAKE_CURRENT_SOURCE_DIR}/../../../__Builds//lib
        QCommon-console
    )
endif()

if(WIN32)
    target_link_libraries(TACDev PUBLIC
        Advapi32
        Ole32
        SetupAPI
        Shell32
        Winusb
        Wtsapi32
    )
endif()

if(CMAKE_BUILD_TYPE STREQUAL Debug AND WIN32)
    target_compile_options(TACDev
        /Zi
    )
endif()

if((CONFIG(x32)) AND (WIN32))
    target_compile_definitions(TACDev PUBLIC
        _WIN32
    )
endif()

if((CONFIG(x32)) AND (LINUX))
    target_compile_definitions(TACDev PUBLIC
        __i386__
    )
endif()

if((NOT (CONFIG(x32))) AND (LINUX))
    target_compile_definitions(TACDev PUBLIC
        __X86_64__
    )
endif()

if(UNIX)
    target_compile_options(TACDev
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

install(TARGETS TACDev
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
