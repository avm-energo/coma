cmake_minimum_required(VERSION 3.14)

project(dialogs LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
find_package(Qt5Core)
find_package(
  QXlsx CONFIG REQUIRED PATHS
  ${CMAKE_CURRENT_SOURCE_DIR}/../../share/QXlsx/cmake
  ${CMAKE_CURRENT_SOURCE_DIR}/../../share/QXlsx)

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Network Xml Widgets SerialPort Concurrent
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Network Xml Widgets SerialPort Concurrent
  REQUIRED)

add_library(
  ${PROJECT_NAME} STATIC
  abstractinterfacedialog.cpp
  abstractinterfacedialog.h
  connectdialog.cpp
  connectdialog.h
  errordialog.cpp
  errordialog.h
  fwuploaddialog.cpp
  fwuploaddialog.h
  infodialog.cpp
  infodialog.h
  interfaceethernetdialog.cpp
  interfaceethernetdialog.h
  interfaceserialdialog.cpp
  interfaceserialdialog.h
  interfaceusbdialog.cpp
  interfaceusbdialog.h
  journalsdialog.cpp
  journalsdialog.h
  keypressdialog.cpp
  keypressdialog.h
  settingsdialog.cpp
  settingsdialog.h
  timedialog.cpp
  timedialog.h)
set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE interfaces
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
  PRIVATE Qt${QT_VERSION_MAJOR}::Xml
  PRIVATE Qt${QT_VERSION_MAJOR}::Network
  PRIVATE Qt${QT_VERSION_MAJOR}::SerialPort
  PRIVATE Qt${QT_VERSION_MAJOR}::Concurrent
  PRIVATE QXlsx)
target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)
