cmake_minimum_required(VERSION 3.14)

project(models LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpic")
find_package(Qt5Core)
find_package(
  QXlsx CONFIG REQUIRED PATHS
  ${CMAKE_CURRENT_SOURCE_DIR}/../../share/QXlsx/cmake
  ${CMAKE_CURRENT_SOURCE_DIR}/../../share/QXlsx)
# find_package(LimeReport CONFIG REQUIRED PATHS
# ${CMAKE_CURRENT_SOURCE_DIR}/../../include/LimeReport-bin )
set(LimeReportDir
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/LimeReport/build/5.15.2/linux64/release/lib
)
link_directories(${LimeReportDir})
include_directories(${LimeReportDir}/include)
# add_library(limereport SHARED IMPORTED)
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
  report.cpp
  report.h)
set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
  PRIVATE Qt${QT_VERSION_MAJOR}::Xml
  PRIVATE Qt${QT_VERSION_MAJOR}::Network
  PRIVATE Qt${QT_VERSION_MAJOR}::PrintSupport
  PRIVATE Qt${QT_VERSION_MAJOR}::Qml
  PRIVATE QXlsx
  PRIVATE limereport)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)
