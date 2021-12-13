cmake_minimum_required(VERSION 3.14)

project(startup LANGUAGES CXX)

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
  abstractstartupdialog.cpp
  abstractstartupdialog.h
  startupkdvdialog.cpp
  startupkdvdialog.h
  startupkivdialog.cpp
  startupkivdialog.h
  startupktfdialog.cpp
  startupktfdialog.h)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Widgets)

target_compile_definitions(${PROJECT_NAME} PRIVATE INTERFACES_LIBRARY)
