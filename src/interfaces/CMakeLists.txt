cmake_minimum_required(VERSION 3.14)

project(interfaces LANGUAGES CXX)

find_package(Qt${QT_VERSION_MAJOR}
  COMPONENTS Network SerialPort
  REQUIRED)

set(INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces")
set(SRC_PATH     "${CMAKE_CURRENT_SOURCE_DIR}/src")

file(GLOB_RECURSE files_h   ${INCLUDE_PATH}/*.h)
file(GLOB_RECURSE files_cpp ${SRC_PATH}/*.cpp)

add_library(${PROJECT_NAME} STATIC ${files_cpp} ${files_h}
  src/types/base_settings.cpp)

# Including directories for library
target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

# Visual Studio C++ compile options
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(${PROJECT_NAME} PRIVATE /W4)
# GCC and Clang compile options
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Linking hidapi library
  message(STATUS "Using external hidapi")
  include(hidapi)
  message(STATUS "HidAPI source dir: ${hidapi_SOURCE_DIR}")
  message(STATUS "HidAPI lib dir: ${hidapi_BINARY_DIR}")
  target_include_directories(${PROJECT_NAME} PRIVATE ${hidapi_SOURCE_DIR})

  # Linking dependencies
  target_link_libraries(${PROJECT_NAME} PUBLIC
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::SerialPort
    avm-gen
    coma
  )

if(SYSTEM_NAME STREQUAL "windows")
    target_link_libraries(${PROJECT_NAME} PUBLIC
      hidapi
    )
elseif(SYSTEM_NAME STREQUAL "linux")
    target_link_libraries(${PROJECT_NAME} PUBLIC
      hidapi-hidraw
    )
endif()

  # Adding test target
if(BUILD_TESTING)
  find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test REQUIRED)
  set(INTERFACES_TESTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")
  set(INTERFACES_TESTS_SOURCES
    "${INTERFACES_TESTS_DIR}/interfaces_test_obj.h"
    "${INTERFACES_TESTS_DIR}/interfaces_test_obj.cpp"
    "${INTERFACES_TESTS_DIR}/modbus_compat_test.cpp"
    "${INTERFACES_TESTS_DIR}/iec104_apci_asdu_test.cpp"
    "${INTERFACES_TESTS_DIR}/iec104_cblock_test.cpp")

  # interfaces_tests target
  add_executable(${PROJECT_NAME}_tests ${INTERFACES_TESTS_SOURCES})
  target_include_directories(${PROJECT_NAME}_tests PUBLIC
    "$<BUILD_INTERFACE:${INTERFACES_TESTS_DIR}>")
  target_link_libraries(${PROJECT_NAME}_tests PUBLIC
    interfaces
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Test
    avm-gen
)

if(SYSTEM_NAME STREQUAL "windows")
    target_link_libraries(${PROJECT_NAME}_tests PUBLIC
      hidapi
    )
elseif(SYSTEM_NAME STREQUAL "linux")
    target_link_libraries(${PROJECT_NAME}_tests PUBLIC
      hidapi-hidraw
    )
endif()


  # Adding target as test
  add_test(NAME ${PROJECT_NAME}_tests COMMAND ${PROJECT_NAME}_tests)
  # Black magic for ctest using under windows
  if(SYSTEM_NAME STREQUAL "windows")
    get_filename_component(hidapi_BIN_DIR ${hidapi_BINARY_DIR} ABSOLUTE)
    set_tests_properties(${PROJECT_NAME}_tests PROPERTIES ENVIRONMENT
      "PATH=${hidapi_BIN_DIR}\;$<TARGET_FILE_DIR:Qt${QT_VERSION_MAJOR}::Core>\;$<TARGET_FILE_DIR:avm-gen>\;")
  endif()
endif()
