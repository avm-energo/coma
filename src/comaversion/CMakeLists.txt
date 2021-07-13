cmake_minimum_required(VERSION 3.14)

project(comaversion LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_SCRIPT_MODE_FILE ON)

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core]
  REQUIRED)
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
add_library(${PROJECT_NAME} STATIC comaversion.h comaversion.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core)

target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)
set(CMAKE_SCRIPT_MODE_FILE OFF)
