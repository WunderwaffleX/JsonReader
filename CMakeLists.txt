cmake_minimum_required(VERSION 3.19)
project(JsonReader LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Sql)
qt_standard_project_setup()

file(GLOB_RECURSE SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
)

qt_add_executable(JsonReader
    WIN32 MACOSX_BUNDLE
    main.cpp
    ${SRC_FILES}
)

target_include_directories(JsonReader PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(JsonReader PRIVATE
    Qt::Core
    Qt::Widgets
    Qt::Sql
)

include(GNUInstallDirs)

install(TARGETS JsonReader
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


