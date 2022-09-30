cmake_minimum_required(VERSION 3.14)

project(startup LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Widgets
  REQUIRED)

file(GLOB_RECURSE files_cpp ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE files_h ${PROJECT_SOURCE_DIR}/*.h)

add_library(
  ${PROJECT_NAME} STATIC
  ${files_cpp} ${files_h} )

target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC gen
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Widgets)

target_compile_definitions(${PROJECT_NAME} PRIVATE INTERFACES_LIBRARY)
