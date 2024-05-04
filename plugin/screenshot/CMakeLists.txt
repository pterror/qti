# https://doc.qt.io/qt-6/cmake-manual.html
cmake_minimum_required(VERSION 3.16)
project(Qti VERSION "0.1.0" LANGUAGES CXX C)

set(QT_MIN_VERSION "6.6.0")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# nix workaround
if (CMAKE_EXPORT_COMPILE_COMMANDS)
    set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

add_compile_options(-Wall -Wextra)

find_package(Qt6 REQUIRED COMPONENTS Core Qml)

if (DEFINED ENV{QTWAYLANDSCANNER})
    set(qtwaylandscanner $ENV{QTWAYLANDSCANNER})
else()
    find_program(qtwaylandscanner NAMES qtwaylandscanner)
endif()
if (qtwaylandscanner STREQUAL "qtwaylandscanner-NOTFOUND")
    message(FATAL_ERROR "qtwaylandscanner not found. Set the QTWAYLANDSCANNER environment variable to specify its path explicity.")
endif()
message(STATUS "Found QtWaylandScanner: ${qtwaylandscanner}")

find_program(waylandscanner NAMES wayland-scanner)
message(STATUS "Found WaylandScanner: ${waylandscanner}")

execute_process(
    COMMAND pkg-config --variable=pkgdatadir wayland-protocols
    OUTPUT_VARIABLE WAYLAND_PROTOCOLS_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(STATUS "Found WaylandProtocols: ${WAYLAND_PROTOCOLS_DIR}")

if(QT_KNOWN_POLICY_QTP0001)
    qt_policy(SET QTP0001 NEW)
endif()

qt_standard_project_setup()
add_subdirectory(src/Qti/Screenshot)
