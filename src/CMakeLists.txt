cmake_minimum_required(VERSION 3.15)
project(coma)
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
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

##### OPTIONS #####
option(ENABLE_EMULATOR "Enable emulator interface" OFF)
option(ENABLE_PVS "Enable PVS-Studio" OFF)
option(BUILD_WITH_ALISE "Build with alise for AVTUK" OFF)

include(DetermineTargetArchitecture)
include(SetProjectTargetName)
include(ConanInstallDependencies)

find_package(QT NAMES Qt6 Qt5)
# Old Qt does not provide QT_VERSION_MAJOR
if(NOT QT_VERSION_MAJOR)
  string(SUBSTRING ${QT_VERSION} 0 1 QT_VERSION_MAJOR)
endif()

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS
  Core Gui PrintSupport Sql Qml Widgets REQUIRED)



if(NOT DEFINED PROJECT_ARCHITECTURE)
  target_architecture(PROJECT_ARCHITECTURE)
  set_project_target_name(${PROJECT_ARCHITECTURE} PROJECT_TARGET_NAME USER_DIRECTORY)
  message(STATUS "Current target arch: " ${PROJECT_TARGET_NAME})
  message(STATUS "Current user directory for conan: " ${USER_DIRECTORY})
  conan_install_dependencies(${PROJECT_ARCHITECTURE} ${PROJECT_TARGET_NAME} ${USER_DIRECTORY})

  if(${PROJECT_ARCHITECTURE}} MATCHES "x86_64")
    set(AVTUK-14 TRUE)
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64)
  elseif(${PROJECT_ARCHITECTURE}} MATCHES "armv7")
    set(AVTUK-12 TRUE)
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE armhf)
  endif()
endif()

if(MSVC)
    add_compile_options(/permissive-)
    string(APPEND CMAKE_CXX_FLAGS " /w14456 /w14189 /w14458 /w14100 /w14996")
else()
    #TODO make the same as msvc
endif()

if(NOT MSVC_C_ARCHITECTURE_ID)
    if(NOT AVTUK-12)
        string(APPEND CMAKE_CXX_FLAGS_DEBUG " -fno-omit-frame-pointer -fsanitize=address,undefined -fstack-protector")
        string(APPEND CMAKE_LINKER_FLAGS_DEBUG " -fno-omit-frame-pointer -fsanitize=address,undefined -fstack-protector")
    else()
        string(APPEND CMAKE_CXX_FLAGS_DEBUG " -fno-omit-frame-pointer -fstack-protector")
        string(APPEND CMAKE_LINKER_FLAGS_DEBUG " -fno-omit-frame-pointer -fstack-protector")
    endif()
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(QT_VERSION ${Qt5Core_VERSION})
message(STATUS "QT version - ${QT_VERSION}")

if(CMAKE_BUILD_TYPE EQUAL "DEBUG")
  message(STATUS "debug mode")
  set(MODE_DIR Debug)
else()
  message(STATUS "not a debug mode")
  set(MODE_DIR Release)
endif()

if(ENABLE_PVS)
  if(NOT EXISTS "${CMAKE_BINARY_DIR}/PVS-Studio.cmake")
    message(STATUS "Downloading PVS-Studio.cmake from https://github.com/viva64/pvs-studio-cmake-examples")
    file(DOWNLOAD "https://raw.githubusercontent.com/viva64/pvs-studio-cmake-examples/master/PVS-Studio.cmake"
                  "${CMAKE_BINARY_DIR}/PVS-Studio.cmake"
                  EXPECTED_HASH SHA256=C53013A568A71CC4DBB10437256F16C027004B08ECA3DAF9911BAC08896B4DE7
                  TLS_VERIFY ON)
  endif()
  include(${CMAKE_BINARY_DIR}/PVS-Studio.cmake)
endif()

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake"
                EXPECTED_HASH SHA256=5cdb3042632da3efff558924eecefd580a0e786863a857ca097c3d1d43df5dcd
                TLS_VERIFY ON)
endif()
include(${CMAKE_BINARY_DIR}/conan.cmake)
conan_cmake_autodetect(settings)

if(CMAKE_GENERATOR MATCHES "Visual Studio")
  set(CUSTOM_TARGET_PLATFORM_ARG -A${WINDOWS_TARGET_NAME})
else()
  set(CUSTOM_TARGET_GENERATOR "-G${CMAKE_GENERATOR}")
endif()

include(limereport)
include(qxlsx)
include(qcustomplot)

include(gen)
add_subdirectory(comaversion)
if(NOT AVTUK-12)
    add_subdirectory(interfaces)
endif()
if (BUILD_WITH_ALISE)
    set(PROTOS_GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/protos/protos)
    add_subdirectory(alise)
    add_subdirectory(protos)
endif()
if(Qt${QT_VERSION_MAJOR}Widgets_FOUND AND NOT AVTUK-12)
    add_subdirectory(s2)
    add_subdirectory(oscillograms)
    add_subdirectory(datablocks)
    add_subdirectory(module)
    add_subdirectory(startup)
    add_subdirectory(widgets)
    add_subdirectory(dialogs)
    add_subdirectory(tune)
    add_subdirectory(tests)
    add_subdirectory(models)
    add_subdirectory(xml)
    add_subdirectory(AVM-Service)
    add_subdirectory(AVM-Debug)
else()
    message(STATUS "Build without GUI")
endif()

if(ENABLE_PVS)
    pvs_studio_add_target(
        TARGET ${PROJECT_NAME}.analyze ALL
        OUTPUT FORMAT errorfile
        COMPILE_COMMANDS
        MODE GA:1,2
        LOG ${PROJECT_NAME}.err)
endif()
