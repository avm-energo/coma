cmake_minimum_required(VERSION 3.14)

project(abatcher LANGUAGES CXX)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS REQUIRED)

# Collecting sources
set(ABATCHER_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(ABATCHER_SRC_PATH     "${CMAKE_CURRENT_SOURCE_DIR}/src")

file(GLOB_RECURSE files_h   ${ABATCHER_INCLUDE_PATH}/*.h)
file(GLOB_RECURSE files_cpp ${ABATCHER_SRC_PATH}/*.cpp)

# Create library target
add_library(${PROJECT_NAME} STATIC ${files_cpp} ${files_h})

# Including directories for library
target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${limereport_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC
  libavm-widgets
  interfaces
)


