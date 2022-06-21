cmake_minimum_required(VERSION 3.14)

project(s2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Network Xml
  REQUIRED)

add_library(
  s2 STATIC
  configv.h
  configv.cpp
  datarec.h
  datarecv.h
  datarecv.cpp
  filestruct.h
  filestruct.cpp
  s2.h
  s2.cpp
  s2datatypes.h
  s2helper.h
  s2helper.cpp
  s2settings.h)

target_link_libraries(
  s2
  PRIVATE gen
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Xml
  PRIVATE Qt${QT_VERSION_MAJOR}::Network)

target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)
