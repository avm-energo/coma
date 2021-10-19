cmake_minimum_required(VERSION 3.14)

project(module LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Network Xml Widgets
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Network Xml Widgets
  REQUIRED)

#WARN Do not use in release build
option(USE_BUILTIN_XML "Use builtin xml configs instead of locally stored" OFF)

add_library(
  ${PROJECT_NAME} STATIC
  alarmstateall.cpp
  alarmstateall.h
  basealarm.cpp
  basealarm.h
  journals.cpp
  journals.h
  journkdv.cpp
  journkdv.h
  journkiv.cpp
  journkiv.h
  journktf.cpp
  journktf.h
  modulealarm.cpp
  modulealarm.h
  module.cpp
  module.h
  ../../settings/settings.qrc)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE
  gen
  widgets
  config
  comaversion
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Widgets
  Qt${QT_VERSION_MAJOR}::Xml
  Qt${QT_VERSION_MAJOR}::Network
  PUBLIC QXlsx)

target_compile_definitions(${PROJECT_NAME} PRIVATE INTERFACES_LIBRARY)

if(USE_BUILTIN_XML)
   target_compile_definitions(${PROJECT_NAME} PRIVATE USE_INTERNAL_RCS)
endif()
