cmake_minimum_required(VERSION 3.14)

project(models LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Core)

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Network Xml Widgets PrintSupport Qml
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Network Xml Widgets PrintSupport Qml
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
  PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
  PRIVATE Qt${QT_VERSION_MAJOR}::Xml
  PRIVATE Qt${QT_VERSION_MAJOR}::Network
  PRIVATE Qt${QT_VERSION_MAJOR}::PrintSupport
  PRIVATE Qt${QT_VERSION_MAJOR}::Qml
  PRIVATE QXlsx
  PUBLIC limereport)

#add_dependencies(${PROJECT_NAME} QXlsxBuild LimeReportBuild)
target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)
