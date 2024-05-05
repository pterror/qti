# https://doc.qt.io/qt-6/cmake-build-reusable-qml-module.html
find_package(Qt6 REQUIRED COMPONENTS Gui Quick WaylandClient)
qt_add_qml_module(qti_screenshot URI Qti.Screenshot VERSION 0.1 SHARED
    SOURCES
    screenshot.cpp screenshot.hpp
    screenshot_image_provider.cpp screenshot_image_provider.hpp
    shm_buffer.cpp shm_buffer.hpp
    zwlr_screencopy_manager.cpp zwlr_screencopy_manager.hpp
    zwlr_screencopy_frame.cpp zwlr_screencopy_frame.hpp
    qimage_format_utils.hpp
)
qt_generate_wayland_protocol_client_sources(
    qti_screenshot # PRIVATE_CODE
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/../../../dep/wlroots/protocol/wlr-screencopy-unstable-v1.xml")
install(TARGETS qti_screenshot)
install(DIRECTORY "${QT_QML_OUTPUT_DIRECTORY}" DESTINATION "${INSTALL_QMLDIR}")
get_target_property(TARGET_PATH qti_screenshot QT_QML_MODULE_TARGET_PATH)
install(TARGETS qti_screenshotplugin LIBRARY DESTINATION "${INSTALL_QMLDIR}/${TARGET_PATH}")
target_link_libraries(qti_screenshot PRIVATE Qt::Core Qt6::Gui Qt::Qml Qt::Quick Qt6::WaylandClient Qt6::WaylandClientPrivate Wayland::Client)
