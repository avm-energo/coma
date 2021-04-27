cmake_minimum_required(VERSION 3.14)

project(comaversion LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_SCRIPT_MODE_FILE ON)

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core]
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core
  REQUIRED)

find_package(Git REQUIRED)
execute_process(
  COMMAND ${GIT_EXECUTABLE} rev-parse --short=8 HEAD
  RESULT_VARIABLE SHORT_HASH_RESULT
  OUTPUT_VARIABLE SHORT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE)
set(gitHash ${SHORT_HASH})
message(${SHORT_HASH_RESULT} "Hash")
message(${SHORT_HASH} "Hash")
set(SHORT_HASH_FILE "version.txt")
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/gversion.h
  COMMAND echo "#define ${SHORT_HASH} $(git rev-parse head)"
  VERBATIM)
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
execute_process(
  COMMAND ${GIT_EXECUTABLE} rev-list --count HEAD
  RESULT_VARIABLE COMMIT_COUNT_RESULT
  OUTPUT_VARIABLE COMMIT_COUNT)
set(gitCommitCounter ${COMMIT_COUNT})
configure_file(version.h.in gversion.h @ONLY)
add_library(${PROJECT_NAME} STATIC comaversion.h comaversion.cpp)
set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE Qt${QT_VERSION_MAJOR}::Core)

target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)
