cmake_minimum_required(VERSION 3.15)

project(s2 LANGUAGES CXX)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS
  Core
  REQUIRED)

# Collecting sources
set(INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include/s2")
set(SRC_PATH     "${CMAKE_CURRENT_SOURCE_DIR}/src")

set(headers
  "${INCLUDE_PATH}/dataitem.h"
  "${INCLUDE_PATH}/delegate_common.h"
  "${INCLUDE_PATH}/filestruct.h"
  "${INCLUDE_PATH}/hexparser.h"
  "${INCLUDE_PATH}/modbusitem.h"
  "${INCLUDE_PATH}/s2configstorage.h"
  "${INCLUDE_PATH}/s2configuration.h"
  "${INCLUDE_PATH}/s2datafactory.h"
  "${INCLUDE_PATH}/s2datamanager.h"
  "${INCLUDE_PATH}/s2datatypes.h"
  "${INCLUDE_PATH}/s2util.h"
)

set(sources
  "${SRC_PATH}/dataitem.cpp"
  "${SRC_PATH}/filestruct.cpp"
  "${SRC_PATH}/hexparser.cpp"
  "${SRC_PATH}/s2configstorage.cpp"
  "${SRC_PATH}/s2configuration.cpp"
  "${SRC_PATH}/s2datafactory.cpp"
  "${SRC_PATH}/s2datamanager.cpp"
  "${SRC_PATH}/s2util.cpp"
)

# find_package(avm-gen)
# Creating library target
add_library(${PROJECT_NAME} STATIC ${sources} ${headers})
# add_dependencies(${PROJECT_NAME} gen)
target_include_directories(${PROJECT_NAME} PRIVATE ${avm-gen_SOURCE_DIR}/include)

# Including directories for library
target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

# Linking library dependencies
target_link_libraries(${PROJECT_NAME} PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    ctti::ctti
    avm-gen
)
