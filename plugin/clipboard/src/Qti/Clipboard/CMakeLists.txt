# https://doc.qt.io/qt-6/cmake-build-reusable-qml-module.html
find_package(Qt6 REQUIRED COMPONENTS Gui)
qt_add_qml_module(qti_clipboard URI Qti.Clipboard VERSION 0.1 SHARED
    SOURCES
    clipboard.cpp clipboard.hpp
)
install(TARGETS qti_clipboard RESOURCE DESTINATION "${INSTALL_PLUGINSDIR}")
target_link_libraries(qti_clipboard PRIVATE Qt::Core Qt::Qml Qt::Gui)
