# https://doc.qt.io/qt-6/cmake-build-reusable-qml-module.html
find_package(Qt6 REQUIRED COMPONENTS Gui)
qt_add_qml_module(qti_clipboard URI Qti.Clipboard VERSION 0.1 SHARED
    SOURCES
    clipboard.cpp clipboard.hpp
)
install(TARGETS qti_clipboard)
install(DIRECTORY "${QT_QML_OUTPUT_DIRECTORY}" DESTINATION "${INSTALL_QMLDIR}")
get_target_property(TARGET_PATH qti_clipboard QT_QML_MODULE_TARGET_PATH)
install(TARGETS qti_clipboardplugin LIBRARY DESTINATION "${INSTALL_QMLDIR}/${TARGET_PATH}")
target_link_libraries(qti_clipboard PRIVATE Qt::Core Qt::Qml Qt::Gui)
