cmake_minimum_required(VERSION 3.14)

project(journals LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Widgets
  REQUIRED)

#file(GLOB_RECURSE files_cpp ${PROJECT_SOURCE_DIR}/*.cpp)
#file(GLOB_RECURSE files_h ${PROJECT_SOURCE_DIR}/*.h)

set(files_h
    basejournal.h
    journals.h
    journkdv.h
    journkiv.h
    journktf.h
)
set(files_cpp
    basejournal.cpp
    journals.cpp
    journkdv.cpp
    journkiv.cpp
    journktf.cpp
)

add_library(${PROJECT_NAME} STATIC ${files_cpp} ${files_h})

target_link_libraries(${PROJECT_NAME} PRIVATE
  module
  models
  widgets
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Widgets
  gen
  QXlsx)
