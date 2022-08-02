cmake_minimum_required(VERSION 3.14)

project(oscillograms LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Widgets Concurrent
  REQUIRED)

file(GLOB_RECURSE files_cpp *.cpp)
file(GLOB_RECURSE files_h *.h)

add_library(
  ${PROJECT_NAME} STATIC
  ${files_cpp}
  ${files_h}
)

target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC qcustomplot
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
  PRIVATE Qt${QT_VERSION_MAJOR}::Concurrent
  PUBLIC QXlsx)

target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)
