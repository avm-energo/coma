cmake_minimum_required(VERSION 3.14)

project(gen LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpic")
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Network REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Network REQUIRED)

add_library(gen STATIC
    stdfunc.h
    s2.h
    s2helper.h
    datamanager.h
    datarecv.h
    datarecv.cpp
    datatypes.h
    error.h
    error.cpp
    errorqueue.cpp
    errorqueue.cpp
    files.cpp
    files.h
    helper.cpp
    helper.h
    logclass.cpp
    logclass.h
    logger.h
    logger.cpp
    pch.h
    singleton.h
    timefunc.h
    timefunc.cpp
    datamanager.cpp
    stdfunc.cpp
    s2.cpp
)
set_property(TARGET gen PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(gen PRIVATE Qt${QT_VERSION_MAJOR}::Core PRIVATE Qt${QT_VERSION_MAJOR}::Network)
target_compile_features(gen PRIVATE cxx_std_17)
target_compile_definitions(gen PRIVATE INTERFACES_LIBRARY)
