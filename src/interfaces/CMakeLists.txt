cmake_minimum_required(VERSION 3.14)

project(interfaces LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Core)
find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Network SerialPort
  REQUIRED)
find_package(
  Qt5
  COMPONENTS Core Network SerialPort
  REQUIRED)

add_library(
  ${PROJECT_NAME} STATIC
  baseinterface.cpp
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
target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network
          Qt${QT_VERSION_MAJOR}::SerialPort)
if (UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE hidapi-hidraw)
endif()
if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE hidapi)
endif()
target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)
