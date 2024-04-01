cmake_minimum_required(VERSION 3.14)

project(oscillograms LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Collecting sources
set(OSC_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include/oscillograms")
set(OSC_SRC_PATH     "${CMAKE_CURRENT_SOURCE_DIR}/src")
file(GLOB_RECURSE files_h   ${OSC_INCLUDE_PATH}/*.h)
file(GLOB_RECURSE files_cpp ${OSC_SRC_PATH}/*.cpp)

# Creating library target
add_library(${PROJECT_NAME} STATIC ${files_cpp} ${files_h})

# Including directories for library
target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

# Linking library dependencies
target_link_libraries(${PROJECT_NAME} PUBLIC widgets qcustomplot QXlsx)
