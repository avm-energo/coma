cmake_minimum_required(VERSION 3.21)

project(coma VERSION 3.2.2)

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

##### OPTIONS #####
option(ENABLE_EMULATOR "Enable emulator interface" OFF)
option(ENABLE_PVS "Enable PVS-Studio" OFF)

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

# Using Qt6: finding Qt6 QmlIntegration library
# Q: Why?
# A: LimeReport use Qml and if we want use LimeReport,
# we must find QmlIntegration for using its private headers.
#if(${QT_VERSION_MAJOR} EQUAL 6)
#  find_package(Qt${QT_VERSION_MAJOR} COMPONENTS QmlIntegration REQUIRED)
#endif()

if(SYSTEM_NAME STREQUAL "windows")
    message(STATUS "MSVC version: ${MSVC_VERSION}, toolset version: ${MSVC_TOOLSET_VERSION}")
# else()
#     string(APPEND CMAKE_CXX_FLAGS_DEBUG " -fsanitize=address,undefined -fstack-protector")
#     string(APPEND CMAKE_LINKER_FLAGS_DEBUG " -fsanitize=address,undefined -fstack-protector")
endif()

# Enabling PVS Studio static analyzer
if(ENABLE_PVS)
  include(FetchContent)
  FetchContent_Declare(
    PVS_CMakeModule
    GIT_REPOSITORY "https://github.com/viva64/pvs-studio-cmake-module.git"
    GIT_TAG        "master")
  FetchContent_MakeAvailable(PVS_CMakeModule)
  include("${pvs_cmakemodule_SOURCE_DIR}/PVS-Studio.cmake")
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

# Running PVS Studio static analyzer
if(ENABLE_PVS)
  pvs_studio_add_target(
    TARGET ${PROJECT_NAME}.analyze ALL
    OUTPUT FORMAT errorfile
    COMPILE_COMMANDS
    MODE GA:1,2
    LOG ${PROJECT_NAME}.err)
endif()
