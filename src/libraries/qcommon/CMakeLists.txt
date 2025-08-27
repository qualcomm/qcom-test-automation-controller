include(${CMAKE_SOURCE_DIR}/src/libraries/qcommon/Common.cmake)
set(LIBRARY_NAME QCommon)

qt_add_library(${LIBRARY_NAME} STATIC
    AboutDialog.cpp AboutDialog.h AboutDialog.ui
    AlpacaApplication.cpp AlpacaApplication.h
    ApplicationEnhancements.cpp ApplicationEnhancements.h
    ColorSwatch.cpp ColorSwatch.h
    CustomComboBox.h
    HappinessDialog.cpp HappinessDialog.h HappinessDialog.ui
    IntFilteredEditor.cpp IntFilteredEditor.h
    LEDWidget.cpp LEDWidget.h
    PainterEffects.cpp PainterEffects.h
    PinLED.cpp PinLED.h PinLED.ui
    ProgressDialog.cpp ProgressDialog.h ProgressDialog.ui
    QCommon.cpp QCommon.h
    QCommonGlobal.h
    StyledLabel.cpp StyledLabel.h
    TableCheckBox.cpp TableCheckBox.h
    TableComboBox.cpp TableComboBox.h
)

set_target_properties(${LIBRARY_NAME} PROPERTIES
    VERSION ALPACA_VERSION
)

QCommonSettings(${LIBRARY_NAME})

target_include_directories(${LIBRARY_NAME} PUBLIC
    ../qcommon-console
)

target_compile_definitions(${LIBRARY_NAME} PUBLIC
    QCOMMON_LIBRARY
    QTAC_STATIC
    QT_DEPRECATED_WARNINGS
    QT_DISABLE_DEPRECATED_UP_TO=0x060600

    $<$<CONFIG:Debug>:DEBUG;_DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG;_NDEBUG>
)

target_link_libraries(${LIBRARY_NAME} PUBLIC
    Qt::Concurrent
    Qt::Core
    Qt::Gui
    Qt::SerialPort
    Qt::Widgets
    Qt::Xml
)

qt_add_resources(${LIBRARY_NAME}
    QCommon.qrc
)

install(TARGETS ${LIBRARY_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
