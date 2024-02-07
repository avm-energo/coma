cmake_minimum_required(VERSION 3.15)
project(coma VERSION 2.10.0)

add_compile_definitions(DEVICETYPE=1)
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

# Including cmake scripts for some functions usage
include(DetermineTargetArchitecture)
include(SetProjectTargetName)
include(WinDeployQt)

# Finding Qt libraries
find_package(QT NAMES Qt6 Qt5)
# Old Qt does not provide QT_VERSION_MAJOR
if(NOT QT_VERSION_MAJOR)
  string(SUBSTRING ${QT_VERSION} 0 1 QT_VERSION_MAJOR)
endif()
message(STATUS "Qt major version: ${QT_VERSION_MAJOR}")
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS
  Core Gui PrintSupport Widgets REQUIRED)

# Using Qt6: finding Qt6 QmlIntegration library
# Q: Why?
# A: LimeReport use Qml and if we want use LimeReport,
# we must find QmlIntegration for using its private headers.
if(${QT_VERSION_MAJOR} EQUAL 6)
  find_package(Qt${QT_VERSION_MAJOR} COMPONENTS QmlIntegration REQUIRED)
endif()

# Determine target's architecture and name. Then installing dependencies with conan
if(NOT DEFINED PROJECT_ARCHITECTURE)
  target_architecture(PROJECT_ARCHITECTURE)
  set_project_target_name(${PROJECT_ARCHITECTURE} PROJECT_TARGET_NAME USER_DIRECTORY)
  message(STATUS "Current target arch: " ${PROJECT_TARGET_NAME})
  message(STATUS "Current user directory for conan: " ${USER_DIRECTORY})
endif()

# Getting CMake system name
string(TOLOWER "${CMAKE_SYSTEM_NAME}" CMAKE_SYSTEM_NAME_LOWER)
# Export compile commands. See:
# - https://cmake.org/cmake/help/latest/variable/CMAKE_EXPORT_COMPILE_COMMANDS.html
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Visual Studio compilation flags
if(MSVC)
  add_compile_options(/permissive-)
  string(APPEND CMAKE_CXX_FLAGS " /w14456 /w14189 /w14458 /w14100 /w14996")
# GCC / Clang compilation flags
else()
  # Compiling flags for x86_64 architecture
  if(${PROJECT_ARCHITECTURE}} MATCHES "x86_64")
    string(APPEND CMAKE_CXX_FLAGS_DEBUG " -fno-omit-frame-pointer -fsanitize=address,undefined -fstack-protector")
    string(APPEND CMAKE_LINKER_FLAGS_DEBUG " -fno-omit-frame-pointer -fsanitize=address,undefined -fstack-protector")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64)
  # Compiling flags for armhf architecture
  elseif(${PROJECT_ARCHITECTURE}} MATCHES "armv7")
    string(APPEND CMAKE_CXX_FLAGS_DEBUG " -fno-omit-frame-pointer -fstack-protector")
    string(APPEND CMAKE_LINKER_FLAGS_DEBUG " -fno-omit-frame-pointer -fstack-protector")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE armhf)
  endif()
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

# Downloading conan CMake script and using for autodetecting
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(
    DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake" "${CMAKE_BINARY_DIR}/conan.cmake"
    EXPECTED_HASH SHA256=5cdb3042632da3efff558924eecefd580a0e786863a857ca097c3d1d43df5dcd
    TLS_VERIFY ON
  )
endif()
include(${CMAKE_BINARY_DIR}/conan.cmake)

# Getting build type, compiler, etc... (other settings)
conan_cmake_autodetect(settings)
# Collecting Boost headers and hidapi library on windows
if(CMAKE_SYSTEM_NAME_LOWER STREQUAL "windows")
  conan_cmake_configure(
    REQUIRES hidapi/0.10.1 boost/1.75.0 xz_utils/5.2.5
    GENERATORS cmake_find_package
    OPTIONS hidapi:shared=True boost:header_only=True xz_utils:shared=False)
else()
  conan_cmake_configure(
    REQUIRES boost/1.75.0
    GENERATORS cmake_find_package
    OPTIONS boost:header_only=True)
endif()
# Installing libraries
conan_cmake_install(
  PATH_OR_REFERENCE .
  BUILD missing
  REMOTE conancenter
  SETTINGS ${settings})

# Setup packaging install prefix for DEB generator
if(NOT DEFINED CPACK_PACKAGING_INSTALL_PREFIX)
  set(CPACK_PACKAGING_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
endif()
message(STATUS "Packaging install prefix: ${CPACK_PACKAGING_INSTALL_PREFIX}")

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
add_subdirectory(s2)
add_subdirectory(interfaces)
add_subdirectory(device)
add_subdirectory(settings)
add_subdirectory(datablocks)
add_subdirectory(module)
add_subdirectory(oscillograms)
add_subdirectory(startup)
add_subdirectory(widgets)
add_subdirectory(dialogs)
add_subdirectory(tune)
add_subdirectory(models)
add_subdirectory(xml)
add_subdirectory(journals)
add_subdirectory(coma_core)
# Step 2: Build tests
if(COMA_BUILD_TESTS)
  add_subdirectory(tests)
endif()
# Step 3: Build executable targets
add_subdirectory(AVM-Debug)
add_subdirectory(AVM-Service)

# Linux deploy with cpack
if(CMAKE_SYSTEM_NAME_LOWER STREQUAL "linux")
  include(LinuxDeploy)
endif()

# Running PVS Studio static analyzer
if(ENABLE_PVS)
  pvs_studio_add_target(
    TARGET ${PROJECT_NAME}.analyze ALL
    OUTPUT FORMAT errorfile
    COMPILE_COMMANDS
    MODE GA:1,2
    LOG ${PROJECT_NAME}.err)
endif()
