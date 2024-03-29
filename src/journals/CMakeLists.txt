cmake_minimum_required(VERSION 3.14)

project(journals LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Widgets
  REQUIRED)

set(files_h
    basejournal.h
    eventparser.h
    sysjournal.h
    workjournal.h
    measjournal.h
    measparser.h
    journaldialog.h
    journaltabwidget.h
    journalviewer.h
)

set(files_cpp
    basejournal.cpp
    eventparser.cpp
    sysjournal.cpp
    workjournal.cpp
    measjournal.cpp
    measparser.cpp
    journaldialog.cpp
    journaltabwidget.cpp
    journalviewer.cpp
)

add_library(${PROJECT_NAME} STATIC ${files_cpp} ${files_h})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG_JOURNALS)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE xml QXlsx)
