cmake_minimum_required(VERSION 3.14)

project(device LANGUAGES CXX)

set(INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include/device")
set(SRC_PATH     "${CMAKE_CURRENT_SOURCE_DIR}/src")

set(files_h
    "${INCLUDE_PATH}/bsi.h"
    "${INCLUDE_PATH}/device_list.h"
)

set(files_cpp
    "${SRC_PATH}/bsi.cpp"
)

add_library(${PROJECT_NAME} STATIC ${files_cpp} ${files_h})

# Including directories for library
target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Linking dependencies
target_link_libraries(${PROJECT_NAME} PUBLIC interfaces)
