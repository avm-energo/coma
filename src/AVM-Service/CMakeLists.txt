project(AVM-Service)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(PROGNAME "AVM-Service")
set(COMAVERSION 2.0.0)
configure_file(cconfig.h.in config.h @ONLY)
find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Xml Widgets Network
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Xml Widgets Network
  REQUIRED)
find_package(
  QXlsx CONFIG REQUIRED PATHS
  ${CMAKE_CURRENT_SOURCE_DIR}/../../share/QXlsx/cmake
  ${CMAKE_CURRENT_SOURCE_DIR}/../../share/QXlsx)

link_directories(${ThirdPartyLibDir})

add_executable(${PROJECT_NAME} main.cpp servicecoma.cpp servicecoma.h
                               svcmodule.cpp svcmodule.h)
target_link_libraries(
  ${PROJECT_NAME}
  interfaces
  gen
  module
  check
  config
  widgets
  dialogs
  models
  startup
  comaversion
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Xml
  Qt${QT_VERSION_MAJOR}::Widgets
  Qt${QT_VERSION_MAJOR}::Network
  QXlsx
  limereport)
