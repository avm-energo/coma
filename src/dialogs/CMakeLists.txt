cmake_minimum_required(VERSION 3.14)

project(dialogs LANGUAGES CXX)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Concurrent REQUIRED)

# Collecting sources
set(DIALOGS_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include/dialogs")
set(DIALOGS_SRC_PATH     "${CMAKE_CURRENT_SOURCE_DIR}/src")
file(GLOB_RECURSE files_h   ${DIALOGS_INCLUDE_PATH}/*.h)
file(GLOB_RECURSE files_cpp ${DIALOGS_SRC_PATH}/*.cpp)

# Creating library target
add_library(${PROJECT_NAME} STATIC ${files_cpp} ${files_h})

# Including directories for library
target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

# Linking library dependencies
target_link_libraries(${PROJECT_NAME} PUBLIC
  alarms
  journals
  oscillograms
  startup
  tune
  Qt${QT_VERSION_MAJOR}::Concurrent
)

# Building library with emulator support if this option enabled
if(ENABLE_EMULATOR)
  target_compile_definitions(${PROJECT_NAME} PRIVATE ENABLE_EMULATOR)
endif()
