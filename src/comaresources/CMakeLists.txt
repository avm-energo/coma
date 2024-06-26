cmake_minimum_required(VERSION 3.14)

project(comaresources
  LANGUAGES CXX
  VERSION 1.0.0)

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
set(COMARES_SOURCES
  "${COMARES_INCLUDE}/export.h"
  "${COMARES_INCLUDE}/manage.h"
  "${COMARES_SRC}/manage.cpp")

# Create library target
if(COMARES_STATIC)
  add_library(${PROJECT_NAME} STATIC ${COMARES_SOURCES} ${files_qrc})
else()
  add_library(${PROJECT_NAME} SHARED ${COMARES_SOURCES} ${files_qrc})
  target_compile_definitions(${PROJECT_NAME}
    PRIVATE "-DCOMARES_SHARED_EXPORT"
    INTERFACE "-DCOMARES_SHARED_IMPORT")
endif()

# Set project version as library target property
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

# Including directories for library
target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
# Linking libraries for current target
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core)

# Cross-platform installing commands
if(CMAKE_SYSTEM_NAME_LOWER STREQUAL "windows")
  set(COMARES_BINDIR "${CMAKE_CURRENT_BINARY_DIR}")
  message(STATUS "Coma resources binary dir: ${COMARES_BINDIR}")
  install(CODE "set(COMARES_BINDIR \"${COMARES_BINDIR}\")")
elseif(CMAKE_SYSTEM_NAME_LOWER STREQUAL "linux")
  install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION "/usr/local/lib" COMPONENT "AVMRESOURCES")
endif()
