# https://doc.qt.io/qt-6/cmake-build-reusable-qml-module.html
qt_add_qml_module(qti_file_system URI Qti.Filesystem VERSION 0.1 SHARED
    SOURCES
    file.cpp file.hpp
    folder.cpp folder.hpp
    filesystem_entry.cpp filesystem_entry.hpp
)
install(TARGETS qti_file_system RESOURCE DESTINATION "${INSTALL_PLUGINSDIR}")
target_link_libraries(qti_file_system PRIVATE Qt::Core Qt6::Qml)
