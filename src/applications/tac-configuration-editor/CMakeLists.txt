qt_add_executable(TACConfigEditor WIN32 MACOSX_BUNDLE
    ButtonEditor.cpp ButtonEditor.h ButtonEditor.ui
    CodeEditor.cpp CodeEditor.h CodeEditor.ui
    ConfigEditorApplication.cpp ConfigEditorApplication.h
    ConfigWindow.cpp ConfigWindow.h ConfigWindow.ui
    CreateConfigurationDialog.cpp CreateConfigurationDialog.h CreateConfigurationDialog.ui
    EditorView.cpp EditorView.h
    FTDIEditorView.cpp FTDIEditorView.h FTDIEditorView.ui
    ManageTabsDialog.cpp ManageTabsDialog.h ManageTabsDialog.ui
    PineCommandLine.cpp PineCommandLine.h
    TACPreviewWindow.cpp TACPreviewWindow.h TACPreviewWindow.ui
    TabEditorDialog.cpp TabEditorDialog.h TabEditorDialog.ui
    main.cpp
)
target_include_directories(TACConfigEditor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ../../libraries/qcommon/../../../__Builds//ui/QCommon
    ../qcommon-console
    ../ui-common
)

target_compile_definitions(TACConfigEditor PRIVATE
    QTAC_STATIC
    QT_DEPRECATED_WARNINGS
    QT_DISABLE_DEPRECATED_BEFORE=0x060000
)

target_link_libraries(TACConfigEditor PRIVATE
    # Remove: L${CMAKE_CURRENT_SOURCE_DIR}/../../../__Builds//lib
    Qt::Core
    Qt::Gui
    Qt::SerialPort
    Qt::Widgets
    Qt::Xml
    ftd2xx
)


# Resources:
set(TACConfigEditor_resource_files
    "resources/About.txt"
    "resources/BugWriter.png"
    "resources/ManageTabs.png"
    "resources/NotificationBellRing.png"
    "resources/NotificationBellSilent.png"
    "resources/Open.png"
    "resources/Preview.drawio.png"
    "resources/Save.png"
    "resources/TACConfigEditor.bmp"
    "resources/TACConfigEditor.ico"
    "resources/TACConfigEditor.png"
    "resources/TACConfigEditor_BS.png"
    "resources/TabsDown.png"
    "resources/TabsPlus.png"
    "resources/TabsRemove.png"
    "resources/TabsRename.png"
    "resources/TabsUp.png"
    "resources/download-spinner.png"
    "resources/entry-add.png"
    "resources/entry-delete.png"
    "resources/new-config.png"
    "resources/script.png"
    "resources/scriptcheck.png"
)

qt_add_resources(TACConfigEditor "TACConfigEditor"
    PREFIX
        "/"
    BASE
        "resources"
    FILES
        ${TACConfigEditor_resource_files}
)

if(WIN32)
    target_compile_options(TACConfigEditor
        /Zi
    )
endif()

if(LINUX)
    target_link_libraries(TACConfigEditor PRIVATE
        # Remove: L/usr/local/lib
        dl
        rt
        uuid
    )
endif()

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_definitions(TACConfigEditor PRIVATE
        DEBUG
        _DEBUG
    )

    target_link_libraries(TACConfigEditor PRIVATE
        # Remove: L${CMAKE_CURRENT_SOURCE_DIR}/../../../__Builds//lib
        QCommon-consoled
        QCommond
        UId
    )
endif()

if(NOT CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_definitions(TACConfigEditor PRIVATE
        NDEBUG
        _NDEBUG
    )

    target_link_libraries(TACConfigEditor PRIVATE
        # Remove: L${CMAKE_CURRENT_SOURCE_DIR}/../../../__Builds//lib
        QCommon
        QCommon-console
        UI
    )
endif()

if(UNIX)
    target_link_libraries(TACConfigEditor PRIVATE
        # Remove: rdynamic
    )

    target_compile_options(TACConfigEditor
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

if(APPLE)
    target_link_libraries(TACConfigEditor PRIVATE
        "-framework CoreFoundation"
        "-framework IOKit"
        objc
    )
endif()

if((CONFIG(x32)) AND (WIN32))
    target_compile_definitions(TACConfigEditor PRIVATE
        _WIN32
    )
endif()

if((CONFIG(x32)) AND (LINUX))
    target_compile_definitions(TACConfigEditor PRIVATE
        __i386__
    )
endif()

if((NOT (CONFIG(x32))) AND (LINUX))
    target_compile_definitions(TACConfigEditor PRIVATE
        __X86_64__
    )
endif()

install(TARGETS TACConfigEditor
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET TACConfigEditor
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
