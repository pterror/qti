# https://doc.qt.io/qt-6/cmake-build-reusable-qml-module.html
find_package(Qt6 REQUIRED COMPONENTS Gui)
qt_add_qml_module(qti_core URI Qti.Core VERSION 0.1 SHARED
    SOURCES
    qti_core.cpp qti_core.hpp
)
install(TARGETS qti_core)
install(DIRECTORY "${QT_QML_OUTPUT_DIRECTORY}" DESTINATION "${INSTALL_QMLDIR}")
get_target_property(TARGET_PATH qti_core QT_QML_MODULE_TARGET_PATH)
install(TARGETS qti_coreplugin LIBRARY DESTINATION "${INSTALL_QMLDIR}/${TARGET_PATH}")
target_link_libraries(qti_core PRIVATE Qt::Core Qt::Qml Qt::Gui)
