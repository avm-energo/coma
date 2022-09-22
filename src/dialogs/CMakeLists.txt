cmake_minimum_required(VERSION 3.14)

project(dialogs LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Widgets SerialPort Concurrent Svg
  REQUIRED)

file(GLOB_RECURSE files_cpp ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE files_h ${PROJECT_SOURCE_DIR}/*.h)

if (ENABLE_EMULATOR)
    list(APPEND files_cpp interfaceemudialog.cpp )
    list(APPEND files_h interfaceemudialog.h)
endif()

add_library(
  ${PROJECT_NAME} STATIC
  ${files_cpp} ${files_h} )

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE interfaces
  PRIVATE oscillograms
  PRIVATE models
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
  PRIVATE Qt${QT_VERSION_MAJOR}::SerialPort
  PRIVATE Qt${QT_VERSION_MAJOR}::Concurrent
  PRIVATE Qt${QT_VERSION_MAJOR}::Svg
  PRIVATE QXlsx
  PRIVATE qcustomplot)

target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)

if (ENABLE_EMULATOR)
    target_compile_definitions(${PROJECT_NAME} PRIVATE ENABLE_EMULATOR)
endif()
