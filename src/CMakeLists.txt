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

if(NOT CONAN_EXEC)
    message(FATAL_ERROR  "Conan package manager required, please download it from https://conan.io/downloads.html")
endif(NOT CONAN_EXEC)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../scripts")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/3rdParty")


include(DetermineTargetArchitecture)
target_architecture(PROJECT_ARCHITECTURE)
message(STATUS "MSVC_C_ARCHITECTURE_ID: " ${MSVC_C_ARCHITECTURE_ID})

message(STATUS "Target architecture - ${PROJECT_ARCHITECTURE}")
string(TOLOWER "${CMAKE_SYSTEM_NAME}" CMAKE_SYSTEM_NAME_LOWER)
    set(CONAN_PROFILE "default")
if(${PROJECT_ARCHITECTURE}} MATCHES "x86_64")
    set(CONAN_TARGET_NAME "x86_64")
elseif(${PROJECT_ARCHITECTURE}} MATCHES "i386")
    set(CONAN_TARGET_NAME "x86")
elseif(${PROJECT_ARCHITECTURE}} MATCHES "armv7")
    set(CONAN_TARGET_NAME "armv7hf")
    set(CONAN_PROFILE "armhf")
else()
    message(FATAL_ERROR "Invalid arch name: ${PROJECT_ARCHITECTURE}")
endif()

if(CMAKE_SYSTEM_NAME_LOWER STREQUAL "windows")
  message(STATUS "Windows")
  ADD_DEFINITIONS(-DUNICODE)
  ADD_DEFINITIONS(-D_UNICODE)
  if (NOT USER_DIRECTORY)
    set(USER_DIRECTORY $ENV{USERPROFILE})
  endif()
  if(${PROJECT_ARCHITECTURE}} MATCHES "x86_64")
    set(PROJECT_TARGET_NAME "win64")
  elseif(${PROJECT_ARCHITECTURE}} MATCHES "i386")
    set(PROJECT_TARGET_NAME "win32")
  else()
    message(FATAL_ERROR "Invalid arch name: ${PROJECT_ARCHITECTURE}")
  endif()
elseif(CMAKE_SYSTEM_NAME_LOWER STREQUAL "linux")
  message(STATUS "Linux")

  if (NOT USER_DIRECTORY)
    set(USER_DIRECTORY $ENV{HOME})
  endif()
  if(${PROJECT_ARCHITECTURE}} MATCHES "x86_64")
    set(PROJECT_TARGET_NAME "linux64")
    set(AVTUK-14 TRUE)
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64)
  elseif(${PROJECT_ARCHITECTURE}} MATCHES "i386")
    set(PROJECT_TARGET_NAME "linux32")
  elseif(${PROJECT_ARCHITECTURE}} MATCHES "armv7")
    set(PROJECT_TARGET_NAME "linux-armhf")
    set(AVTUK-12 TRUE)
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE armhf)
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

if (MSVC)
    add_compile_options(/permissive-)
    string(APPEND CMAKE_CXX_FLAGS " /w14456 /w14189 /w14458 /w14100 /w14996")
else()
    #TODO make the same as msvc
endif()

if (NOT MSVC_C_ARCHITECTURE_ID)
    if (NOT AVTUK-12)
        string(APPEND CMAKE_CXX_FLAGS_DEBUG " -fno-omit-frame-pointer -fsanitize=address,undefined -fstack-protector")
        string(APPEND CMAKE_LINKER_FLAGS_DEBUG " -fno-omit-frame-pointer -fsanitize=address,undefined -fstack-protector")
    else()
        string(APPEND CMAKE_CXX_FLAGS_DEBUG " -fno-omit-frame-pointer -fstack-protector")
        string(APPEND CMAKE_LINKER_FLAGS_DEBUG " -fno-omit-frame-pointer -fstack-protector")
    endif(NOT AVTUK-12)
endif(NOT MSVC_C_ARCHITECTURE_ID)

set(CMAKE_EXPORT_COMPILE_COMMANDS             ON)

message(STATUS "Cached target arch: " ${CACHED_PROJECT_TARGET_NAME})
message(STATUS "Current target arch: " ${PROJECT_TARGET_NAME})
message(STATUS "Sysroot: " ${CMAKE_SYSROOT})
if(NOT DEFINED CACHED_PROJECT_TARGET_NAME)
    set(CACHED_PROJECT_TARGET_NAME "dummy")
endif()

message(STATUS "Current user directory for conan: " ${USER_DIRECTORY})

set(CONAN_MAIN_INIT ${CONAN_EXEC} install . --profile ${CONAN_PROFILE} -g cmake_find_package -s arch=${CONAN_TARGET_NAME} -s arch_build=${CONAN_TARGET_NAME} -if ${CMAKE_BINARY_DIR} --build=missing)

