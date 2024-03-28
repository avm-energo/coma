cmake_minimum_required(VERSION 3.14)

project(alarms LANGUAGES CXX)

set(INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include/alarms")
set(SRC_PATH     "${CMAKE_CURRENT_SOURCE_DIR}/src")

set(files_h
    "${INCLUDE_PATH}/alarmbutton.h"
    "${INCLUDE_PATH}/alarmstateall.h"
    "${INCLUDE_PATH}/alarmwidget.h"
    "${INCLUDE_PATH}/basealarm.h"
    "${INCLUDE_PATH}/modulealarm.h"
)

set(files_cpp
    "${SRC_PATH}/alarmbutton.cpp"
    "${SRC_PATH}/alarmstateall.cpp"
    "${SRC_PATH}/alarmwidget.cpp"
    "${SRC_PATH}/basealarm.cpp"
    "${SRC_PATH}/modulealarm.cpp"
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
target_link_libraries(${PROJECT_NAME} PUBLIC widgets)
