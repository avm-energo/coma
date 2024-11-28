cmake_minimum_required(VERSION 3.14)

project(appconfig LANGUAGES CXX)

find_package(Qt${QT_VERSION_MAJOR}
  COMPONENTS Core REQUIRED)

# Collecting sources
set(APPCONFIG_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include/appconfig")
set(APPCONFIG_SRC_PATH     "${CMAKE_CURRENT_SOURCE_DIR}/src")
file(GLOB_RECURSE files_h   ${APPCONFIG_INCLUDE_PATH}/*.h)
file(GLOB_RECURSE files_cpp ${APPCONFIG_SRC_PATH}/*.cpp)

# Creating library target
add_library(${PROJECT_NAME} STATIC ${files_cpp} ${files_h})

# Including directories for library
target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

# Linking library dependencies
target_link_libraries(${PROJECT_NAME} PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
)
