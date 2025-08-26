qt_add_executable(DeviceCatalog WIN32 MACOSX_BUNDLE
    DeviceCatalog.cpp DeviceCatalog.h DeviceCatalog.ui
    DeviceCatalogApplication.cpp DeviceCatalogApplication.h
    DeviceSelectionDialog.cpp DeviceSelectionDialog.h DeviceSelectionDialog.ui
    main.cpp
)
target_include_directories(DeviceCatalog PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ../../libraries/qcommon/../../../__Builds//ui/QCommon
    ../qcommon-console
    ../ui-common
)

target_compile_definitions(DeviceCatalog PRIVATE
    QTAC_STATIC
    QT_DEPRECATED_WARNINGS
    QT_DISABLE_DEPRECATED_BEFORE=0x060000
)

target_link_libraries(DeviceCatalog PRIVATE
    # Remove: L${CMAKE_CURRENT_SOURCE_DIR}/../../../__Builds//lib
    Qt::Core
    Qt::Gui
    Qt::SerialPort
    Qt::Widgets
    ftd2xx
)


# Resources:
set(DeviceCatalog_resource_files
    "resources/device-catalog.ico"
    "resources/device-catalog.png"
)

qt_add_resources(DeviceCatalog "DeviceCatalog"
    PREFIX
        "/"
    FILES
        ${DeviceCatalog_resource_files}
)

if(WIN32)
    target_link_libraries(DeviceCatalog PRIVATE
        Ole32
        SetupAPI
        Winusb
    )

    target_compile_options(DeviceCatalog
        /Zi
    )
endif()

if(LINUX)
    target_link_libraries(DeviceCatalog PRIVATE
        dl
    )
endif()

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_definitions(DeviceCatalog PRIVATE
        DEBUG
        _DEBUG
    )

    target_link_libraries(DeviceCatalog PRIVATE
        # Remove: L${CMAKE_CURRENT_SOURCE_DIR}/../../../__Builds//lib
        QCommon-consoled
        QCommond
        UId
    )
endif()

if(NOT CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_definitions(DeviceCatalog PRIVATE
        NDEBUG
        _NDEBUG
    )

    target_link_libraries(DeviceCatalog PRIVATE
        # Remove: L${CMAKE_CURRENT_SOURCE_DIR}/../../../__Builds//lib
        QCommon
        QCommon-console
        UI
    )
endif()

if((CONFIG(x32)) AND (WIN32))
    target_compile_definitions(DeviceCatalog PRIVATE
        _WIN32
    )
endif()

if((CONFIG(x32)) AND (LINUX))
    target_compile_definitions(DeviceCatalog PRIVATE
        __i386__
    )
endif()

if((NOT (CONFIG(x32))) AND (LINUX))
    target_compile_definitions(DeviceCatalog PRIVATE
        __X86_64__
    )
endif()

if(UNIX)
    target_compile_options(DeviceCatalog
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

install(TARGETS DeviceCatalog
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET DeviceCatalog
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
