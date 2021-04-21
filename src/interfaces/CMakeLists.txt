cmake_minimum_required(VERSION 3.14)

project(interfaces LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpic")
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

add_library(interfaces STATIC
  baseinterface.cpp
  interfacesettings.h
  protocom_p.h
  protocom.h
  protocomprivate.h
  protocomthread.h
  settingstypes.h
  usbhidport.h
  usbhidportinfo.h
  usbhidsettings.h
  usbhidsettings.cpp
  usbhidportinfo.cpp
  usbhidport.cpp
  protocomthread.cpp
  protocom.cpp
  protocom_p.cpp
  interfacesettings.cpp
  baseinterface.h
)
set_property(TARGET interfaces PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(interfaces PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(interfaces PRIVATE hidapi-hidraw)
target_compile_definitions(interfaces PRIVATE INTERFACES_LIBRARY)
