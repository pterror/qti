# https://doc.qt.io/qt-6/cmake-manual.html
find_package(Qt6 REQUIRED COMPONENTS Gui)

if(QT_KNOWN_POLICY_QTP0001)
    qt_policy(SET QTP0001 NEW)
endif()

qt_standard_project_setup()
set(QT_QML_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml_modules)

qt_add_library(
    qt_screenshot
    SHARED
    screenshot.cpp
    screenshot.hpp
)

qt_add_qml_module(qt_screenshot URI Qti.Screenshot VERSION 0.1)

install(TARGETS qt_screenshot
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
target_link_libraries(qt_screenshot PRIVATE Qt::Core Qt6::Gui Qt::Qml)
