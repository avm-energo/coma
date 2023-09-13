cmake_minimum_required(VERSION 3.14)

project(interfaces LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Network SerialPort
  REQUIRED)

#file(GLOB_RECURSE files_cpp ${PROJECT_SOURCE_DIR}/*.cpp EXCLUDE ethernet.cpp)
#file(GLOB_RECURSE files_h ${PROJECT_SOURCE_DIR}/*.h EXCLUDE ethernet.h)

set(files_h
    baseinterface.h
    baseinterfacethread.h
    baseport.h
    emulator.h
    ethernet.h
    iec104.h
    iec104private.h
    iec104thread.h
    interfacesettings.h
    modbus.h
    modbusprivate.h
    modbusthread.h
    moduledataupdater.h
    protocom.h
    protocomprivate.h
    protocomthread.h
    serialport.h
    settingstypes.h
    usbhidport.h
    usbhidportinfo.h
    usbhidsettings.h
)

set(files_cpp
    baseinterface.cpp
    baseinterfacethread.cpp
    baseport.cpp
    emulator.cpp
    ethernet.cpp
    iec104.cpp
    iec104thread.cpp
    modbus.cpp
    modbusprivate.cpp
    modbusthread.cpp
    moduledataupdater.cpp
    protocom.cpp
    protocomthread.cpp
    serialport.cpp
    usbhidport.cpp
    usbhidportinfo.cpp
    usbhidsettings.cpp
)

add_library(${PROJECT_NAME} STATIC ${files_cpp} ${files_h})

target_link_libraries(${PROJECT_NAME} PRIVATE
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Network
  Qt${QT_VERSION_MAJOR}::SerialPort
  Qt${QT_VERSION_MAJOR}::Widgets
  s2
  PUBLIC gen
)

if(UNIX)
  target_link_libraries(${PROJECT_NAME} PRIVATE hidapi-hidraw)
endif()
if(WIN32)
  find_package(hidapi REQUIRED)
  target_link_libraries(${PROJECT_NAME} PRIVATE hidapi::hidapi)
endif()
target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)

message(STATUS "Interfaces import dirs: ${hidapi_LIB_DIRS}/../bin")
install(CODE "set(hidapi_BIN_DIRS \"${hidapi_LIB_DIRS}/../bin\")")
