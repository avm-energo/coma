cmake_minimum_required(VERSION 3.14)

project(check LANGUAGES CXX)

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
  abstractcheckdialog.cpp
  abstractcheckdialog.h
  delegate_check.h
  check.cpp
  check.h
  checkkdvdialog.cpp
  checkkdvdialog.h
  checkkdvharmonicdialog.cpp
  checkkdvharmonicdialog.h
  checkkdvvibrdialog.cpp
  checkkdvvibrdialog.h
  checkkivdialog.cpp
  checkkivdialog.h
  checkktfdialog.cpp
  checkktfdialog.h
  checkktfharmonicdialog.cpp
  checkktfharmonicdialog.h
  check3533dialog.h
  check3533dialog.cpp
  signaldialog.cpp
  signaldialog.h
  signaldialog84.h
  signaldialog84.cpp)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
  PRIVATE QXlsx)

target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)
