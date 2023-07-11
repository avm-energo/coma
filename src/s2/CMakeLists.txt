cmake_minimum_required(VERSION 3.15)

project(s2 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Network Xml Widgets
  REQUIRED)

#file(GLOB_RECURSE files_cpp ${PROJECT_SOURCE_DIR}/*.cpp)
#file(GLOB_RECURSE files_h ${PROJECT_SOURCE_DIR}/*.h)

set(headers
    configv.h
    dataitem.h
    filestruct.h
    hexparser.h
    s2util.h
    s2configstorage.h
    s2datatypes.h
    s2datafactory.h
)

set(sources
    configv.cpp
    dataitem.cpp
    filestruct.cpp
    hexparser.cpp
    s2util.cpp
    s2configstorage.cpp
    s2datafactory.cpp
)


add_library(${PROJECT_NAME} STATIC ${sources} ${headers})

target_link_libraries(${PROJECT_NAME} PRIVATE
  Qt${QT_VERSION_MAJOR}::Core
  PUBLIC gen
)

target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)
