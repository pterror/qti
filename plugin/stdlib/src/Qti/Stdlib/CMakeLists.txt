# https://doc.qt.io/qt-6/cmake-build-reusable-qml-module.html
set_source_files_properties(Theme.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)
qt_add_qml_module(qti_stdlib URI Qti.Stdlib VERSION 0.1 SHARED
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
get_target_property(TARGET_PATH qti_stdlib QT_QML_MODULE_TARGET_PATH)
install(TARGETS qti_stdlibplugin LIBRARY DESTINATION "${INSTALL_QMLDIR}/${TARGET_PATH}")
