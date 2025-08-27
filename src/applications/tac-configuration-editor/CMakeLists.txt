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
    ../../libraries/qcommon
    ../../libraries/qcommon-console
    ../../libraries/ui-common
)

target_compile_definitions(TACConfigEditor PRIVATE
    QTAC_STATIC
    QT_DEPRECATED_WARNINGS
    QT_DISABLE_DEPRECATED_UP_TO=0x060600
    $<$<CONFIG:Debug>:DEBUG;_DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG;_NDEBUG>
)

target_link_libraries(TACConfigEditor PRIVATE
    # Remove: L${CMAKE_CURRENT_SOURCE_DIR}/../../../__Builds//lib
    Qt::Core
    Qt::Gui
    Qt::SerialPort
    Qt::Widgets
    Qt::Xml
    ftd2xx
    $<$<CONFIG:Debug>:QcommonConsoled;QCommond;UId>
    $<$<NOT:$<CONFIG:Debug>>:QCommon;QCommonConsole;UI>
)

qt_add_resources(TACConfigEditor "TACConfigEditor"
    PREFIX "/"
    BASE "resources"
    FILES ${TACConfigEditor_resource_files}
)

set(TACConfigEditor_resource_files
    "About.txt"
    "BugWriter.png"
    "ManageTabs.png"
    "NotificationBellRing.png"
    "NotificationBellSilent.png"
    "Open.png"
    "Preview.drawio.png"
    "Save.png"
    "TACConfigEditor.bmp"
    "TACConfigEditor.ico"
    "TACConfigEditor.png"
    "TabsDown.png"
    "TabsPlus.png"
    "TabsRemove.png"
    "TabsRename.png"
    "TabsUp.png"
    "download-spinner.png"
    "entry-add.png"
    "entry-delete.png"
    "new-config.png"
    "script.png"
    "scriptcheck.png"
)

if(WIN32)
    target_compile_options(TACConfigEditor PRIVATE
        $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/Zi>
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

if(UNIX)
    target_link_libraries(TACConfigEditor PRIVATE
        # Remove: rdynamic
    )
    target_compile_options(TACConfigEditor PRIVATE
        -Wno-comment
        -Wno-conversion-null
        -Wno-reorder
        -Wno-unknown-pragmas
        -Wno-unused-function
        -Wno-unused-result
        -Wno-write-strings
    )
endif()

if(APPLE)
    target_link_libraries(TACConfigEditor PRIVATE
        "-framework CoreFoundation"
        "-framework IOKit"
        objc
    )
endif()

install(TARGETS TACConfigEditor
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET TACConfigEditor
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
