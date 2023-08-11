cmake_minimum_required(VERSION 3.14)

project(comaresources LANGUAGES CXX)

include(GenerateExportHeader)

option(COMARES_STATIC:BOOL "Build resources as static library" OFF)

# Finding QtCore library for using rcc compiler and "qt_add_binary_resources" function
find_package(Qt${QT_VERSION_MAJOR}
  COMPONENTS Core REQUIRED)

# Collecting qrc files
set(QDARKSTYLESHEET_PATH ${CMAKE_CURRENT_SOURCE_DIR}/styles/QDarkStyleSheet/qdarkstyle)
set(files_qrc
    "${CMAKE_CURRENT_SOURCE_DIR}/../../images/images.qrc"
    "${QDARKSTYLESHEET_PATH}/light/lightstyle.qrc"
    "${QDARKSTYLESHEET_PATH}/dark/darkstyle.qrc"
    "${CMAKE_CURRENT_SOURCE_DIR}/styles/styles.qrc")

# Collecting sources
set(COMARES_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}")
set(COMARES_SRC     "${CMAKE_CURRENT_SOURCE_DIR}/src/")
set(sources
    "${COMARES_INCLUDE}/export.h"
    "${COMARES_INCLUDE}/manage.h"
    "${COMARES_SRC}/manage.cpp")

# Create library target
if(COMARES_STATIC)
    add_library(${PROJECT_NAME} STATIC ${sources} ${files_qrc})
else()
    add_library(${PROJECT_NAME} SHARED ${sources} ${files_qrc})
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE "-DCOMARES_SHARED_EXPORT"
        INTERFACE "-DCOMARES_SHARED_IMPORT")
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core)

set(COMARES_BINDIR "${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "Coma resources binary dir: ${CMAKE_CURRENT_BINARY_DIR}")
install(CODE "set(COMARES_BINDIR \"${COMARES_BINDIR}\")")
