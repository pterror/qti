# https://doc.qt.io/qt-6/cmake-build-reusable-qml-module.html
qt_add_qml_module(qti_filesystem URI Qti.Filesystem VERSION 0.1 SHARED
    SOURCES
    file.cpp file.hpp
    folder.cpp folder.hpp
    filesystem_entry.cpp filesystem_entry.hpp
)
install(TARGETS qti_filesystem)
install(DIRECTORY "${QT_QML_OUTPUT_DIRECTORY}" DESTINATION "${INSTALL_QMLDIR}")
get_target_property(TARGET_PATH qti_filesystem QT_QML_MODULE_TARGET_PATH)
install(TARGETS qti_filesystemplugin LIBRARY DESTINATION "${INSTALL_QMLDIR}/${TARGET_PATH}")
target_link_libraries(qti_filesystem PRIVATE Qt::Core Qt6::Qml)
