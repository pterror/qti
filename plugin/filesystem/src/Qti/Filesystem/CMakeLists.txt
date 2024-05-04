# https://doc.qt.io/qt-6/cmake-manual.html
qt_add_library(qti_file_system SHARED
    file.cpp
    file.hpp
    folder.cpp
    folder.hpp
    filesystem_entry.cpp
    filesystem_entry.hpp
)
qt_add_qml_module(qti_file_system URI Qti.Filesystem VERSION 0.1 SHARED
    SOURCES
    file.cpp file.hpp
    folder.cpp folder.hpp
    filesystem_entry.cpp filesystem_entry.hpp
)
install(TARGETS qti_file_system)
install(TARGETS qti_file_systemplugin LIBRARY DESTINATION "${INSTALL_PLUGINSDIR}")
target_link_libraries(qti_file_system PRIVATE Qt::Core Qt6::Qml)
