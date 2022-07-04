cmake_minimum_required(VERSION 3.15)

project(xml LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Xml Widgets
  REQUIRED)

add_library(
  ${PROJECT_NAME} STATIC
  xmlconfigparser.cpp
  xmlconfigparser.h
  xmleditor/xmleditcheckparser.cpp
  xmleditor/xmleditcheckparser.h
  xmleditor/xmleditor.cpp
  xmleditor/xmleditor.h
  xmleditor/xmlproxymodel.cpp
  xmleditor/xmlproxymodel.h
  xmleditor/models/xmlmodel.h
  xmleditor/models/xmlmodel.cpp
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
