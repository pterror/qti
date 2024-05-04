# https://doc.qt.io/qt-6/cmake-manual.html
find_package(Qt6 REQUIRED COMPONENTS Gui Quick DBus)

if(QT_KNOWN_POLICY_QTP0001)
    qt_policy(SET QTP0001 NEW)
endif()

qt_standard_project_setup()
set(QT_QML_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml_modules)

qt_add_dbus_interface(DBUS_INTERFACES ../dep/xdg-desktop-portal/data/org.freedesktop.portal.Account.xml portal_account)
qt_add_dbus_interface(DBUS_INTERFACES ../dep/xdg-desktop-portal/data/org.freedesktop.portal.Background.xml portal_background)
qt_add_dbus_interface(DBUS_INTERFACES ../dep/xdg-desktop-portal/data/org.freedesktop.portal.Camera.xml portal_camera)
qt_add_dbus_interface(DBUS_INTERFACES ../dep/xdg-desktop-portal/data/org.freedesktop.portal.Clipboard.xml portal_clipboard)
qt_add_dbus_interface(DBUS_INTERFACES ../dep/xdg-desktop-portal/data/org.freedesktop.portal.Documents.xml portal_documents)
qt_add_dbus_interface(DBUS_INTERFACES ../dep/xdg-desktop-portal/data/org.freedesktop.portal.DynamicLauncher.xml portal_dynamic_launcher)
qt_add_dbus_interface(DBUS_INTERFACES ../dep/xdg-desktop-portal/data/org.freedesktop.portal.Email.xml portal_email)
qt_add_dbus_interface(DBUS_INTERFACES ../dep/xdg-desktop-portal/data/org.freedesktop.portal.FileChooser.xml portal_file_chooser)
qt_add_dbus_interface(DBUS_INTERFACES ../dep/xdg-desktop-portal/data/org.freedesktop.portal.FileTransfer.xml portal_file_transfer)
qt_add_dbus_interface(DBUS_INTERFACES ../dep/xdg-desktop-portal/data/org.freedesktop.portal.GameMode.xml portal_game_mode)
qt_add_dbus_interface(DBUS_INTERFACES ../dep/xdg-desktop-portal/data/org.freedesktop.portal.GlobalShortcuts.xml portal_global_shortcuts)
qt_add_dbus_interface(DBUS_INTERFACES ../dep/xdg-desktop-portal/data/org.freedesktop.portal.Inhibit.xml portal_inhibit)
qt_add_dbus_interface(DBUS_INTERFACES ../dep/xdg-desktop-portal/data/org.freedesktop.portal.InputCapture.xml portal_input_capture)
qt_add_dbus_interface(DBUS_INTERFACES ../dep/xdg-desktop-portal/data/org.freedesktop.portal.Location.xml portal_location)
qt_add_dbus_interface(DBUS_INTERFACES ../dep/xdg-desktop-portal/data/org.freedesktop.portal.MemoryMonitor.xml portal_memory_monitor)
qt_add_dbus_interface(DBUS_INTERFACES ../dep/xdg-desktop-portal/data/org.freedesktop.portal.NetworkMonitor.xml portal_network_monitor)
qt_add_dbus_interface(DBUS_INTERFACES ../dep/xdg-desktop-portal/data/org.freedesktop.portal.Notification.xml portal_notification)
qt_add_dbus_interface(DBUS_INTERFACES ../dep/xdg-desktop-portal/data/org.freedesktop.portal.OpenURI.xml portal_open_uri)
qt_add_dbus_interface(DBUS_INTERFACES ../dep/xdg-desktop-portal/data/org.freedesktop.portal.PowerProfileMonitor.xml portal_power_profile_monitor)
qt_add_dbus_interface(DBUS_INTERFACES ../dep/xdg-desktop-portal/data/org.freedesktop.portal.Print.xml portal_print)
qt_add_dbus_interface(DBUS_INTERFACES ../dep/xdg-desktop-portal/data/org.freedesktop.portal.ProxyResolver.xml portal_proxy_resolver)
qt_add_dbus_interface(DBUS_INTERFACES ../dep/xdg-desktop-portal/data/org.freedesktop.portal.Realtime.xml portal_realtime)
qt_add_dbus_interface(DBUS_INTERFACES ../dep/xdg-desktop-portal/data/org.freedesktop.portal.RemoteDesktop.xml portal_remote_desktop)
qt_add_dbus_interface(DBUS_INTERFACES ../dep/xdg-desktop-portal/data/org.freedesktop.portal.Request.xml portal_request)
qt_add_dbus_interface(DBUS_INTERFACES ../dep/xdg-desktop-portal/data/org.freedesktop.portal.ScreenCast.xml portal_screen_cast)
qt_add_dbus_interface(DBUS_INTERFACES ../dep/xdg-desktop-portal/data/org.freedesktop.portal.Screenshot.xml portal_screenshot)
qt_add_dbus_interface(DBUS_INTERFACES ../dep/xdg-desktop-portal/data/org.freedesktop.portal.Secret.xml portal_secret)
qt_add_dbus_interface(DBUS_INTERFACES ../dep/xdg-desktop-portal/data/org.freedesktop.portal.Session.xml portal_session)
qt_add_dbus_interface(DBUS_INTERFACES ../dep/xdg-desktop-portal/data/org.freedesktop.portal.Settings.xml portal_settings)
qt_add_dbus_interface(DBUS_INTERFACES ../dep/xdg-desktop-portal/data/org.freedesktop.portal.Trash.xml portal_trash)
qt_add_dbus_interface(DBUS_INTERFACES ../dep/xdg-desktop-portal/data/org.freedesktop.portal.Wallpaper.xml portal_wallpaper)

qt_add_library(
    qti_xdg_desktop_portal
    SHARED
    ${DBUS_INTERFACES}
    xdp_screenshot.cpp
    xdp_screenshot.hpp
)

qt_add_qml_module(qti_xdg_desktop_portal URI Qti.XdgDesktopPortal VERSION 0.1)

# dbus headers
target_include_directories(qti_xdg_desktop_portal PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

install(TARGETS qti_xdg_desktop_portal
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
target_link_libraries(qti_xdg_desktop_portal PRIVATE Qt::Core Qt6::Gui Qt::Qml Qt::Quick Qt::DBus)
