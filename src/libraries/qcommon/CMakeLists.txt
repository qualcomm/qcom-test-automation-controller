qt_add_library(qcommon
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
target_include_directories(qcommon PUBLIC
    ../../../__Builds//ui/QCommon
    ../qcommon-console
)

target_compile_definitions(qcommon PUBLIC
    QCOMMON_LIBRARY
    QTAC_STATIC
    QT_DEPRECATED_WARNINGS
    QT_DISABLE_DEPRECATED_BEFORE=0x060000
)

target_link_libraries(qcommon PUBLIC
    Qt::Concurrent
    Qt::Core
    Qt::Gui
    Qt::SerialPort
    Qt::Widgets
    Qt::Xml
)


# Resources:
set(QCommon_resource_files
    "resources/Alpaca_BS.png"
    "resources/BlueLed.png"
    "resources/ChartKey.png"
    "resources/DissatisfiedStar.png"
    "resources/GreenLed.png"
    "resources/HappyStar.png"
    "resources/LedOff.png"
    "resources/LoveStar.png"
    "resources/QTAC.ico"
    "resources/RedLed.png"
    "resources/Satisfied.png"
    "resources/Unhappy.png"
)

qt_add_resources(qcommon "QCommon"
    PREFIX
        "/"
    FILES
        ${QCommon_resource_files}
)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_definitions(qcommon PUBLIC
        DEBUG
        _DEBUG
    )
endif()

if(NOT CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_definitions(qcommon PUBLIC
        NDEBUG
        _NDEBUG
    )
endif()

if(WIN32)
    target_compile_options(qcommon
        /Zi
    )
endif()

if((CONFIG(x32)) AND (WIN32))
    target_compile_definitions(qcommon PUBLIC
        _WIN32
    )
endif()

if((CONFIG(x32)) AND (LINUX))
    target_compile_definitions(qcommon PUBLIC
        __i386__
    )
endif()

if((NOT (CONFIG(x32))) AND (LINUX))
    target_compile_definitions(qcommon PUBLIC
        __X86_64__
    )
endif()

if(UNIX)
    target_compile_options(qcommon
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

install(TARGETS qcommon
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
