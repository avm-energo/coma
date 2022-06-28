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
  colors.h
  datatypes.h
  datamanager/datamanager.h
  datamanager/datamanager.cpp
  datamanager/typesproxy.h
  datamanager/typesproxy.cpp
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
