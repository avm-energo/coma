project(AVM-Debug)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpic")
# find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
# find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
# find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
# find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED) find_package(QT
# NAMES Qt6 Qt5 COMPONENTS Xml REQUIRED)
find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Xml Widgets
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Xml Widgets
  REQUIRED)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
set(PROGNAME "AVM-Service")
set(SOFTDEVELOPER "AVM-Energo")
set(COMAVERSION 2.0.0)
configure_file(cconfig.h.in config.h @ONLY)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../lib)
link_directories(${LimeReportDir})
# find_package( QT NAMES Qt6 Qt5 COMPONENTS Core Xml Widgets REQUIRED)
# find_package( Qt${QT_VERSION_MAJOR} COMPONENTS Core Xml widgets REQUIRED)
# find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
message(${QT_VERSION_MAJOR},"")
add_executable(${PROJECT_NAME} dbgmodule.cpp dbgmodule.h debugcoma.cpp
                               debugcoma.h main.cpp)
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
  tune
  qcustomplot
  avtuk
  datablocks
  comaversion
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Xml
  Qt${QT_VERSION_MAJOR}::Widgets
  limereport)
