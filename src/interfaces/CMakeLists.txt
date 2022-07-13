cmake_minimum_required(VERSION 3.14)

project(interfaces LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(hidapi REQUIRED)

find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Network SerialPort
  REQUIRED)

file(GLOB_RECURSE files_cpp ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE files_h ${PROJECT_SOURCE_DIR}/*.h)

if (ENABLE_EMULATOR)
    list(APPEND files_cpp emulator.cpp )
    list(APPEND files_h emulator.h)
endif()

add_library(
  ${PROJECT_NAME} STATIC
  ${files_cpp} ${files_h} )

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE
  gen
  module
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Network
  Qt${QT_VERSION_MAJOR}::SerialPort)

if(UNIX)
  target_link_libraries(${PROJECT_NAME} PRIVATE hidapi-hidraw)
endif()
if(WIN32)
  target_link_libraries(${PROJECT_NAME} PRIVATE hidapi::hidapi)
endif()
target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)

message(STATUS "Interfaces import dirs" "${hidapi_LIB_DIRS}/../bin")
install(CODE "set(hidapi_BIN_DIRS \"${hidapi_LIB_DIRS}/../bin\")")
