cmake_minimum_required(VERSION 3.14)

project(interfaces LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(hidapi REQUIRED)

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Network SerialPort
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Network SerialPort
  REQUIRED)

set(SRCS   baseinterface.cpp
    baseinterface.h
    ethernet.cpp
    ethernet.h
    iec104.cpp
    iec104.h
    iec104private.h
    iec104thread.cpp
    iec104thread.h
    interfacesettings.cpp
    interfacesettings.h
    modbus.cpp
    modbus.h
    modbusprivate.cpp
    modbusprivate.h
    modbusthread.cpp
    modbusthread.h
    protocom.cpp
    protocom.h
    protocom_p.cpp
    protocom_p.h
    protocomprivate.h
    protocomthread.cpp
    protocomthread.h
    serialport.cpp
    serialport.h
    settingstypes.h
    usbhidport.cpp
    usbhidport.h
    usbhidportinfo.cpp
    usbhidportinfo.h
    usbhidsettings.cpp
    usbhidsettings.h)

if (ENABLE_EMULATOR)
    list(APPEND SRCS emulator.cpp emulator.h)
endif()

add_library(
  ${PROJECT_NAME} STATIC
  ${SRCS}
)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE
  gen
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
