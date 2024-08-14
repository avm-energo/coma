cmake_minimum_required(VERSION 3.18)
project(coma VERSION 2.12.0)

set(SOFTDEVELOPER "AVM-Energo")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../scripts")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

##### OPTIONS #####
option(ENABLE_EMULATOR "Enable emulator interface" OFF)
option(ENABLE_PVS "Enable PVS-Studio" OFF)
option(COMA_BUILD_TESTS "Build tests" ON)

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

string(APPEND CMAKE_CXX_FLAGS_DEBUG " -fsanitize=address,undefined -fstack-protector")
string(APPEND CMAKE_LINKER_FLAGS_DEBUG " -fsanitize=address,undefined -fstack-protector")

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

# Collecting Boost headers and hidapi library on windows
if(SYSTEM_NAME STREQUAL "windows")
#  conan_cmake_configure(
#    REQUIRES hidapi/0.10.1 boost/1.75.0 xz_utils/5.2.5
#    GENERATORS cmake_find_package
#    OPTIONS hidapi:shared=True boost:header_only=True xz_utils:shared=False)
else()
    find_library(HIDAPI_LIBRARY NAMES libhidapi-libusb.so)
    if(NOT HIDAPI_LIBRARY)
      message(FATAL_ERROR "Hidapi library not found. Install libhidapi-dev please")
    endif()
    find_library(LIBLZMA_LIBRARY NAMES liblzma.so)
    if(NOT LIBLZMA_LIBRARY)
      message(FATAL_ERROR "Lzma library not found. Install liblzma-dev please")
    endif()
endif()

# Including ctest
if(COMA_BUILD_TESTS)
  include(CTest)
endif()

# Building libraries and targets
# Step 1: Build dependencies
include(gen)
include(expected)
include(qcustomplot)
include(limereport)
include(qxlsx)
add_subdirectory(comaversion)
add_subdirectory(comaresources)
add_subdirectory(ctti)
add_subdirectory(s2)
add_subdirectory(interfaces)
add_subdirectory(device)
add_subdirectory(xml)
add_subdirectory(settings)
add_subdirectory(models)
add_subdirectory(widgets)
add_subdirectory(alarms)
add_subdirectory(journals)
add_subdirectory(oscillograms)
add_subdirectory(startup)
add_subdirectory(datablocks)
add_subdirectory(tune)
add_subdirectory(dialogs)
add_subdirectory(coma_core)

# Step 2: Build tests
if(COMA_BUILD_TESTS)
  add_subdirectory(tests)
endif()

# Step 3: Build executable targets
add_subdirectory(AVM-Debug)
add_subdirectory(AVM-Service)

# Running PVS Studio static analyzer
if(ENABLE_PVS)
  pvs_studio_add_target(
    TARGET ${PROJECT_NAME}.analyze ALL
    OUTPUT FORMAT errorfile
    COMPILE_COMMANDS
    MODE GA:1,2
    LOG ${PROJECT_NAME}.err)
endif()
