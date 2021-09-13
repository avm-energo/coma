cmake_minimum_required(VERSION 3.14)

project(qcustomplot LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Widgets
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Widgets
  REQUIRED)

add_library(${PROJECT_NAME} STATIC qcustomplot.cpp qcustomplot.h)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)

if (MSVC)
    string(REPLACE "/w14996" "/wd4996" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
else()
    #TODO make the same as msvc
endif()

