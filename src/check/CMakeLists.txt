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

find_package(Boost
    COMPONENTS headers
    REQUIRED)

add_library(
  ${PROJECT_NAME} STATIC
  abstractcheckdialog.cpp
  abstractcheckdialog.h
  delegate_check.h
  check3133dialog.h
  check3133dialog.cpp
  signaldialog.cpp
  signaldialog.h
  signaldialog84.h
  signaldialog84.cpp)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
  PRIVATE QXlsx
  PRIVATE qcustomplot
  PRIVATE Boost::headers)

target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)
