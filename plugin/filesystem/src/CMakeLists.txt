# https://doc.qt.io/qt-6/cmake-manual.html

if(QT_KNOWN_POLICY_QTP0001)
    qt_policy(SET QTP0001 NEW)
endif()

qt_standard_project_setup()
set(QT_QML_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml_modules)

qt_add_library(
    qt_file_system
    SHARED
    file.cpp
    file.hpp
    folder.cpp
    folder.hpp
    filesystem_entry.cpp
    filesystem_entry.hpp
)

qt_add_qml_module(qt_file_system URI Qti.Filesystem VERSION 0.1)

install(TARGETS qt_file_system
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
target_link_libraries(qt_file_system PRIVATE Qt::Core Qt6::Qml)
