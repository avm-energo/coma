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
    datarecv.h
    filestruct.h
    hexparser.h
    s2.h
    s2configstorage.h
    s2datatypes.h
    s2datafactory.h
)

set(sources
    configv.cpp
    datarecv.cpp
    filestruct.cpp
    hexparser.cpp
    s2.cpp
    s2configstorage.cpp
    s2datafactory.cpp
)


add_library(${PROJECT_NAME} STATIC ${sources} ${headers})

target_link_libraries(${PROJECT_NAME} PRIVATE
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Xml
  Qt${QT_VERSION_MAJOR}::Network
  Qt${QT_VERSION_MAJOR}::Xml
  PUBLIC gen module)

target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)
