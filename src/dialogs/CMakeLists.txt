cmake_minimum_required(VERSION 3.14)

project(dialogs LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt${QT_VERSION_MAJOR}
  COMPONENTS Concurrent
  REQUIRED)

file(GLOB_RECURSE files_cpp ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE files_h ${PROJECT_SOURCE_DIR}/*.h)

add_library(
  ${PROJECT_NAME} STATIC
  ${files_cpp} ${files_h})

target_link_libraries(${PROJECT_NAME} PUBLIC
  alarms
  oscillograms
  tune
  startup
  journals
  Qt${QT_VERSION_MAJOR}::Concurrent
)

target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)

if (ENABLE_EMULATOR)
    target_compile_definitions(${PROJECT_NAME} PRIVATE ENABLE_EMULATOR)
endif()
