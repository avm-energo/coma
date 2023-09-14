cmake_minimum_required(VERSION 3.14)

project(interfaces LANGUAGES CXX)

find_package(Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Network SerialPort
  REQUIRED)

set(INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include/interfaces")
set(SRC_PATH     "${CMAKE_CURRENT_SOURCE_DIR}/src")

set(files_h
    "${INCLUDE_PATH}/baseinterface.h"
    "${INCLUDE_PATH}/baseinterfacethread.h"
    "${INCLUDE_PATH}/baseport.h"
    "${INCLUDE_PATH}/emulator.h"
    "${INCLUDE_PATH}/ethernet.h"
    "${INCLUDE_PATH}/iec104.h"
    "${INCLUDE_PATH}/iec104private.h"
    "${INCLUDE_PATH}/iec104thread.h"
    "${INCLUDE_PATH}/interfacesettings.h"
    "${INCLUDE_PATH}/modbus.h"
    "${INCLUDE_PATH}/modbusprivate.h"
    "${INCLUDE_PATH}/modbusthread.h"
    "${INCLUDE_PATH}/moduledataupdater.h"
    "${INCLUDE_PATH}/protocom.h"
    "${INCLUDE_PATH}/protocomprivate.h"
    "${INCLUDE_PATH}/protocomthread.h"
    "${INCLUDE_PATH}/serialport.h"
    "${INCLUDE_PATH}/settingstypes.h"
    "${INCLUDE_PATH}/usbhidport.h"
    "${INCLUDE_PATH}/usbhidportinfo.h"
    "${INCLUDE_PATH}/usbhidsettings.h"
)

set(files_cpp
    "${SRC_PATH}/baseinterface.cpp"
    "${SRC_PATH}/baseinterfacethread.cpp"
    "${SRC_PATH}/baseport.cpp"
    "${SRC_PATH}/emulator.cpp"
    "${SRC_PATH}/ethernet.cpp"
    "${SRC_PATH}/iec104.cpp"
    "${SRC_PATH}/iec104thread.cpp"
    "${SRC_PATH}/modbus.cpp"
    "${SRC_PATH}/modbusprivate.cpp"
    "${SRC_PATH}/modbusthread.cpp"
    "${SRC_PATH}/moduledataupdater.cpp"
    "${SRC_PATH}/protocom.cpp"
    "${SRC_PATH}/protocomthread.cpp"
    "${SRC_PATH}/serialport.cpp"
    "${SRC_PATH}/usbhidport.cpp"
    "${SRC_PATH}/usbhidportinfo.cpp"
    "${SRC_PATH}/usbhidsettings.cpp"
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
