# https://doc.qt.io/qt-6/cmake-build-reusable-qml-module.html
qt_add_qml_module(qti_process URI Qti.Process VERSION 0.1 SHARED
	SOURCES
	process.cpp process.hpp
)
install(TARGETS qti_process)
install(DIRECTORY "${QT_QML_OUTPUT_DIRECTORY}" DESTINATION "${INSTALL_QMLDIR}")
get_target_property(TARGET_PATH qti_process QT_QML_MODULE_TARGET_PATH)
install(TARGETS qti_processplugin LIBRARY DESTINATION "${INSTALL_QMLDIR}/${TARGET_PATH}")
target_link_libraries(qti_process PRIVATE Qt::Core Qt6::Qml)
