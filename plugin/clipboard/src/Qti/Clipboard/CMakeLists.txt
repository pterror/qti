# https://doc.qt.io/qt-6/cmake-manual.html

find_package(Qt6 REQUIRED COMPONENTS Gui)

qt_standard_project_setup()
set(QT_QML_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml_modules)

qt_add_library(qti_clipboard SHARED
    clipboard.cpp
    clipboard.hpp
)

qt_add_qml_module(qti_clipboard URI Qti.Clipboard VERSION 0.1)

install(TARGETS qti_clipboard
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
target_link_libraries(qti_clipboard PRIVATE Qt::Core Qt::Qml Qt::Gui)
