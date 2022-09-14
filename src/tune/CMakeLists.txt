cmake_minimum_required(VERSION 3.14)

project(tune LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

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

file(GLOB_RECURSE files_cpp *.cpp)
file(GLOB_RECURSE files_h *.h)

add_library(
  ${PROJECT_NAME} STATIC
  ${files_cpp}
  ${files_h})

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
  PRIVATE xml
  PRIVATE limereport
  PRIVATE datablocks)

target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)
