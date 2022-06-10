cmake_minimum_required(VERSION 3.14)

project(gen LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Network Xml
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Network Xml
  REQUIRED)

add_library(
  gen STATIC
  board.cpp
  board.h
  colors.h
  datatypes.h
  error.h
  error.cpp
  errorqueue.h
  errorqueue.cpp
  files.cpp
  files.h
  helper.h
  helper.cpp
  logclass.cpp
  logclass.h
  logger.h
  logger.cpp
  module_kxx.h
  modules.h
  pch.h
  registers.h
  settings.h
  singleton.h
  stdfunc.h
  stdfunc.cpp
  std_ext.h
  timefunc.h
  timefunc.cpp
  uint24.h)

target_link_libraries(
  gen
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Xml
  PRIVATE Qt${QT_VERSION_MAJOR}::Network)

target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)
