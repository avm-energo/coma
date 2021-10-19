cmake_minimum_required(VERSION 3.14)

project(models LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Xml Widgets PrintSupport Qml
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Xml Widgets PrintSupport Qml
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Widgets
  REQUIRED)

add_library(
  ${PROJECT_NAME} STATIC
  abstracttablerow.h
  comboboxdelegate.cpp
  comboboxdelegate.h
  errorprotocolitem.cpp
  errorprotocolitem.h
  errorprotocolmodel.cpp
  errorprotocolmodel.h
  etablemodel.cpp
  etablemodel.h
  etablerow.cpp
  etablerow.h
  edynamictablemodel.cpp
  edynamictablemodel.h
  report.cpp
  report.h)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Widgets
  Qt${QT_VERSION_MAJOR}::Xml
  Qt${QT_VERSION_MAJOR}::PrintSupport
  Qt${QT_VERSION_MAJOR}::Qml
  PUBLIC limereport QXlsx)

target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)
