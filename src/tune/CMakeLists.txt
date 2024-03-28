cmake_minimum_required(VERSION 3.14)

project(tune LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB_RECURSE files_cpp ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE files_h ${PROJECT_SOURCE_DIR}/*.h)

add_library(${PROJECT_NAME} STATIC ${files_cpp} ${files_h})

target_link_libraries(${PROJECT_NAME} PUBLIC datablocks limereport)

target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)
