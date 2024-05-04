# https://doc.qt.io/qt-6/cmake-manual.html

find_package(Qt6 REQUIRED COMPONENTS Gui)

qt_standard_project_setup()
set(QT_QML_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml_modules)

qt_add_library(
    qt_q_gui_application
    SHARED
    q_gui_application.cpp
    q_gui_application.hpp
)

qt_add_qml_module(qt_q_gui_application URI Qti.QGuiApplication VERSION 0.1)

install(TARGETS qt_q_gui_application
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
target_link_libraries(qt_q_gui_application PRIVATE Qt::Core Qt::Qml Qt::Gui)
