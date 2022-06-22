cmake_minimum_required(VERSION 3.14)

project(oscillograms LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Widgets
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Widgets
  REQUIRED)

add_library(
  ${PROJECT_NAME} STATIC
  filemanager.h
  swjmanager.cpp
  swjmanager.h
  oscdialog.cpp
  oscdialog.h
  oscmanager.cpp
  oscmanager.h
  parseid10001.cpp
  parseid10001.h
  parseid10020.cpp
  parseid10020.h
  parseid10030.h
  parseid10030.cpp
  # TODO Check with real avtuk
  # parseid9050.cpp
  # parseid9050.h
  parsemodule.h
  parsemodule.cpp
  trendview/trendviewdialog.h
  trendview/trendviewdialog.cpp
  trendview/trendviewmodel.h
  trendview/trendviewmodel.cpp
  trendview/trendviewmodel21.h
  trendview/trendviewmodel21.cpp
  trendview/trendviewmodel80.h
  trendview/trendviewmodel80.cpp
  trendview/trendviewmodel85.h
  trendview/trendviewmodel85.cpp
  trendview/trendviewmodels.h
)

target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC qcustomplot
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)
