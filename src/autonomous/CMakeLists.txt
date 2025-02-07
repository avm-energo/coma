cmake_minimum_required(VERSION 3.15)

project(autonomous LANGUAGES CXX)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS
  Core
  Widgets
  REQUIRED)

# Collecting sources
set(INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include/autonomous")
set(SRC_PATH     "${CMAKE_CURRENT_SOURCE_DIR}/src")
file(GLOB_RECURSE files_h   ${INCLUDE_PATH}/*.h)
file(GLOB_RECURSE files_cpp ${SRC_PATH}/*.cpp)

# Creating library target
add_library(${PROJECT_NAME} STATIC ${files_cpp} ${files_h})
target_include_directories(${PROJECT_NAME} PRIVATE ${avm-gen_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${avm-widgets_SOURCE_DIR}/include)

# Including directories for library
target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

# Linking library dependencies
target_link_libraries(${PROJECT_NAME} PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    s2
    avm-widgets
    avm-gen
)
