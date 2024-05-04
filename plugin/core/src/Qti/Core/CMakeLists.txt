# https://doc.qt.io/qt-6/cmake-manual.html
find_package(Qt6 REQUIRED COMPONENTS Gui)
qt_add_qml_module(qti_core URI Qti.Core VERSION 0.1 SHARED
    SOURCES
    core.cpp core.hpp
)
install(TARGETS qti_core)
install(TARGETS qti_coreplugin LIBRARY DESTINATION "${INSTALL_PLUGINSDIR}")
target_link_libraries(qti_core PRIVATE Qt::Core Qt::Qml Qt::Gui)
