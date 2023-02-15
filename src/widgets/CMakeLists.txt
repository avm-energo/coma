cmake_minimum_required(VERSION 3.14)

project(widgets LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Network Widgets Xml
  REQUIRED)

find_package(Boost
  COMPONENTS headers
  REQUIRED)

set(QDARKSTYLESHEET_PATH ${CMAKE_CURRENT_SOURCE_DIR}/styles/QDarkStyleSheet/qdarkstyle)

file(GLOB_RECURSE files_cpp ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE files_h ${PROJECT_SOURCE_DIR}/*.h)

add_library(
  ${PROJECT_NAME} STATIC
  ${files_cpp} ${files_h}
  ../../images/vectorIcons.qrc
  ../../settings/settings.qrc
  ${QDARKSTYLESHEET_PATH}/light/lightstyle.qrc
  ${QDARKSTYLESHEET_PATH}/dark/darkstyle.qrc
  styles/styles.qrc)

target_link_libraries(${PROJECT_NAME} PRIVATE
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Network
  Qt${QT_VERSION_MAJOR}::Widgets
  Qt${QT_VERSION_MAJOR}::Xml
  Boost::headers
  QXlsx
  dialogs
  module
  oscillograms
  s2
  xml)

target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)
