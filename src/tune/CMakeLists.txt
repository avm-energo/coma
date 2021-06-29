cmake_minimum_required(VERSION 3.14)

project(tune LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
find_package(Qt5Core)
find_package(
  QXlsx CONFIG REQUIRED)

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core
             Network
             Xml
             Widgets
             Svg
             PrintSupport
             Qml
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core
             Network
             Xml
             Widgets
             Svg
             PrintSupport
             Qml
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Widgets
  REQUIRED)

#link_directories(${LimeReportDir})
#include_directories(${LimeReportDir}/include)

add_library(
  ${PROJECT_NAME} STATIC
  abstracttunedialog.cpp
  abstracttunedialog.h
  generaltunedialog.cpp
  generaltunedialog.h
  tunesequencefile.cpp
  tunesequencefile.h
  tunesteps.h
  kiv/tunekivadc.cpp
  kiv/tunekivadc.h
  kiv/tunekivcheck.cpp
  kiv/tunekivcheck.h
  kiv/tunekiv.cpp
  kiv/tunekivdialog.cpp
  kiv/tunekivdialog.h
  kiv/tunekiv.h
  kiv/tunekivr.cpp
  kiv/tunekivr.h
  kiv/tunekivtemp60.cpp
  kiv/tunekivtemp60.h
  84/tune84adc.cpp
  84/tune84adc.h
  84/tune84check.cpp
  84/tune84check.h
  84/tune84dialog.cpp
  84/tune84dialog.h
  84/tune84temp60.cpp
  84/tune84temp60.h)
set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
  PRIVATE Qt${QT_VERSION_MAJOR}::Xml
  PRIVATE Qt${QT_VERSION_MAJOR}::Network
  PRIVATE Qt${QT_VERSION_MAJOR}::Svg
  PRIVATE Qt${QT_VERSION_MAJOR}::PrintSupport
  PRIVATE Qt${QT_VERSION_MAJOR}::Qml
  PRIVATE QXlsx
  PRIVATE limereport)

target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)
