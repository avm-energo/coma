cmake_minimum_required(VERSION 3.14)

project(coma LANGUAGES CXX)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

include(githash)

# Collecting sources
set(COMA_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(COMA_SRC_PATH     "${CMAKE_CURRENT_SOURCE_DIR}/src")

file(GLOB_RECURSE files_h   ${COMA_INCLUDE_PATH}/*.h)
file(GLOB_RECURSE files_cpp ${COMA_SRC_PATH}/*.cpp)

# Create library target
add_library(${PROJECT_NAME} STATIC ${files_cpp} ${files_h})

# Including directories for library
target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC
  Qt${QT_VERSION_MAJOR}::Core
  alarms
  avm-gen
  avm-widgets
  device
  journals
  models
  qcustomplot
  QXlsx
  settings
  startup
  tune
)


