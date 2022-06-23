cmake_minimum_required(VERSION 3.15)

project(s2 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Xml Widgets
  REQUIRED)

add_library(
  ${PROJECT_NAME} STATIC
  #configv.cpp
  #configv.h
  #s2.cpp
  #s2.h
  #s2helper.h
  ../../settings/settings.qrc)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE gen
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Network
  PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
  PRIVATE Qt${QT_VERSION_MAJOR}::Xml)

target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)
