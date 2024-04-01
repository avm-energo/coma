cmake_minimum_required(VERSION 3.14)

project(journals LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Widgets
  REQUIRED)

# Collecting sources
set(JOUR_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include/journals")
set(JOUR_SRC_PATH     "${CMAKE_CURRENT_SOURCE_DIR}/src")

set(files_h
  "${JOUR_INCLUDE_PATH}/basejournal.h"
  "${JOUR_INCLUDE_PATH}/eventparser.h"
  "${JOUR_INCLUDE_PATH}/sysjournal.h"
  "${JOUR_INCLUDE_PATH}/workjournal.h"
  "${JOUR_INCLUDE_PATH}/measjournal.h"
  "${JOUR_INCLUDE_PATH}/measparser.h"
  "${JOUR_INCLUDE_PATH}/journaldialog.h"
  "${JOUR_INCLUDE_PATH}/journaltabwidget.h"
  "${JOUR_INCLUDE_PATH}/journalviewer.h"
)

set(files_cpp
  "${JOUR_SRC_PATH}/basejournal.cpp"
  "${JOUR_SRC_PATH}/eventparser.cpp"
  "${JOUR_SRC_PATH}/sysjournal.cpp"
  "${JOUR_SRC_PATH}/workjournal.cpp"
  "${JOUR_SRC_PATH}/measjournal.cpp"
  "${JOUR_SRC_PATH}/measparser.cpp"
  "${JOUR_SRC_PATH}/journaldialog.cpp"
  "${JOUR_SRC_PATH}/journaltabwidget.cpp"
  "${JOUR_SRC_PATH}/journalviewer.cpp"
)

# Create library target
add_library(${PROJECT_NAME} STATIC ${files_cpp} ${files_h})

# Including directories for library
target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

# Adding debug define for library
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG_JOURNALS)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE xml QXlsx)
