# https://doc.qt.io/qt-6/cmake-build-reusable-qml-module.html
cmake_minimum_required(VERSION 3.19)
project(QtiCore VERSION "0.0.1")

set(QT_MIN_VERSION "6.6.0")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(QML_OUTPUT_SUBDIR "" CACHE STRING "Subdirectory to which the QML plugin should be compiled")

# nix workaround
if (CMAKE_EXPORT_COMPILE_COMMANDS)
    set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

add_compile_options(-Wall -Wextra)

find_package(Qt6 REQUIRED COMPONENTS Core Qml)

if(QT_KNOWN_POLICY_QTP0001)
    qt_policy(SET QTP0001 NEW)
endif()

set(QT_QML_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}${QML_OUTPUT_SUBDIR}")
qt_standard_project_setup()
add_subdirectory(src)
