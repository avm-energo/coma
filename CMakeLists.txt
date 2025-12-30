cmake_minimum_required(VERSION 3.21)

project(pkdn-service VERSION 3.4.3)

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
  Core Gui PrintSupport Widgets SerialPort REQUIRED)

# Define a variable for the number
set(PROGRAM_VARIANT_OPTION "FULL" CACHE STRING "Program size (functions content): FULL/LARGE/MEDIUM/SMALL")

if(PROGRAM_VARIANT_OPTION STREQUAL "FULL")
  add_compile_definitions(PROGSIZE=4)
elseif(PROGRAM_VARIANT_OPTION STREQUAL "LARGE")
  add_compile_definitions(PROGSIZE=3)
elseif(PROGRAM_VARIANT_OPTION STREQUAL "MEDIUM")
  add_compile_definitions(PROGSIZE=2)
else()
  add_compile_definitions(PROGSIZE=1)
endif()

if(SYSTEM_NAME STREQUAL "windows")
    message(STATUS "MSVC version: ${MSVC_VERSION}, toolset version: ${MSVC_TOOLSET_VERSION}")
endif()

# Including ctest
if(BUILD_TESTING)
  include(CTest)
endif()

# Building libraries and targets
# Step 1: Build dependencies

include(hidapi)
include(limereport)
include(qxlsx)
include(githash)

configure_file(
  ${CMAKE_SOURCE_DIR}/cconfig.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/config.h
@ONLY)

configure_file(
  ${CMAKE_SOURCE_DIR}/versioninfo.rc.in
  ${CMAKE_CURRENT_BINARY_DIR}/versioninfo.rc
@ONLY)

file(GLOB_RECURSE files_cpp ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE files_h ${PROJECT_SOURCE_DIR}/*.h)

add_executable(${PROJECT_NAME}
  ${files_cpp} ${files_h} coma.rc
  ${CMAKE_CURRENT_BINARY_DIR}/versioninfo.rc)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${limereport_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${hidapi_SOURCE_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE true)
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Gui
  Qt${QT_VERSION_MAJOR}::PrintSupport
  Qt${QT_VERSION_MAJOR}::Widgets
  Qt${QT_VERSION_MAJOR}::SerialPort
  limereport
  QXlsx
  )

if(SYSTEM_NAME STREQUAL "windows")
    target_link_libraries(${PROJECT_NAME} PUBLIC
      hidapi
    )
elseif(SYSTEM_NAME STREQUAL "linux")
    target_link_libraries(${PROJECT_NAME} PUBLIC
      hidapi-hidraw
    )
endif()

if(ENABLE_EMULATOR)
  target_compile_definitions(${PROJECT_NAME} PRIVATE ENABLE_EMULATOR)
endif()

# Getting appdata directory path
if(SYSTEM_NAME STREQUAL "windows")
    # On windows
    set(APPDATA_STR "$ENV{APPDATA}/AVM-Energo/${PROJECT_NAME}")
    string(REPLACE "\\" "/" APPDATA ${APPDATA_STR})
    message(STATUS "Set appdata to ${APPDATA}")
else()
    # On linux
    set(APPDATA $ENV{HOME}/.config/AVM-Energo/${PROJECT_NAME})
    message(STATUS "Set appdata to ${APPDATA}")
endif()

# Copy settings one time at first build
set(DBG_CHECK_PATH "${CMAKE_BINARY_DIR}/pkdn-check.txt")
if(NOT EXISTS ${DBG_CHECK_PATH})
  file(TOUCH ${DBG_CHECK_PATH})
  # Copying reports lrxml files to appdata directory after building
  add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD COMMAND
    ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/reports"
    "${APPDATA}/reports"
    COMMENT "Copying reports...")
  add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD COMMAND
    ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/images"
    "${APPDATA}/images"
    COMMENT "Copying images...")
  add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD COMMAND
    ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/errors/ermsgs.dat"
    "${APPDATA}"
    COMMENT "Copying error messages...")
endif()

install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Cross-platform deploy target
if(SYSTEM_NAME STREQUAL "windows")
  include(windeploy)
elseif(SYSTEM_NAME STREQUAL "linux")
  set(COMPONENT_NAME "PKDNSERVICE")
#  include(LinuxDeploy)
endif()

set(VERSION_IN ${CMAKE_CURRENT_SOURCE_DIR}/VERSION.in)
set(VERSION_OUT ${CMAKE_CURRENT_SOURCE_DIR}/package/src/VERSION)
configure_file(${VERSION_IN} ${VERSION_OUT} @ONLY)
