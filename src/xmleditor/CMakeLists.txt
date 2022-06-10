cmake_minimum_required(VERSION 3.14)

project(xmleditor LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Xml Widgets
  REQUIRED)

add_library(
  ${PROJECT_NAME} STATIC
  moduleseditor.cpp
  moduleseditor.h
  xmlconfigparser.cpp
  xmlconfigparser.h
  xmlparser.cpp
  xmlparser.h
  ../../settings/settings.qrc)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE gen
  PRIVATE s2
  # TODO: Remove this dependence
  PRIVATE interfaces
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Network
  PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
  PRIVATE Qt${QT_VERSION_MAJOR}::Xml)

target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)
