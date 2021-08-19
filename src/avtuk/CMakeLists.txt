cmake_minimum_required(VERSION 3.14)

project(avtuk LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Network Xml Widgets
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Network Xml Widgets
  REQUIRED)

add_library(
  ${PROJECT_NAME} STATIC
  eoscillogram.cpp
  eoscillogram.h
  oscdialog.cpp
  oscdialog.h
  oscmanager.cpp
  oscmanager.h
  parseid10031.cpp
  parseid10031.h
  parseid9000.cpp
  parseid9000.h
  parseid9050.cpp
  parseid9050.h
  parsemodule.cpp
  parsemodule.h
  pushbuttondelegate.cpp
  pushbuttondelegate.h
  switchjournaldialog.cpp
  switchjournaldialog.h
  swjdialog.cpp
  swjdialog.h
  relaydialog.h
  relaydialog.cpp
  trendviewdialog.cpp
  trendviewdialog.h
  trendviewmodel.cpp
  trendviewmodel.h)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE qcustomplot
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
  PRIVATE Qt${QT_VERSION_MAJOR}::Xml
  PRIVATE Qt${QT_VERSION_MAJOR}::Network
  PRIVATE QXlsx)

target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)
