cmake_minimum_required(VERSION 3.14)

project(widgets LANGUAGES CXX)

find_package(Boost COMPONENTS headers REQUIRED)

# Collecting sources
set(WIDGETS_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include/widgets")
set(WIDGETS_SRC_PATH     "${CMAKE_CURRENT_SOURCE_DIR}/src")
file(GLOB_RECURSE files_h   ${WIDGETS_INCLUDE_PATH}/*.h)
file(GLOB_RECURSE files_cpp ${WIDGETS_SRC_PATH}/*.cpp)

# Creating library target
add_library(${PROJECT_NAME} STATIC ${files_cpp} ${files_h})

# Including directories for library
target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

# Linking library dependencies
target_link_libraries(${PROJECT_NAME} PUBLIC
  Boost::headers device settings models
)
