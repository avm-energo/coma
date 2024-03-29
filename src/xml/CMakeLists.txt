cmake_minimum_required(VERSION 3.15)

project(xml LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Xml REQUIRED)

# Collecting sources
set(XML_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}")
set(XML_SRC     "${CMAKE_CURRENT_SOURCE_DIR}/src/")
file(GLOB_RECURSE files_h   ${XML_INCLUDE}/*.h)
file(GLOB_RECURSE files_cpp ${XML_SRC}/*.cpp)

# Create library target
add_library(${PROJECT_NAME} STATIC ${files_cpp} ${files_h})

# Including directories for library
target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_link_libraries(
  ${PROJECT_NAME} PUBLIC
  widgets Qt${QT_VERSION_MAJOR}::Xml)
