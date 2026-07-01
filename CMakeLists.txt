cmake_minimum_required(VERSION 3.21)

project(coma VERSION 3.6.1)

set(SOFTDEVELOPER "AVM-Energo")

option(BUILD_TESTING "Enable testing" OFF)
option(BUILD_SHARED_LIBS "Generate shared libraries" ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(HIDAPI_INSTALL_TARGETS ON)
set(HIDAPI_WITH_TESTS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/scripts")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Getting CMake system name
string(TOLOWER "${CMAKE_SYSTEM_NAME}" SYSTEM_NAME)

if(SYSTEM_NAME STREQUAL "linux")
    message(STATUS "Compiling for linux")
elseif(SYSTEM_NAME STREQUAL "windows")
    message(STATUS "Compiling for windows")
else()
    message(STATUS "Compiling for unknown")
endif()

# Finding Qt libraries
find_package(QT NAMES Qt6)
message(STATUS "Qt major version: ${QT_VERSION_MAJOR}")
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS
  Core Gui PrintSupport Widgets REQUIRED)

if(SYSTEM_NAME STREQUAL "windows")
    message(STATUS "MSVC version: ${MSVC_VERSION}, toolset version: ${MSVC_TOOLSET_VERSION}")
endif()

# tell the MSVC to use multi-processor builds
if (MSVC)
    add_compile_options(/MP)
endif()

# Building libraries and targets
# Step 1: Build dependencies

find_package(libavm-widgets QUIET)
if(${libavm-widgets_FOUND})
  message(STATUS "Found package avm-widgets")
else()
  include(avm-widgets)
endif()

find_package(limereport QUIET)
if(${limereport_FOUND})
  message(STATUS "Found package limereport")
else()
  include(limereport)
endif()

find_package(libavm-qcustomplot QUIET)
if(${libavm-qcustomplot_FOUND})
  message(STATUS "Found package libavm-qcustomplot")
else()
  include(qcustomplot)
endif()

find_package(qxlsx QUIET)
if(${qxlsx_FOUND})
  message(STATUS "Found package qxlsx")
else()
  include(qxlsx)
endif()

add_subdirectory(src/coma)
add_subdirectory(src/ctti)
add_subdirectory(src/interfaces)
add_subdirectory(src/abatcher)

# Step 2: Build tests
if(BUILD_TESTING)
  message(STATUS "Will build tests")
  enable_testing()
  add_subdirectory(src/tests)
else()
  message(STATUS "Tests are disabled")
endif()

# Step 3: Build executable targets
add_subdirectory(src/avm-debug)
add_subdirectory(src/avm-service)
add_subdirectory(src/avm-autobatch/)

set(VERSION_IN ${CMAKE_CURRENT_SOURCE_DIR}/VERSION.in)
set(VERSION_OUT ${CMAKE_CURRENT_SOURCE_DIR}/package/src/VERSION)
configure_file(${VERSION_IN} ${VERSION_OUT} @ONLY)
