# https://doc.qt.io/qt-6/cmake-manual.html
cmake_minimum_required(VERSION 3.19)
project(Qti VERSION "0.1.0" LANGUAGES CXX)

set(QT_MIN_VERSION "6.6.0")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# nix workaround
if (CMAKE_EXPORT_COMPILE_COMMANDS)
    set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

add_compile_options(-Wall -Wextra)
qt_standard_project_setup()
set(QT_QML_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml_modules)
qt_add_library(qt_git SHARED src/main.cpp)
install(TARGETS qti
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
