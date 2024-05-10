# https://doc.qt.io/qt-6/cmake-manual.html
cmake_minimum_required(VERSION 3.19)
# c is required for wayland files
project(Qti VERSION "0.1.0")

set(QT_MIN_VERSION "6.6.0")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(WAYLAND "Enable Wayland support" ON)
option(PLUGIN_STDLIB "Include Stdlib plugin" ON)
option(PLUGIN_CORE "Include Core plugin" ON)
option(PLUGIN_SCREENSHOT "Include Screenshot plugin" ON)
option(PLUGIN_CLIPBOARD "Include Clipboard plugin" ON)

# nix workaround
if (CMAKE_EXPORT_COMPILE_COMMANDS)
    set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

add_compile_options(-Wall -Wextra)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2 Svg WaylandClient)
qt_standard_project_setup()
qt_add_executable(qti src/main.cpp)
install(TARGETS qti)
target_link_libraries(qti PRIVATE Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick Qt6::WaylandClient Qt6::WaylandClientPrivate)

if (PLUGIN_CORE)
    add_subdirectory(plugin/core)
endif()

if (PLUGIN_SCREENSHOT)
    add_subdirectory(plugin/screenshot)
endif()

if (PLUGIN_CLIPBOARD)
    add_subdirectory(plugin/clipboard)
endif()

if (PLUGIN_STDLIB)
    add_subdirectory(plugin/stdlib)
endif()
