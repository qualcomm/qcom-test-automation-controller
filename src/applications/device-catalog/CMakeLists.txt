qt_add_executable(DeviceCatalog WIN32 MACOSX_BUNDLE
    DeviceCatalog.cpp DeviceCatalog.h DeviceCatalog.ui
    DeviceCatalogApplication.cpp DeviceCatalogApplication.h
    DeviceSelectionDialog.cpp DeviceSelectionDialog.h DeviceSelectionDialog.ui
    main.cpp
)

target_include_directories(DeviceCatalog PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ../../../libraries/qcommon-console
    ../../../libraries/qcommon
    ../../../libraries/ui-common
)

target_compile_definitions(DeviceCatalog PRIVATE
    QTAC_STATIC
    QT_DEPRECATED_WARNINGS
    QT_DISABLE_DEPRECATED_UP_TO=0x060600
    $<$<CONFIG:Debug>:DEBUG;_DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG;_NDEBUG>
)

target_link_libraries(DeviceCatalog PRIVATE
    Qt::Core
    Qt::Gui
    Qt::SerialPort
    Qt::Widgets
    ftd2xx
    $<$<CONFIG:Debug>:QCommonConsoled;QCommond;UId>
    $<$<NOT:$<CONFIG:Debug>>:QCommon;QCommonConsole;UI>
)

qt_add_resources(DeviceCatalog "DeviceCatalog"
    PREFIX "/"
    BASE "resources"
    FILES ${DeviceCatalog_resource_files}
)

set(DeviceCatalog_resource_files
    "device-catalog.ico"
    "device-catalog.png"
)

if(WIN32)
    target_link_libraries(DeviceCatalog PRIVATE
        Ole32
        SetupAPI
        Winusb
    )

    target_compile_options(DeviceCatalog PRIVATE
        $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/Zi>
    )
endif()

if(LINUX)
    target_link_libraries(DeviceCatalog PRIVATE dl)
endif()

if(UNIX)
    target_compile_options(DeviceCatalog PRIVATE
        -Wno-comment
        -Wno-conversion-null
        -Wno-reorder
        -Wno-unknown-pragmas
        -Wno-unused-function
        -Wno-unused-result
        -Wno-write-strings
    )
endif()

install(TARGETS DeviceCatalog
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET DeviceCatalog
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
