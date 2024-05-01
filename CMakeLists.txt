# https://doc.qt.io/qt-6/cmake-manual.html
cmake_minimum_required(VERSION 3.16)
# c is required for wayland files
project(Qti VERSION "0.1.0" LANGUAGES CXX)

set(QT_MIN_VERSION "6.6.0")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(WAYLAND "Enable Wayland support" ON)

# nix workaround
if (CMAKE_EXPORT_COMPILE_COMMANDS)
    set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

add_compile_options(-Wall -Wextra)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2 Svg WaylandClient) # DBus
qt_standard_project_setup()
set(QT_QML_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml_modules)
qt_add_executable(qti src/main.cpp)
install(TARGETS qti
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
target_link_libraries(qti PRIVATE Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick Qt6::WaylandClient Qt6::WaylandClientPrivate)
