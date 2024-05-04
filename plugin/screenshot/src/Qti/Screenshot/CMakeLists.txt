# https://doc.qt.io/qt-6/cmake-manual.html
find_package(Qt6 REQUIRED COMPONENTS Gui Quick WaylandClient)
qt_add_qml_module(qti_screenshot URI Qti.Screenshot VERSION 0.1 SHARED
    SOURCES
    screenshot.cpp screenshot.hpp
    screenshot_wl_registry.cpp screenshot_wl_registry.hpp
)
qt_generate_wayland_protocol_client_sources(
    qti_screenshot # PRIVATE_CODE
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/../../../dep/wlroots/protocol/wlr-screencopy-unstable-v1.xml")
qt_generate_wayland_protocol_client_sources(
    qti_screenshot # PRIVATE_CODE
    FILES "${WAYLAND_PROTOCOLS_DIR}/unstable/xdg-output/xdg-output-unstable-v1.xml")
install(TARGETS qti_screenshot)
install(TARGETS qti_screenshotplugin LIBRARY DESTINATION "${INSTALL_PLUGINSDIR}")
target_link_libraries(qti_screenshot PRIVATE Qt::Core Qt6::Gui Qt::Qml Qt::Quick Qt6::WaylandClient Qt6::WaylandClientPrivate Wayland::Client)
