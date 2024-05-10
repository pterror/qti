# https://doc.qt.io/qt-6/cmake-build-reusable-qml-module.html
set_source_files_properties(Theme.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)
qt_add_qml_module(qti_stdlib URI Qti.Stdlib VERSION 0.1 SHARED
    NO_PLUGIN
    QML_FILES
    Theme.qml
    QtiWindow.qml
    Text.qml
    TextButton.qml
    IconButton.qml
    CustomButton.qml
)
install(TARGETS qti_stdlib)
install(DIRECTORY "${QT_QML_OUTPUT_DIRECTORY}" DESTINATION "${INSTALL_QMLDIR}")
get_target_property(QMLDIR_CONTENT qti_stdlib _qt_internal_qmldir_content)
string(REGEX REPLACE "prefer :[^\n]+\n" "" QMLDIR_CONTENT "${QMLDIR_CONTENT}")
set_property(TARGET qti_stdlib PROPERTY _qt_internal_qmldir_content "${QMLDIR_CONTENT}")
