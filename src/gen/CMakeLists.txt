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
  settings.h
  board.cpp
  board.h
  colors.h
  stdfunc.h
  s2.h
  s2helper.h
  datamanager.h
  datarecv.h
  datarecv.cpp
  datatypes.h
  error.h
  error.cpp
  errorqueue.cpp
  files.cpp
  files.h
  helper.cpp
  helper.h
  logclass.cpp
  logclass.h
  logger.h
  logger.cpp
  pch.h
  uint24.h
  singleton.h
  timefunc.h
  timefunc.cpp
  datamanager.cpp
  stdfunc.cpp
  xmlparser.cpp
  xmlparser.h
  module_kxx.h
  modules.h
  registers.h
  errorqueue.h
  std_ext.h
  s2.cpp
  configv.cpp
  configv.h)

target_link_libraries(
  gen
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Xml
  PRIVATE Qt${QT_VERSION_MAJOR}::Network)

target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)
