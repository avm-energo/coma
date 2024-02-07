cmake_minimum_required(VERSION 3.14)

project(interfaces LANGUAGES CXX)

find_package(Qt${QT_VERSION_MAJOR}
  COMPONENTS Network SerialPort
  REQUIRED)

set(INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces")
set(SRC_PATH     "${CMAKE_CURRENT_SOURCE_DIR}/src")

set(files_h
    "${INCLUDE_PATH}/connectioncontext.h"
    "${INCLUDE_PATH}/connectionmanager.h"
    "${INCLUDE_PATH}/moduledataupdater.h"
    "${INCLUDE_PATH}/conn/active_connection.h"
    "${INCLUDE_PATH}/conn/async_connection.h"
    "${INCLUDE_PATH}/conn/sync_connection.h"
    "${INCLUDE_PATH}/exec/default_query_executor.h"
    "${INCLUDE_PATH}/exec/iec104_query_executor.h"
    "${INCLUDE_PATH}/exec/query_executor_fabric.h"
    "${INCLUDE_PATH}/ifaces/baseinterface.h"
    "${INCLUDE_PATH}/ifaces/ethernet.h"
    "${INCLUDE_PATH}/ifaces/serialport.h"
    "${INCLUDE_PATH}/ifaces/usbhidport.h"
    "${INCLUDE_PATH}/parsers/base_request_parser.h"
    "${INCLUDE_PATH}/parsers/base_response_parser.h"
    "${INCLUDE_PATH}/parsers/iec104parser.h"
    "${INCLUDE_PATH}/parsers/iec104_request_parser.h"
    "${INCLUDE_PATH}/parsers/iec104_response_parser.h"
    "${INCLUDE_PATH}/parsers/modbus_request_parser.h"
    "${INCLUDE_PATH}/parsers/modbus_response_parser.h"
    "${INCLUDE_PATH}/parsers/protocom_request_parser.h"
    "${INCLUDE_PATH}/parsers/protocom_response_parser.h"
    "${INCLUDE_PATH}/types/base_settings.h"
    "${INCLUDE_PATH}/types/common_types.h"
    "${INCLUDE_PATH}/types/iec104/apci.h"
    "${INCLUDE_PATH}/types/iec104/asdu.h"
    "${INCLUDE_PATH}/types/iec104/asdu_unpacker.h"
    "${INCLUDE_PATH}/types/iec104/control_block.h"
    "${INCLUDE_PATH}/types/iec104/unnumbered_control.h"
    "${INCLUDE_PATH}/types/iec104_settings.h"
    "${INCLUDE_PATH}/types/iec104_types.h"
    "${INCLUDE_PATH}/types/protocol_settings.h"
    "${INCLUDE_PATH}/types/modbus_types.h"
    "${INCLUDE_PATH}/types/protocom_types.h"
    "${INCLUDE_PATH}/types/settingstypes.h"
    "${INCLUDE_PATH}/types/usbhidportinfo.h"
    "${INCLUDE_PATH}/types/usbhidsettings.h"
    "${INCLUDE_PATH}/utils/modbus_convertations.h"
    "${INCLUDE_PATH}/utils/request_queue.h"
    "${INCLUDE_PATH}/utils/slot_trait.h"
)

set(files_cpp
    "${SRC_PATH}/connectioncontext.cpp"
    "${SRC_PATH}/connectionmanager.cpp"
    "${SRC_PATH}/moduledataupdater.cpp"
    "${SRC_PATH}/conn/active_connection.cpp"
    "${SRC_PATH}/conn/async_connection.cpp"
    "${SRC_PATH}/conn/sync_connection.cpp"
    "${SRC_PATH}/exec/default_query_executor.cpp"
    "${SRC_PATH}/exec/iec104_query_executor.cpp"
    "${SRC_PATH}/exec/query_executor_fabric.cpp"
    "${SRC_PATH}/ifaces/baseinterface.cpp"
    "${SRC_PATH}/ifaces/ethernet.cpp"
    "${SRC_PATH}/ifaces/serialport.cpp"
    "${SRC_PATH}/ifaces/usbhidport.cpp"
    "${SRC_PATH}/parsers/base_request_parser.cpp"
    "${SRC_PATH}/parsers/base_response_parser.cpp"
    "${SRC_PATH}/parsers/iec104parser.cpp"
    "${SRC_PATH}/parsers/iec104_request_parser.cpp"
    "${SRC_PATH}/parsers/iec104_response_parser.cpp"
    "${SRC_PATH}/parsers/modbus_request_parser.cpp"
    "${SRC_PATH}/parsers/modbus_response_parser.cpp"
    "${SRC_PATH}/parsers/protocom_request_parser.cpp"
    "${SRC_PATH}/parsers/protocom_response_parser.cpp"
    "${SRC_PATH}/types/iec104/apci.cpp"
    "${SRC_PATH}/types/iec104/asdu.cpp"
    "${SRC_PATH}/types/iec104/asdu_unpacker.cpp"
    "${SRC_PATH}/types/iec104/control_block.cpp"
    "${SRC_PATH}/types/iec104/unnumbered_control.cpp"
    "${SRC_PATH}/types/protocol_settings.cpp"
    "${SRC_PATH}/types/usbhidportinfo.cpp"
    "${SRC_PATH}/types/usbhidsettings.cpp"
    "${SRC_PATH}/utils/request_queue.cpp"
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
target_link_libraries(${PROJECT_NAME} PUBLIC
  Qt${QT_VERSION_MAJOR}::Network
  Qt${QT_VERSION_MAJOR}::SerialPort
  s2 tl::expected
)

if(UNIX)
  target_link_libraries(${PROJECT_NAME} PRIVATE hidapi-hidraw)
endif()
if(WIN32)
  find_package(hidapi REQUIRED)
  target_link_libraries(${PROJECT_NAME} PRIVATE hidapi::hidapi)
endif()

# Adding test target
if(COMA_BUILD_TESTS)
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
    gen interfaces
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Test)

  # Adding target as test
  add_test(NAME ${PROJECT_NAME}_tests COMMAND ${PROJECT_NAME}_tests)
  # Black magic for ctest using under windows
  if(CMAKE_SYSTEM_NAME_LOWER STREQUAL "windows")
    get_filename_component(hidapi_BIN_DIR ${hidapi_LIB_DIRS}/../bin ABSOLUTE)
    set_tests_properties(${PROJECT_NAME}_tests PROPERTIES ENVIRONMENT
      "PATH=${hidapi_BIN_DIR}\;$<TARGET_FILE_DIR:Qt${QT_VERSION_MAJOR}::Core>\;$<TARGET_FILE_DIR:gen>\;")
  endif()
endif()

message(STATUS "Interfaces import dirs: ${hidapi_LIB_DIRS}/../bin")
install(CODE "set(hidapi_BIN_DIRS \"${hidapi_LIB_DIRS}/../bin\")")
