cmake_minimum_required(VERSION 3.21)

project(coma VERSION 3.3.2)

set(SOFTDEVELOPER "AVM-Energo")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(BUILD_SHARED_LIBS ON)
set(BUILD_TESTING OFF)
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

# Including cmake scripts for some functions usage
include(WinDeployQt)

# Finding Qt libraries
find_package(QT NAMES Qt6 Qt5)
message(STATUS "Qt major version: ${QT_VERSION_MAJOR}")
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS
  Core Gui PrintSupport Widgets REQUIRED)

if(SYSTEM_NAME STREQUAL "windows")
    message(STATUS "MSVC version: ${MSVC_VERSION}, toolset version: ${MSVC_TOOLSET_VERSION}")
endif()

# Including ctest
if(BUILD_TESTING)
  include(CTest)
endif()

# Building libraries and targets
# Step 1: Build dependencies

include(gen)
include(expected)
include(qcustomplot)
include(limereport)
include(qxlsx)
add_subdirectory(src/appconfig)
add_subdirectory(src/autonomous)
add_subdirectory(src/comaversion)
add_subdirectory(src/comaresources)
add_subdirectory(src/ctti)
add_subdirectory(src/s2)
add_subdirectory(src/interfaces)
add_subdirectory(src/device)
add_subdirectory(src/xml)
add_subdirectory(src/settings)
add_subdirectory(src/models)
add_subdirectory(src/widgets)
add_subdirectory(src/alarms)
add_subdirectory(src/journals)
add_subdirectory(src/oscillograms)
add_subdirectory(src/startup)
add_subdirectory(src/datablocks)
add_subdirectory(src/tune)
add_subdirectory(src/dialogs)
add_subdirectory(src/coma_core)

# Step 2: Build tests
if(BUILD_TESTING)
  add_subdirectory(src/tests)
endif()

# Step 3: Build executable targets
add_subdirectory(src/AVM-Debug)
add_subdirectory(src/AVM-Service)
