include(${CMAKE_CURRENT_SOURCE_DIR}/../qcommon/Common.cmake)

set(LIBRARY_NAME UI)

qt_add_library(${LIBRARY_NAME} STATIC
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

set_target_properties(${LIBRARY_NAME} PROPERTIES
    VERSION ALPACA_VERSION
)

target_include_directories(${LIBRARY_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ../qcommon-console
    ../qcommon
)

target_compile_definitions(${LIBRARY_NAME} PUBLIC
    QTAC_STATIC
    QT_DEPRECATED_WARNINGS
    QT_DISABLE_DEPRECATED_UP_TO=0x060600
    $<$<CONFIG:Debug>:DEBUG;_DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG;_NDEBUG>
)

target_link_libraries(${LIBRARY_NAME} PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

qt_add_resources(${LIBRARY_NAME}
    resources/UI.qrc
)

set_target_properties(${LIBRARY_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
)

install(TARGETS ${LIBRARY_NAME}
    # TODO: Might change this
    BUNDLE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
