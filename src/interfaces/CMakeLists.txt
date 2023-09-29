cmake_minimum_required(VERSION 3.14)

project(interfaces LANGUAGES CXX)

find_package(Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Network SerialPort
  REQUIRED)

set(INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces")
set(SRC_PATH     "${CMAKE_CURRENT_SOURCE_DIR}/src")

set(files_h
    "${INCLUDE_PATH}/connection.h"
    "${INCLUDE_PATH}/connectioncontext.h"
    "${INCLUDE_PATH}/connectionmanager.h"
    "${INCLUDE_PATH}/moduledataupdater.h"
    #"${INCLUDE_PATH}/conn/emulator.h"
    #"${INCLUDE_PATH}/conn/iec104.h"
    #"${INCLUDE_PATH}/conn/modbus.h"
    #"${INCLUDE_PATH}/conn/protocom.h"
    "${INCLUDE_PATH}/ports/baseport.h"
    "${INCLUDE_PATH}/ports/ethernet.h"
    "${INCLUDE_PATH}/ports/serialport.h"
    "${INCLUDE_PATH}/ports/usbhidport.h"
    "${INCLUDE_PATH}/threads/baseconnectionthread.h"
    "${INCLUDE_PATH}/threads/iec104thread.h"
    "${INCLUDE_PATH}/threads/modbusthread.h"
    "${INCLUDE_PATH}/threads/protocomthread.h"
    "${INCLUDE_PATH}/types/common_types.h"
    "${INCLUDE_PATH}/types/iec104_types.h"
    "${INCLUDE_PATH}/types/interfacesettings.h"
    "${INCLUDE_PATH}/types/modbus_types.h"
    "${INCLUDE_PATH}/types/protocom_types.h"
    "${INCLUDE_PATH}/types/settingstypes.h"
    "${INCLUDE_PATH}/types/usbhidportinfo.h"
    "${INCLUDE_PATH}/types/usbhidsettings.h"
    "${INCLUDE_PATH}/utils/request_queue.h"
    "${INCLUDE_PATH}/utils/slot_trait.h"
)

set(files_cpp
    "${SRC_PATH}/connection.cpp"
    "${SRC_PATH}/connectioncontext.cpp"
    "${SRC_PATH}/connectionmanager.cpp"
    "${SRC_PATH}/moduledataupdater.cpp"
    #"${SRC_PATH}/conn/emulator.cpp"
    #"${SRC_PATH}/conn/iec104.cpp"
    #"${SRC_PATH}/conn/modbus.cpp"
    #"${SRC_PATH}/conn/protocom.cpp"
    "${SRC_PATH}/ports/baseport.cpp"
    "${SRC_PATH}/ports/ethernet.cpp"
    "${SRC_PATH}/ports/serialport.cpp"
    "${SRC_PATH}/ports/usbhidport.cpp"
    "${SRC_PATH}/threads/baseconnectionthread.cpp"
    "${SRC_PATH}/threads/iec104thread.cpp"
    "${SRC_PATH}/threads/modbusthread.cpp"
    "${SRC_PATH}/threads/protocomthread.cpp"
    "${SRC_PATH}/types/usbhidportinfo.cpp"
    "${SRC_PATH}/types/usbhidsettings.cpp"
    "${SRC_PATH}/utils/request_queue.cpp"
)

add_library(${PROJECT_NAME} STATIC ${files_cpp} ${files_h})

# Including directories for library
target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_link_libraries(${PROJECT_NAME} PUBLIC
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Network
  Qt${QT_VERSION_MAJOR}::SerialPort
  Qt${QT_VERSION_MAJOR}::Widgets
  s2
)

if(UNIX)
  target_link_libraries(${PROJECT_NAME} PRIVATE hidapi-hidraw)
endif()
if(WIN32)
  find_package(hidapi REQUIRED)
  target_link_libraries(${PROJECT_NAME} PRIVATE hidapi::hidapi)
endif()

message(STATUS "Interfaces import dirs: ${hidapi_LIB_DIRS}/../bin")
install(CODE "set(hidapi_BIN_DIRS \"${hidapi_LIB_DIRS}/../bin\")")
