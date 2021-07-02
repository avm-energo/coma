cmake_minimum_required(VERSION 3.15)
project(root)
add_compile_definitions(DEVICETYPE=1)
set(SOFTDEVELOPER "AVM-Energo")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_program(CONAN_FOUND "conan")
if(CONAN_FOUND)
    set(CONAN_EXEC conan)
endif()



set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../scripts")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/..")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/3rdParty")


include(DetermineTargetArchitecture)
target_architecture(PROJECT_ARCHITECTURE)
message(STATUS "Target architecture - ${PROJECT_ARCHITECTURE}")
string(TOLOWER "${CMAKE_SYSTEM_NAME}" CMAKE_SYSTEM_NAME_LOWER)
if(${PROJECT_ARCHITECTURE}} MATCHES "x86_64")
    set(CONAN_TARGET_NAME "x86_64")
elseif(${PROJECT_ARCHITECTURE}} MATCHES "i386")
    set(CONAN_TARGET_NAME "x86")
else()
    message(FATAL_ERROR "Invalid arch name: ${PROJECT_ARCHITECTURE}")
endif()    

if(CMAKE_SYSTEM_NAME_LOWER STREQUAL "windows")
  message(STATUS "Windows")
  ADD_DEFINITIONS(-DUNICODE)
  ADD_DEFINITIONS(-D_UNICODE)
  if(${PROJECT_ARCHITECTURE}} MATCHES "x86_64")
    set(PROJECT_TARGET_NAME "win64")
  elseif(${PROJECT_ARCHITECTURE}} MATCHES "i386")
    set(PROJECT_TARGET_NAME "win32")
  else()
    message(FATAL_ERROR "Invalid arch name: ${PROJECT_ARCHITECTURE}")
  endif()
elseif(CMAKE_SYSTEM_NAME_LOWER STREQUAL "linux")
  message(STATUS "Linux")
  if(${PROJECT_ARCHITECTURE}} MATCHES "x86_64")
    set(PROJECT_TARGET_NAME "linux64")
  elseif(${PROJECT_ARCHITECTURE}} MATCHES "i386")
    set(PROJECT_TARGET_NAME "linux32")
  else()
    message(FATAL_ERROR "Invalid arch name: ${PROJECT_ARCHITECTURE}")
  endif()
elseif(CMAKE_SYSTEM_NAME_LOWER STREQUAL "unix")
  message(STATUS "Unix")
  if(${PROJECT_ARCHITECTURE}} MATCHES "x86_64")
    set(PROJECT_TARGET_NAME "unix64")
  elseif(${PROJECT_ARCHITECTURE}} MATCHES "i386")
    set(PROJECT_TARGET_NAME "unix32")
  else()
    message(FATAL_ERROR "Invalid arch name: ${PROJECT_ARCHITECTURE}")
  endif()
endif()
string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)

message(STATUS "Cached target arch: " ${CACHED_PROJECT_TARGET_NAME})
message(STATUS "Current target arch: " ${PROJECT_TARGET_NAME})

if(NOT DEFINED CACHED_PROJECT_TARGET_NAME)
    set(CACHED_PROJECT_TARGET_NAME "dummy")
endif()
if (NOT ( EXISTS "${CMAKE_SOURCE_DIR}/../conanbuildinfo.txt" AND ${CACHED_PROJECT_TARGET_NAME} STREQUAL ${PROJECT_TARGET_NAME}))
    execute_process(COMMAND ${CONAN_EXEC} install . -g cmake -g cmake_find_package -s arch="${CONAN_TARGET_NAME}" -s arch_build="${CONAN_TARGET_NAME}"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/..
        RESULT_VARIABLE CMD_ERROR)
    set(CACHED_PROJECT_TARGET_NAME ${PROJECT_TARGET_NAME} CACHE STRING "Cached target arch")
    message(STATUS "Init conan for " ${PROJECT_TARGET_NAME})
    message( STATUS "CMD_ERROR:" ${CMD_ERROR})
endif()


message(STATUS "Modules dir: " ${CMAKE_MODULE_PATH})

message(STATUS "${CMAKE_BUILD_TYPE_LOWER} mode")
message(STATUS "Target name - ${PROJECT_TARGET_NAME}")
find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core
  OPTIONAL_COMPONENTS Widgets
  )
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core
  OPTIONAL_COMPONENTS Widgets
  )
set(QT_VERSION ${Qt5Core_VERSION})
message(STATUS "QT version - ${QT_VERSION}")

if(CMAKE_BUILD_TYPE EQUAL "DEBUG")
  message(STATUS "debug mode")
  set(MODE_DIR Debug)
else()
  message(STATUS "not a debug mode")
  set(MODE_DIR Release)
endif(CMAKE_BUILD_TYPE EQUAL "DEBUG")

# required modules for our task
include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(CheckIncludeFiles)
include(ExternalProject)
include(limereport)
include(qxlsx)




add_subdirectory(interfaces)
add_subdirectory(gen)
add_subdirectory(comaversion)
if (Qt5Widgets_FOUND)
    add_subdirectory(config)
    add_subdirectory(avtuk)
    add_subdirectory(check)
    add_subdirectory(qcustomplot)
    add_subdirectory(datablocks)
    add_subdirectory(module)
    add_subdirectory(startup)
    add_subdirectory(widgets)
    add_subdirectory(dialogs)
    add_subdirectory(tune)
    add_subdirectory(tests)
    add_subdirectory(models)
    add_subdirectory(AVM-Service)
    add_subdirectory(AVM-Debug)
else()
    message(STATUS "Build without gui")
endif()
