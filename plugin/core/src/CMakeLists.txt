# https://doc.qt.io/qt-6/cmake-manual.html

find_package(Qt6 REQUIRED COMPONENTS Gui)

qt_standard_project_setup()
set(QT_QML_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml_modules)

qt_add_library(
    qti_core
    SHARED
    core.cpp
    core.hpp
)

qt_add_qml_module(qti_core URI Qti.Core VERSION 0.1)

install(TARGETS qti_core
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
target_link_libraries(qti_core PRIVATE Qt::Core Qt::Qml Qt::Gui)
