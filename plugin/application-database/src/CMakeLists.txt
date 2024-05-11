# https://doc.qt.io/qt-6/cmake-build-reusable-qml-module.html
qt_add_qml_module(qt_application_database URI Qti.ApplicationDatabase VERSION 0.1 SHARED
    SOURCES
    application_database.cpp application_database.hpp
)
install(TARGETS qt_application_database)
install(DIRECTORY "${QT_QML_OUTPUT_DIRECTORY}" DESTINATION "${INSTALL_QMLDIR}")
get_target_property(TARGET_PATH qt_application_database QT_QML_MODULE_TARGET_PATH)
install(TARGETS qt_application_databaseplugin LIBRARY DESTINATION "${INSTALL_QMLDIR}/${TARGET_PATH}")
target_link_libraries(qt_application_database PRIVATE Qt::Core Qt::Qml)
