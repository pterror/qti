# https://doc.qt.io/qt-6/cmake-manual.html
cmake_minimum_required(VERSION 3.19)
project(Qti VERSION "0.0.1")

set(QT_MIN_VERSION "6.6.0")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(WAYLAND "Enable Wayland support" ON)
option(PLUGINS "Set the default value for whether plugins should be built" ON)
option(PLUGIN_STDLIB "Include Stdlib plugin" ${PLUGINS})
option(PLUGIN_CORE "Include Core plugin" ${PLUGINS})
option(PLUGIN_FILESYSTEM "Include Filesystem plugin" ${PLUGINS})
option(PLUGIN_PROCESS "Include Process plugin" ${PLUGINS})
option(PLUGIN_SCREENSHOT "Include Screenshot plugin" ${PLUGINS})
option(PLUGIN_CLIPBOARD "Include Clipboard plugin" ${PLUGINS})
option(PLUGIN_APPLICATION_DATABASE "Include Application Database plugin" ${PLUGINS})
option(PLUGIN_SQL "Include SQL plugin" ${PLUGINS})

# nix workaround
if (CMAKE_EXPORT_COMPILE_COMMANDS)
    set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

add_compile_options(-Wall -Wextra)
set(QT_FPDEPS Core Gui Qml Quick QuickControls2 Svg)
set(QT_PRIVDEPS)
set(QT_LIBS Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick)
if (WAYLAND)
	list(APPEND QT_FPDEPS WaylandClient)
	list(APPEND QT_PRIVDEPS WaylandClientPrivate)
	list(APPEND QT_LIBS Qt6::WaylandClient Qt6::WaylandClientPrivate)
endif()
find_package(Qt6 REQUIRED COMPONENTS ${QT_FPDEPS})
# In Qt 6.10, private dependencies are required to be explicit,
# but they could not be explicitly depended on prior to 6.9.
if (Qt6_VERSION VERSION_GREATER_EQUAL "6.9.0")
	set(QT_NO_PRIVATE_MODULE_WARNING ON)
	find_package(Qt6 REQUIRED COMPONENTS ${QT_PRIVDEPS})
endif()
qt_standard_project_setup()
qt_add_executable(qti src/main.cpp)
install(TARGETS qti)
target_link_libraries(qti PRIVATE ${QT_LIBS})

if (PLUGIN_STDLIB)
    add_subdirectory(plugin/stdlib)
endif()

if (PLUGIN_CORE)
    add_subdirectory(plugin/core)
endif()

if (PLUGIN_FILESYSTEM)
    add_subdirectory(plugin/filesystem)
endif()

if (PLUGIN_PROCESS)
    add_subdirectory(plugin/process)
endif()

if (PLUGIN_SCREENSHOT)
    add_subdirectory(plugin/screenshot)
endif()

if (PLUGIN_CLIPBOARD)
    add_subdirectory(plugin/clipboard)
endif()

if (PLUGIN_APPLICATION_DATABASE)
    add_subdirectory(plugin/application-database)
endif()

if (PLUGIN_SQL)
    add_subdirectory(plugin/sql)
endif()
