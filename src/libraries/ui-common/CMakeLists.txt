qt_add_library(ui-common
    ColorConversion.cpp ColorConversion.h
    CommandButton.cpp CommandButton.h
    CustomValidator.cpp CustomValidator.h
    HoverAwareQWindow.cpp HoverAwareQWindow.h HoverAwareQWindow.ui
    I2CWidget.cpp I2CWidget.h I2CWidget.ui
    NotificationWidget.cpp NotificationWidget.h NotificationWidget.ui
    TACFrame.cpp TACFrame.h TACFrame.ui
    TerminalWidget.cpp TerminalWidget.h TerminalWidget.ui
    UIGlobalLib.h
    UILib.cpp UILib.h
    UserWidget.cpp UserWidget.h UserWidget.ui
    VariableInput.cpp VariableInput.h VariableInput.ui
)
target_include_directories(ui-common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ../qcommon-console
    ../qcommon/../../../__Builds//ui/QCommon
)

target_compile_definitions(ui-common PUBLIC
    QTAC_STATIC
    QT_DEPRECATED_WARNINGS
    QT_DISABLE_DEPRECATED_BEFORE=0x060000
)

target_link_libraries(ui-common PUBLIC
    Qt::Core
    Qt::Gui
    Qt::SerialPort
    Qt::Widgets
)


# Resources:
set(ui-common_resource_files
    "resources/NotificationBellRing.png"
    "resources/NotificationBellSilent.png"
    "resources/Warning.drawio.png"
    "resources/doubleclick.png"
)

qt_add_resources(ui-common "ui-common"
    PREFIX
        "/"
    BASE
        "resources"
    FILES
        ${ui-common_resource_files}
)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_definitions(ui-common PUBLIC
        DEBUG
        _DEBUG
    )
endif()

if(NOT CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_definitions(ui-common PUBLIC
        NDEBUG
        _NDEBUG
    )
endif()

if(WIN32)
    target_compile_options(ui-common
        /Zi
    )
endif()

if((CONFIG(x32)) AND (WIN32))
    target_compile_definitions(ui-common PUBLIC
        _WIN32
    )
endif()

if((CONFIG(x32)) AND (LINUX))
    target_compile_definitions(ui-common PUBLIC
        __i386__
    )
endif()

if((NOT (CONFIG(x32))) AND (LINUX))
    target_compile_definitions(ui-common PUBLIC
        __X86_64__
    )
endif()

if(UNIX)
    target_compile_options(ui-common
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

install(TARGETS ui-common
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
