cmake_minimum_required(VERSION 3.14)

project(module LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Network Xml Widgets
  REQUIRED)

#WARN Do not use in release build
option(USE_BUILTIN_XML "Use builtin xml configs instead of locally stored" OFF)

file(GLOB_RECURSE files_cpp ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE files_h ${PROJECT_SOURCE_DIR}/*.h)

add_library(
  ${PROJECT_NAME} STATIC
  ${files_cpp} ${files_h}
  ../../settings/settings.qrc)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE
  comaversion
  dialogs
  gen
  models
  s2
  widgets
  xml
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Widgets
  Qt${QT_VERSION_MAJOR}::Xml
  Qt${QT_VERSION_MAJOR}::Network
  PUBLIC QXlsx)

target_compile_definitions(${PROJECT_NAME} PRIVATE INTERFACES_LIBRARY)

if(USE_BUILTIN_XML)
   target_compile_definitions(${PROJECT_NAME} PRIVATE USE_INTERNAL_RCS)
endif()
