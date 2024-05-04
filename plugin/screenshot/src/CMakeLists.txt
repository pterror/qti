# https://doc.qt.io/qt-6/cmake-manual.html
find_package(Qt6 REQUIRED COMPONENTS Gui Quick WaylandClient)

qt_standard_project_setup()
set(QT_QML_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml_modules)

qt_add_library(qti_screenshot SHARED
    screenshot.cpp
    screenshot.hpp
    screenshot_wl_registry.cpp
    screenshot_wl_registry.hpp
)

qt_add_qml_module(qti_screenshot URI Qti.Screenshot VERSION 0.1 PLUGIN_TARGET qti_screenshot_plugin)

qt_generate_wayland_protocol_client_sources(
    qti_screenshot # PRIVATE_CODE
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/../dep/wlroots/protocol/wlr-screencopy-unstable-v1.xml")
qt_generate_wayland_protocol_client_sources(
    qti_screenshot # PRIVATE_CODE
    FILES "${WAYLAND_PROTOCOLS_DIR}/unstable/xdg-output/xdg-output-unstable-v1.xml")

install(TARGETS qti_screenshot
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
target_link_libraries(qti_screenshot PRIVATE Qt::Core Qt6::Gui Qt::Qml Qt::Quick Qt6::WaylandClient Qt6::WaylandClientPrivate Wayland::Client)
