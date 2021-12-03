cmake_minimum_required(VERSION 3.14)

project(config LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Network Widgets
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Network Xml Widgets
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Widgets
  REQUIRED)

add_library(${PROJECT_NAME} STATIC configdialog.cpp configdialog.h
                                   widgetfactory.cpp widgetfactory.h)

option(DEBUG_WIDGET_FACTORY "Enable debug logging for WidgetFactory" OFF)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
  PRIVATE Qt${QT_VERSION_MAJOR}::Xml
  PRIVATE Qt${QT_VERSION_MAJOR}::Network)

target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)

if(DEBUG_WIDGET_FACTORY)
  target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG_FACTORY)
endif(DEBUG_WIDGET_FACTORY)