if (NOT AVTUK-12)
    if (NOT ( EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.txt" AND ${CACHED_PROJECT_TARGET_NAME} STREQUAL ${PROJECT_TARGET_NAME}))
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E env
                CONAN_USER_HOME=${USER_DIRECTORY} ${CONAN_MAIN_INIT}
            RESULT_VARIABLE CMD_ERROR
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/..
        )
        set(CACHED_PROJECT_TARGET_NAME ${PROJECT_TARGET_NAME} CACHE STRING "Cached target arch" FORCE)
        message(STATUS "Init conan for " ${PROJECT_TARGET_NAME})
        message( STATUS "CMD_ERROR:" ${CMD_ERROR})
    endif()
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

##### OPTIONS #####
option(ENABLE_EMULATOR "Enable emulator interface" OFF)
option(ENABLE_PVS "Enable PVS-Studio" OFF)
option(BUILD_WITH_ALISE "Build with alise for AVTUK" OFF)


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


if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.16.1/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake"
                EXPECTED_HASH SHA256=396e16d0f5eabdc6a14afddbcfff62a54a7ee75c6da23f32f7a31bc85db23484
                TLS_VERIFY ON)
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)



if (ENABLE_PVS)

    if(NOT EXISTS "${CMAKE_BINARY_DIR}/PVS-Studio.cmake")
      message(STATUS "Downloading PVS-Studio.cmake from https://github.com/viva64/pvs-studio-cmake-examples")
      file(DOWNLOAD "https://raw.githubusercontent.com/viva64/pvs-studio-cmake-examples/master/PVS-Studio.cmake"
                    "${CMAKE_BINARY_DIR}/conan.cmake"
                    EXPECTED_HASH SHA256=C53013A568A71CC4DBB10437256F16C027004B08ECA3DAF9911BAC08896B4DE7
                    TLS_VERIFY ON)
    endif()

    include(${CMAKE_BINARY_DIR}/PVS-Studio.cmake)

endif()

conan_cmake_autodetect(settings)

if(NOT CONAN_DEBUG)
    list(TRANSFORM settings REPLACE "^.*(Debug)$"  "build_type=Release")
    list(TRANSFORM settings REPLACE "^.*(MDd)$"  "compiler.runtime=MD")
    list(TRANSFORM settings REPLACE "^.*(MTd)$"  "compiler.runtime=MT")
endif(NOT CONAN_DEBUG)


# Old Qt does not provide QT_VERSION_MAJOR
if (NOT QT_VERSION_MAJOR)
    string(SUBSTRING ${QT_VERSION} 0 1 QT_VERSION_MAJOR)
endif()

message(STATUS "Qt version: ${QT_VERSION_MAJOR}")
if (NOT AVTUK-12)
    if (Qt${QT_VERSION_MAJOR}Widgets_FOUND)
        message(STATUS "QtGui found")
    endif()
    if (Qt${QT_VERSION_MAJOR}Widgets_FOUND)
        message(STATUS "QtWidgets found")
    endif()

    if (Qt${QT_VERSION_MAJOR}Gui_FOUND)

        if(${PROJECT_ARCHITECTURE}} MATCHES "x86_64")
          set(WINDOWS_TARGET_NAME "x64")
        elseif(${PROJECT_ARCHITECTURE}} MATCHES "i386")
          set(WINDOWS_TARGET_NAME "Win32")
        else()
          message(FATAL_ERROR "Invalid arch name: ${PROJECT_ARCHITECTURE}")
        endif()

        if(CMAKE_GENERATOR MATCHES "Visual Studio")
          set(CUSTOM_TARGET_PLATFORM_ARG -A${WINDOWS_TARGET_NAME})
        else()
            set(CUSTOM_TARGET_GENERATOR "-G${CMAKE_GENERATOR}")
          # Other stuff
        endif()

        include(limereport)
        include(qxlsx)
        include(qcp)

    endif()
endif()

if (NOT AVTUK-12)
    add_subdirectory(interfaces)
endif()
add_subdirectory(gen)
add_subdirectory(comaversion)
if (BUILD_WITH_ALISE)
    set(PROTOS_GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/protos/protos)
    add_subdirectory(alise)
    add_subdirectory(protos)
endif()
if (Qt${QT_VERSION_MAJOR}Widgets_FOUND AND NOT AVTUK-12)
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

if (ENABLE_PVS)

    pvs_studio_add_target(
        TARGET ${PROJECT_NAME}.analyze ALL
        OUTPUT FORMAT errorfile
        COMPILE_COMMANDS
        MODE GA:1,2
        LOG ${PROJECT_NAME}.err)

endif()
