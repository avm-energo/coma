cmake_minimum_required(VERSION 3.14)

project(device LANGUAGES CXX)

set(INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include/device")
set(SRC_PATH     "${CMAKE_CURRENT_SOURCE_DIR}/src")

set(files_h
    "${INCLUDE_PATH}/bsi.h"
    "${INCLUDE_PATH}/bsi_ext.h"
    "${INCLUDE_PATH}/configstorage.h"
    "${INCLUDE_PATH}/current_device.h"
    "${INCLUDE_PATH}/file_provider.h"
    "${INCLUDE_PATH}/device_list.h"
    "${INCLUDE_PATH}/health.h"
    "${INCLUDE_PATH}/xml_settings.h"
)

set(files_cpp
    "${SRC_PATH}/bsi_ext.cpp"
    "${SRC_PATH}/configstorage.cpp"
    "${SRC_PATH}/current_device.cpp"
    "${SRC_PATH}/file_provider.cpp"
    "${SRC_PATH}/xml_settings.cpp"
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
