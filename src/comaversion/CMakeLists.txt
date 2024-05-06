cmake_minimum_required(VERSION 3.14)

project(comaversion LANGUAGES CXX)

set(CMAKE_SCRIPT_MODE_FILE ON)

find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core
  REQUIRED)

if(NOT SHORT_HASH)
    find_package(Git REQUIRED)
    execute_process(
      COMMAND ${GIT_EXECUTABLE} rev-parse --short=8 HEAD
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      RESULT_VARIABLE SHORT_HASH_RESULT
      OUTPUT_VARIABLE SHORT_HASH
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(gitHash ${SHORT_HASH})
    if (${SHORT_HASH_RESULT})
       message(STATUS ${SHORT_HASH_RESULT} " - could not get git hash")
    endif()
endif()

message(STATUS "Commit short hash: " ${SHORT_HASH})
set(SHORT_HASH_FILE ${CMAKE_CURRENT_BINARY_DIR}/version.txt)

if(CMAKE_SCRIPT_MODE_FILE)
  if(EXISTS "${SHORT_HASH_FILE}")
    file(READ ${SHORT_HASH_FILE} READ_IN_SHORT_HASH)
  else()
    set(READ_IN_SHORT_HASH "")
  endif()
  if(NOT ("${READ_IN_SHORT_HASH}" STREQUAL "${SHORT_HASH}"))
    message(STATUS "Short hash is out of date")
    # This will update short_hash.txt, causing cmake to reconfigure
    file(WRITE ${SHORT_HASH_FILE} ${SHORT_HASH})
  endif()
endif()
set(gitHash ${SHORT_HASH})

if (NOT COMMIT_COUNT)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-list --count HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    RESULT_VARIABLE COMMIT_COUNT_RESULT
    OUTPUT_VARIABLE COMMIT_COUNT)
endif()

set(gitCommitCounter ${COMMIT_COUNT})
configure_file(version.h.in ${CMAKE_CURRENT_BINARY_DIR}/gversion.h @ONLY)

set(COMAVERSION_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}")
set(COMAVERSION_SRC     "${CMAKE_CURRENT_SOURCE_DIR}/src/")

set(COMAVERSION_SOURCES
  "${COMAVERSION_INCLUDE}/comaversion.h"
  "${COMAVERSION_SRC}/comaversion.cpp")

add_library(${PROJECT_NAME} STATIC ${COMAVERSION_SOURCES})

# Including private directories for library
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
# Including public directories for library
target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core)
set(CMAKE_SCRIPT_MODE_FILE OFF)
