# https://doc.qt.io/qt-6/cmake-build-reusable-qml-module.html
find_package(Qt6 REQUIRED COMPONENTS Gui Sql)
qt_add_qml_module(qti_sql URI Qti.Sql VERSION 0.1 SHARED
    SOURCES
    sql_database.cpp sql_database.hpp
    sql_table.cpp sql_table.hpp
)
install(TARGETS qti_sql)
install(DIRECTORY "${QT_QML_OUTPUT_DIRECTORY}" DESTINATION "${INSTALL_QMLDIR}")
get_target_property(TARGET_PATH qti_sql QT_QML_MODULE_TARGET_PATH)
install(TARGETS qti_sqlplugin LIBRARY DESTINATION "${INSTALL_QMLDIR}/${TARGET_PATH}")
target_link_libraries(qti_sql PRIVATE Qt::Core Qt::Qml Qt::Gui Qt::Sql)
